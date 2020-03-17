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

    do_depth_first_search(instance, &best_solution, &blank_solution, instance->a, 0);

    if (best_solution.array == NULL || best_solution.is_valid == false) 
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
    if (graph_capacity == 0 || depth == instance->n) 
    {
        calculate_cut_cost(current_solution, instance);

        // ~ ~ ~ future: atomic check
        if (best_solution->is_valid == false || 
            best_solution->cost > current_solution->cost) 
        {
            validate_solution(current_solution, instance);
            
            if (current_solution->is_valid)
            {
                // ~ ~ ~ future: atomic set
                best_solution->cost = current_solution->cost;                
                best_solution->is_valid = true;

                for (int i = 0; i < instance->n; i++) 
                    best_solution->array[i] = current_solution->array[i];
            }
        }

        free(current_solution->array);
        return;
    }

    struct ProblemSolution include_solution = 
    {
        .array = malloc(sizeof(int) * instance->n),
        .size = instance->n,
        .cost = 0,
        .is_valid = false
    };

    struct ProblemSolution exclude_solution = 
    {
        .array = malloc(sizeof(int) * instance->n),
        .size = instance->n,
        .cost = 0,
        .is_valid = false
    };

    for (int i = 0; i < instance->n; i++) 
    {
        include_solution.array[i] = current_solution->array[i];
        exclude_solution.array[i] = current_solution->array[i];
    }

    free(current_solution->array);

    include_solution.array[depth] = 1;
    exclude_solution.array[depth] = 0;

    depth++;

    do_depth_first_search(instance, best_solution, &include_solution, graph_capacity - 1, depth);
    do_depth_first_search(instance, best_solution, &exclude_solution, graph_capacity    , depth);
}