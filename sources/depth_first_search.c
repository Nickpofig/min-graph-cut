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
    struct ProblemSolution blank_solution = 
    {
        .array = malloc(sizeof(int) * instance->n),
        .size = instance->n,
        .cost = 0,
        .is_valid = false,
    };

    for (int i = 0; i < instance->n; i++) 
    {
        blank_solution.array[i] = 0;
    }

    #pragma omp parallel
    {
        do_depth_first_search(instance, &best_solution, &blank_solution, instance->a, 0);
    }
    #pragma omp barrier

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
    struct ProblemSolution include_solution;
    struct ProblemSolution exclude_solution;

    int thread =
    #if defined(_OPENMP)
        omp_get_thread_num(); 
    #else
        0;
    #endif

    printf("[%d] start.\n", thread);

    if (graph_capacity == 0 || depth == instance->n) 
    {
        printf("[%d] try calculate cost.\n", thread);

        calculate_cut_cost(current_solution, instance);

        printf("[%d] cost calculated.\n", thread);

        // ~ ~ ~ future: atomic check
        if (best_solution->is_valid == false || 
            best_solution->cost > current_solution->cost) 
        {
            printf("[%d] try validate\n", thread);

            validate_solution(current_solution, instance);
            
            printf("[%d] valiadted\n", thread);

            if (current_solution->is_valid)
            {
                printf("[%d] write best\n", thread);
                // ~ ~ ~ future: atomic setd
                #pragma omp critical 
                {
                    best_solution->cost = current_solution->cost;                
                    
                    best_solution->is_valid = true;

                    for (int i = 0; i < instance->n; i++) 
                    {
                        best_solution->array[i] = current_solution->array[i];
                    }
                }                
            }
        }

        printf("[%d] try free\n", thread);
        #pragma omp critical 
        {
            free(current_solution->array);
        }
        printf("[%d] free complete\n", thread);
        return;
    }

    // printf("\td: %d", depth);
	// for(int i = 0; i < current_solution->size; i++)
	// {
    //     if (current_solution->array[i] == 1) { printf("!");}
    //     else printf(".");
	// }
    // printf("\t");

    #if defined(_OPENMP)
        printf("thread: %d\n", omp_get_thread_num());
    #endif

    printf("[%d] malloc sol.1\n", thread);
    #pragma omp critical 
    {
        include_solution = (struct ProblemSolution)
        {
            .array = malloc(sizeof(int) * instance->n),
            .size = instance->n,
            .cost = 0,
            .is_valid = false
        };
    }

    printf("[%d] malloc sol.2\n", thread);
    #pragma omp critical 
    {
        exclude_solution = (struct ProblemSolution)
        {
            .array = malloc(sizeof(int) * instance->n),
            .size = instance->n,
            .cost = 0,
            .is_valid = false
        };
    }

    printf("[%d] write sols\n", thread);
    for (int i = 0; i < instance->n; i++) 
    {
        include_solution.array[i] = current_solution->array[i];
        exclude_solution.array[i] = current_solution->array[i];
    }

    printf("[%d] free parent sol\n", thread);
    #pragma omp critical 
    {
        free(current_solution->array);
    }

    include_solution.array[depth] = 1;
    exclude_solution.array[depth] = 0;

    depth++;

    printf("[%d] search left\n", thread);
    #pragma omp task
    {
        do_depth_first_search(instance, best_solution, &include_solution, graph_capacity - 1, depth);
    }
    printf("[%d] search right\n", thread);
    #pragma omp task 
    {
        do_depth_first_search(instance, best_solution, &exclude_solution, graph_capacity    , depth);
    }
}