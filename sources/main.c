#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <omp.h>

#include "graph.h"
#include "problem.h"
#include "depth_first_search.h"
#include "gather_and_search_approach.h"

FILE* open_file(const char* file_path);


int main(const int argc, const char** args)
{
	struct ProblemInstance instance;
	struct ProblemSolution solution;
	struct timespec start_time;
	struct timespec end_time;
	FILE* file;
	
	__logging_is_graph_cyclic = false;

	// panics when no filepath is given
	if (argc == 1) 
	{
        printf("Panic! expects filepath as second argument.\n");
        exit(-1);
    }

#if defined(_OPENMP)
	printf("number of available threads: %d\n", omp_get_max_threads());
#endif

	file = open_file(args[1]);

	instance = read_problem_instance_from(file);
	
	clock_gettime(CLOCK_MONOTONIC, &start_time);

	// solution = run_recursive_depth_first_search(&instance);
	solution = gather_states_and_search_best_solution(instance, 1000);
	
	clock_gettime(CLOCK_MONOTONIC, &end_time);

	// outputs solution
	printf("\n\tinstance { n: %d, a: %d, k: %d}", 
		instance.n, 
		instance.a, 
		instance.k
	);
	printf("\n\tsolution { cut-cost: %f, solution: ", solution.cost);
	for(int i = 0; i < solution.size; i++)
	{
		printf(" %d", solution.array[i]);
	}
	printf("}");

	double ellapsed = (end_time.tv_sec - start_time.tv_sec);
		   ellapsed += (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

	printf
	(
		"\nruntime: %f milliseconds.\n", ellapsed
	);

	free(solution.array);
	free(instance.graph.edges);
}

FILE* open_file(const char* file_path)
{
	FILE* file;

	file = fopen(file_path, "r");

	// panics when file could not be open
	if (file == NULL) 
	{
		printf("Panic: could not open file: %s", file_path);
		exit(-1);
	}

	return file;
}