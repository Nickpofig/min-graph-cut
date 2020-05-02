// begins header guard
#ifndef __include_mpi_approach
#define __include_mpi_approach
//

#include "problem.h"

struct ProblemSolution run_mpi_omp_iterative_brute_force
(
    const struct ProblemInstance instance,
    unsigned int iteration_size 
);

#endif // ends header guard