#include "gather_and_search_approach.h"

struct ProblemSolution gather_states_and_search_best_solution
(
    const struct ProblemInstance instance, 
    const int state_space_capacity
)
{
    // allocate state space
    struct LinearStateSpace space = 
    {
        .instance = instance,
        .solutions = malloc(sizeof(struct ProblemSolution) * state_space_capacity),
        .sequences = malloc(sizeof(int) * instance.n * state_space_capacity),
        .capacity = state_space_capacity,
        .index = 0
    };

    // clear state space
    for (int i = 0; i < state_space_capacity; i++) 
    {
        space.solutions[i].cost = 0;
        space.solutions[i].is_valid = false;
        space.solutions[i].size = instance.n;
        space.solutions[i].array = &space.sequences[i * instance.n];

        for (int j = 0; j < instance.n; j++) 
        {
            space.solutions[i].array[j] = 0;
        }
    }

    // allocate best and root solution
    struct ProblemSolution* root_solution = malloc_solution(instance.n, NULL);
    root_solution->size = instance.n;
    root_solution->is_valid = false;
    root_solution->cost = 0;

    struct ProblemSolution best_solution = 
    {
        .array = malloc(sizeof(int) * instance.n),
        .size = instance.n,
        .is_valid = false,
        .cost = 0
    };

    printf("start");

    // start search for the best solution
    build_states_and_select_best_of_them_recursivly
    (
        &space, 
        &best_solution, 
        root_solution,
        instance.a,
        0
    );

    return best_solution;
}

void build_states_and_select_best_of_them_recursivly
(
    struct LinearStateSpace* space,
    struct ProblemSolution* best_solution,
    struct ProblemSolution* current_solution,
    int target_graph_capacity, 
    int depth
) 
{
    struct ProblemSolution* include_solution;
    struct ProblemSolution* exclude_solution;
    struct ProblemSolution* local_best_solution;

    // when the current state space is full
    // search for better solution 
    if (space->index == space->capacity) 
    {
        local_best_solution = get_best_solution_from(space);
        
        if (best_solution->is_valid == false || 
            best_solution->cost > local_best_solution->cost) 
        {
            deeply_copy_solution(local_best_solution, best_solution);
            printf("\n[best] cost: %f, cut: ", local_best_solution->cost);
            minimally_print_problem_solution(local_best_solution);
        }
        
        free(local_best_solution->array);
        free(local_best_solution);

        space->index = 0;
    }

    // when there is clearly no valid solution
    // abort search for this state space branch
    if (space->instance.n - depth < target_graph_capacity) 
    {
        return;
    }

    // when state is complete
    // add it into the state space
    if (target_graph_capacity == 0 || depth == space->instance.n) 
    {
        deeply_copy_solution(current_solution, &space->solutions[space->index]);

        free(current_solution->array);
        free(current_solution);

        space->index++;

        return;
    }

    // a solution, which inlcudes graph vertex at index of current depth  
    include_solution = malloc_solution(current_solution->size, NULL);
    
    // a solution, which does not inlcude graph vertex at index of current depth
    exclude_solution = malloc_solution(current_solution->size, NULL);
    
    deeply_copy_solution(current_solution, include_solution);
    deeply_copy_solution(current_solution, exclude_solution);

    include_solution->array[depth] = 1;
    exclude_solution->array[depth] = 0;

    free(current_solution->array);
    free(current_solution);

    build_states_and_select_best_of_them_recursivly
    (
        space, 
        best_solution,
        include_solution, 
        target_graph_capacity - 1, 
        depth + 1
    );

    build_states_and_select_best_of_them_recursivly
    (
        space, 
        best_solution,
        exclude_solution, 
        target_graph_capacity, 
        depth + 1
    );
}

struct ProblemSolution* get_best_solution_from
(
    const struct LinearStateSpace* space
)
{
    int thread_count = 
    #if defined(_OPENMP)
        omp_get_max_threads();
    #else
        1;
    #endif 

    struct ProblemSolution* best_solutions = malloc
    (
        sizeof(struct ProblemSolution) * thread_count
    );
    
    for (int i = 0; i < thread_count; i++) 
    {
        best_solutions[i].array = malloc(sizeof(int) * space->instance.n);
        best_solutions[i].size = space->instance.n;
        best_solutions[i].is_valid = false; 
        best_solutions[i].cost = 0;
    }

    #pragma omp parallel for 
    for (int i = 0; i < space->index; i++) 
    {
        int thread = omp_get_thread_num();
        
        calculate_cut_cost(&space->solutions[i], &space->instance);

        if (best_solutions[thread].is_valid == false || 
            best_solutions[thread].cost > space->solutions[i].cost) 
        {
            // temporary allocates memory on the heap 
            validate_solution(&space->solutions[i], &space->instance);

            if (space->solutions[i].is_valid)
            {                
                best_solutions[thread].is_valid = true;
                best_solutions[thread].cost = space->solutions[i].cost;                

                for (int j = 0; j < space->instance.n; j++) 
                {
                    best_solutions[thread].array[j] = space->solutions[i].array[j];
                }
            }
        }
    }

    #if defined(_OPENMP)
        struct ProblemSolution* result_solution = malloc_solution(space->instance.n, NULL);
        deeply_copy_solution(&best_solutions[0], result_solution);
        
        for (int i = 1; i < thread_count; i++) 
        {
            if (best_solutions[i].is_valid && 
                best_solutions[i].cost < result_solution->cost) 
            {
                deeply_copy_solution(&best_solutions[i], result_solution);
            }
        }
        
        free(best_solutions);
        return result_solution;
    #else
        return best_solutions[0];
    #endif
}