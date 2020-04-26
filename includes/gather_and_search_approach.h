// begins header guard
#ifndef __include_gather_and_search_approach 
#define __include_gather_and_search_approach
//

#include <omp.h>

#include "problem.h"
#include "graph.h"

struct LinearStateSpace 
{
    struct ProblemInstance instance;
    int capacity;
    struct ProblemSolution* solutions;
    int* sequences;
    int index;
};

struct ProblemSolution search_best_solution_using_iterational_dps
(
    const struct ProblemInstance instance, 
    const int state_space_capacity
);

struct ProblemSolution* get_best_solution_from
(
    const struct LinearStateSpace* space
);

void build_states_and_select_best_of_them_recursivly
(
    struct LinearStateSpace* space,
    struct ProblemSolution* best_solution,
    struct ProblemSolution* current_solution,
    int target_graph_capacity, 
    int depth
);

#endif // ends header guard