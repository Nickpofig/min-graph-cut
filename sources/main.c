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
#include "bignumbers.h"

FILE* open_file(const char* file_path);


int main(int argc, char** args)
{
	struct ProblemInstance instance;
	struct ProblemSolution solution;
	struct timespec start_time;
	struct timespec end_time;
	FILE* file;
	
	// panics when no filepath is given
	if (argc == 1) 
	{
        printf("Panic! expects filepath as second argument.\n");
        exit(-1);
    }

	MPI_Init(&argc, &args);

	// gets process id and count
	int process_id = 0;
	int process_count = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	if (process_id == 0) 
	{
		printf("mpi initialized...\n");
		printf("number of processes: %d\n", process_count);
	}

	// MPI_Barrier(MPI_COMM_WORLD);

#if defined(_OPENMP)
	printf("[P:%d] number of available threads: %d\n", 
		process_id, 
		omp_get_max_threads()
	);
#endif

	file = open_file(args[1]);

	if (file == NULL) 
	{
		printf("Panic! filepath: \"%s\" is not correct", args[1]);
		exit(-1);
	}

	instance = read_problem_instance_from(file);

	clock_gettime(CLOCK_MONOTONIC, &start_time);

	// * recursive OMP approach
	// solution = run_recursive_depth_first_search(&instance);
	
	// * recursive and data parallelism OMP approach  
	// solution = search_best_solution_using_iterational_dps(instance, 4000);

	// * mpi and omp approach
	solution = run_mpi_omp_iterative_brute_force(instance, 10000000);

	// declares master's buffers to recieve data from slaves
	int* solutions = process_id != 0 ? NULL : malloc(sizeof(int) * instance.n * process_count);
	float* costs = process_id != 0 ? NULL : malloc(sizeof(float) * process_count);

	// gathers solution of each process 
	// to the master solution buffer
	MPI_Gather
	(
		solution.array, instance.n, MPI_INT, 
		solutions, instance.n, MPI_INT, 
		0, MPI_COMM_WORLD
	);

	// gathers cost of each process 
	// to the master cost buffer
	MPI_Gather
	(
		&solution.cost, 1, MPI_FLOAT, 
		costs, 1, MPI_FLOAT, 
		0, MPI_COMM_WORLD
	);

	// clears each slave process solution memory
	MPI_Finalize();

	if (process_id != 0) 
	{
		free(solution.array);
		return 0;
	}

	printf("mpi finilized...\n");

	for (int i = 0; i < process_count; i++) 
	{
		if ((costs[i] != -1 && costs[i] < solution.cost) || solution.cost == -1) 
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
	printf("\ninstance { n: %d, a: %d, k: %d}", 
		instance.n, 
		instance.a, 
		instance.k
	);
	printf("\nsolution { cut-cost: %f, solution: ", solution.cost);
	for(int i = 0; i < solution.size; i++)
	{
		printf(" %d", solution.array[i]);
	}
	printf(" }");

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