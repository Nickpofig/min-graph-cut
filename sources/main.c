#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

#include "graph.h"
#include "problem.h"
#include "depth_first_search.h"
#include "gather_and_search_approach.h"
#include "mpi_approach.h"

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

	MPI_Init(&argc, &args);
	
	// * recursive OMP approach
	// solution = run_recursive_depth_first_search(&instance);
	
	// * recursive and data parallelism OMP approach  
	// solution = search_best_solution_using_iterational_dps(instance, 4000);
	
	// * mpi and omp approach
	solution = run_mpi_omp_iterative_brute_force(instance, 10000000);

	// declares master buffers to recieve data from slaves
	int* solutions = NULL;
	float* costs = NULL;

	// gets process id and count
	int process_id = 0;
	int process_count = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	if (process_id == 0) 
	{
		solutions = malloc(sizeof(int) * instance.n * process_count);
		costs = malloc(sizeof(float) * process_count);
	}

	// gathers solution of each process 
	// to the master solution buffer
	MPI_Gather
	(
		&solution.array, instance.n, MPI_INT, 
		&solutions, instance.n, MPI_INT, 
		0, MPI_COMM_WORLD
	);

	// gathers cost of each process 
	// to the master cost buffer
	MPI_Gather
	(
		&solution.cost, 1, MPI_INT, 
		&costs, 1, MPI_INT, 
		0, MPI_COMM_WORLD
	);

	// clears each slave process solution memory
	if (process_id != 0) free(solution.array);
	MPI_Finalize();

	for (int i = 0; i < process_count; i++) 
	{
		if (costs[i] > solution.cost) 
		{
			solution.cost = costs[i];
			for (int j = 0, offset = i * instance.n; j < instance.n; j++) 
			{
				solution.array[j] = solutions[j + offset];
			}
		}
	}

	free(solutions);
	free(costs);

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