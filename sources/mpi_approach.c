#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>

#include "problem.h"
#include "bignumbers.h"
#include "mpi_approach.h"
#include "log.h"

struct ProblemSolution run_mpi_omp_iterative_brute_force
(
    const struct ProblemInstance instance,
    unsigned int iteration_size 
)
{
    // CONST:
    const unsigned int sinle_block_length = sizeof(unsigned int) * 8;

    // asserts iteration size is valid
    if (iteration_size == 0) 
    {
        printf("Error! iteration size could not be zero!");
        exit(-1);
    }

    // asserts MPI has been initialized
    int is_mpi_initialized = 1;
    if (MPI_Initialized(&is_mpi_initialized) != MPI_SUCCESS) 
    {
        printf("Panic! MPI Initialization check call has not been successful!\n");
        exit(-1);
    }
    else if (!is_mpi_initialized) 
    {
        printf("Panic! MPI has not been initialized!\n");
        exit(-1);
    }

    // ensures that iteration size is less or equal 
    // to total number of possible states
    if (instance.n <= sizeof(unsigned int) * 8) 
    {
        unsigned int number_of_possible_states = ~0;
        int excess = (sizeof(unsigned int) * 8) - instance.n;
        if (excess > 0) number_of_possible_states = (number_of_possible_states << excess) >> excess;
        
        if (iteration_size > number_of_possible_states) 
            iteration_size = number_of_possible_states;
    }

    // calculates how much bytes a single state would occupy
    int state_memory_blocks = instance.n / (sizeof(unsigned int) * 8);
    if (instance.n % sizeof(unsigned int) != 0 || state_memory_blocks == 0)
        state_memory_blocks++;

    // printf("single state memory blocks size: %d\n", state_memory_blocks);

    int process_id = 0;
    int process_count = 0;

    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    // declares variables to store the best founded state of this process   
    struct ProblemSolution best_solution = 
    {
        .array = malloc(sizeof(int) * instance.n),
        .size = instance.n,
        .cost = -1,
        .is_valid = false
    };
    clear_problem_solution(&best_solution);

    // declares variables to iterate over whole state space
    unsigned int* end_state = alloc_big_natural_number_from_power(state_memory_blocks, instance.n);   
    unsigned int* start_state = alloc_big_natural_number(state_memory_blocks, 0); 
    
    unsigned int iteration_step = iteration_size / process_count;
    
    // the last process handles excess of states
    if( (process_id == process_count - 1) && 
        (iteration_size % process_count != 0)) 
    {
        iteration_step += iteration_size - iteration_step * process_count;
    }

    const int thread_count = omp_get_max_threads();
    int search_complete_signal = 0;

    while(   
        compare_big_natural_numbers
        (
            start_state, state_memory_blocks, 
            end_state, state_memory_blocks
        ) == -1
    ) 
    {
        // [ move ]s this process [ start state ] 
        // to the state space it need to process
        for (int i = 0; i < process_id; i++) 
        {
            add_value_to_big_natural_number
            (
                start_state, state_memory_blocks, iteration_step
            );
        }

        // printf("[P:%d] total number of iterations: %d\n", process_id, iteration_step);

        #pragma omp parallel 
        { 
            unsigned int thread_id = omp_get_thread_num();
            unsigned long thread_iteration_steps = iteration_step / thread_count;
            
            struct ProblemSolution temp_solution = 
            {
                .array = malloc(sizeof(int) * instance.n),
                .size = instance.n,
                .cost = -1,
                .is_valid = false,
            };

            // printf("[P:%d, T:%d] begins...\n", process_id, thread_id);

            unsigned int* thread_state = alloc_big_natural_number
            (
                state_memory_blocks, 
                thread_iteration_steps * thread_id
            );

            // shifts thread state to its first appropriate position
            add_big_natural_numbers
            (
                thread_state, state_memory_blocks,
                start_state, state_memory_blocks
            );
            
            // the last thread handles excess of states
            if (thread_id == thread_count - 1 && iteration_step % thread_count != 0)
            {
                thread_iteration_steps += iteration_step - thread_iteration_steps * thread_count;
            }

            for (int i = 0, a; i <= thread_iteration_steps; i++) 
            {
                if (compare_big_natural_numbers
                    (
                        thread_state, state_memory_blocks,
                        end_state, state_memory_blocks
                    ) >= 0
                ) 
                {
                    #pragma omp atomic write
                    search_complete_signal = 1;
                    break;
                }

                a = instance.a;

                for (int j = 0, blc = 0; blc < state_memory_blocks; blc++) 
                {
                    if (j == instance.n) break;

                    for (int bit = 0; bit < sinle_block_length; bit++, j++)
                    {
                        if (j == instance.n) break;

                        if (thread_state[blc] & 1 << bit) 
                        {
                            temp_solution.array[j] = 1;
                            a--;
                        }
                        else temp_solution.array[j] = 0;
                    }
                }

                // moves to the next state
                add_value_to_big_natural_number
                (
                    thread_state, state_memory_blocks, 1
                );

                if (a != 0)
                {
                    continue;
                }

                // validates state
                validate_solution(&temp_solution, &instance);
                
                if (temp_solution.is_valid) 
                {
                    // calculates cost
                    calculate_cut_cost(&temp_solution, &instance);

                    // when cost is less than best cost...
                    if (temp_solution.cost < best_solution.cost || 
                        best_solution.cost == -1)
                    {
                        // writes this state as best one
                        #pragma omp critical
                        {
                            deeply_copy_solution(&temp_solution, &best_solution);
                        }
                    }
                }
            }

            // printf("[P:%d, T:%d] ends...\n", process_id, thread_id);

            free(thread_state);
            free(temp_solution.array);
        }

        if (search_complete_signal) break;

        // alligns this process [ start state ]
        // with the farest [ start state ]
        // to synchronize with next iterational [ move ] action
        for (int i = process_id + 1; i < process_count; i++) 
        {
            add_value_to_big_natural_number
            (
                start_state, state_memory_blocks, iteration_step
            );
        }
    }

    printf("[P:%d] result: ", process_id);
    print_problem_solution(&best_solution);
    printf("\n");

    free(start_state);
    free(end_state);

    return best_solution;
}