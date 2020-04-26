// begins header guard
#ifndef __include_depth_first_search
#define __include_depth_first_search
//

#include "problem.h"

struct ProblemSolution run_recursive_depth_first_search(const struct ProblemInstance* instance);

void do_recursive_depth_first_search(
    const struct ProblemInstance* instance,
    struct ProblemSolution* best_solution,
    struct ProblemSolution* current_solution,
    int graph_capacity,
    int depth
);

#endif // ends header guard