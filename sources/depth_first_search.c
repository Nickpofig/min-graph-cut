#include <omp.h>
#include <time.h>

#include "problem.h"
#include "depth_first_search.h"

struct ProblemSolution run_depth_first_search(const struct ProblemInstance* instance) 
{
    struct ProblemSolution best_solution = 
    {
        .array = malloc(sizeof(int) * instance->n),
        .size = instance->n,
        .cost = 0,
        .is_valid = false,
    };

    // would be deallocated by do_depth_first_search
    struct ProblemSolution* blank_solution = malloc(sizeof(struct ProblemSolution));
    
    blank_solution->array = malloc(sizeof(int) * instance->n);
    blank_solution->size = instance->n;
    blank_solution->cost = 0;
    blank_solution->is_valid = false;

    for (int i = 0; i < instance->n; i++) 
    {
        blank_solution->array[i] = 0;
    }

    #pragma omp parallel
    {
        #pragma omp single 
        {
            do_depth_first_search(instance, &best_solution, blank_solution, instance->a, 0);
        }
    }

    if (best_solution.is_valid == false) 
    {
        printf("Panic! Depth First Search has faild. Algorithm is not correct.\n");
    }

    // !! we know that at least one cut is exist
    // !! that is why the below pointer dereference must be valid
    return best_solution;
}

void do_depth_first_search
(
    const struct ProblemInstance* instance,
    struct ProblemSolution* best_solution, 
    struct ProblemSolution* current_solution,
    int graph_capacity,
    int depth
)
{
    struct ProblemSolution* include_solution;
    struct ProblemSolution* exclude_solution;

    int thread =
    #if defined(_OPENMP)
        omp_get_thread_num(); 
    #else
        0;
    #endif

    printf("[%d] do-dps:depth(%d)\n", thread, depth);

    if (graph_capacity == 0 || depth == instance->n) 
    {
        calculate_cut_cost(current_solution, instance);

        if (best_solution->is_valid == false || 
            best_solution->cost > current_solution->cost) 
        {
            #pragma omp critical
            {
                // temporary allocates memory on the heap 
                validate_solution(current_solution, instance);

                if (current_solution->is_valid)
                {                
                    best_solution->is_valid = true;
                    best_solution->cost = current_solution->cost;                

                    for (int i = 0; i < instance->n; i++) 
                    {
                        best_solution->array[i] = current_solution->array[i];
                    }
                }
            }
        }
        
        #pragma omp critical 
        {
            free(current_solution->array);
            free(current_solution);
        }
        
        return;
    }

    #pragma omp critical 
    {
        include_solution = malloc(sizeof(struct ProblemSolution));
        include_solution->array = malloc(sizeof(int) * instance->n);
        include_solution->size = instance->n;
        include_solution->cost = 0;
        include_solution->is_valid = false;
    }

    #pragma omp critical 
    {
        exclude_solution = malloc(sizeof(struct ProblemSolution));
        exclude_solution->array = malloc(sizeof(int) * instance->n);
        exclude_solution->size = instance->n;
        exclude_solution->cost = 0;
        exclude_solution->is_valid = false;
    }

    for (int i = 0; i < instance->n; i++) 
    {
        include_solution->array[i] = current_solution->array[i];
        exclude_solution->array[i] = current_solution->array[i];
    }

    #pragma omp critical 
    {
        free(current_solution->array);
        free(current_solution);
    }

    include_solution->array[depth] = 1;
    exclude_solution->array[depth] = 0;

    depth++;

    #pragma omp task
    {
        do_depth_first_search(instance, best_solution, include_solution, graph_capacity - 1, depth);
    }
    #pragma omp task 
    {
        do_depth_first_search(instance, best_solution, exclude_solution, graph_capacity    , depth);
    }
}