#include <stdlib.h>
#include <stdio.h>

#include "sequential_approach.h"
#include "bignumbers.h"
#include "problem.h"
#include "log.h"

struct ProblemSolution run_iterative_brute_force
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

    // declares variables to store the best founded state of this process   
    struct ProblemSolution best_solution = 
    {
        .array = malloc(sizeof(int) * instance.n),
        .size = instance.n,
        .cost = -1,
        .is_valid = false
    };
    clear_problem_solution(&best_solution);
    
    struct ProblemSolution temp_solution = 
    {
        .array = malloc(sizeof(int) * instance.n),
        .size = instance.n,
        .cost = -1,
        .is_valid = false,
    };

    // declares variables to iterate over whole state space
    unsigned int* end_state = alloc_big_natural_number_from_power(state_memory_blocks, instance.n);   
    unsigned int* current_state = alloc_big_natural_number(state_memory_blocks, 0); 
    
    while(  
        compare_big_natural_numbers
        (
            current_state, state_memory_blocks, 
            end_state, state_memory_blocks
        ) <= 0
    ) 
    {   
        int a = instance.a;

        // writes bits of current state into the temp solution
        for (int j = 0, blc = 0; blc < state_memory_blocks; blc++) 
        {
            if (j == instance.n) break;

            for (int bit = 0; bit < sinle_block_length; bit++, j++)
            {
                if (j == instance.n) break;

                if (current_state[blc] & 1 << bit) 
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
            current_state, state_memory_blocks, 1
        );

        // when state is invalid...
        if (a != 0)
        {
            // moves to the next state
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

    // printf("best: ");
    // print_problem_solution(&best_solution);
    // printf("\n");

    free(temp_solution.array);
    free(current_state);
    free(end_state);

    return best_solution;
}