//#define __use_parallel_approach

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <omp.h>
#ifdef __include_mpi
#include <mpi.h>
#endif

#include "graph.h"
#include "problem.h"
#include "bignumbers.h"
#include "depth_first_search.h"
#include "gather_and_search_approach.h"
#include "mpi_approach.h"
#include "sequential_approach.h"

// declares main functions
FILE* open_file(const char* file_path);

#ifdef __include_mpi
struct ProblemSolution find_solution_in_parallel
(
	int argc, char** args, 
	struct ProblemInstance instance, 
	unsigned int iteration_size
);
#endif

struct ProblemSolution find_solution_sequentially
(
	struct ProblemInstance instance, 
	unsigned int iteration_size
);
// ends declaration...


int main(int argc, char** args)
{
	struct ProblemInstance instance;
	struct ProblemSolution solution;
	clock_t system_start_clock;
	clock_t system_end_clock;
	FILE* file;
	
	// panics when no filepath is given
	if (argc == 1) 
	{
        printf("Panic! expects filepath as second argument.\n");
        exit(-1);
    }

	file = open_file(args[1]);

	if (file == NULL) 
	{
		printf("Panic! filepath: \"%s\" is not correct", args[1]);
		exit(-1);
	}

	instance = read_problem_instance_from(file);

	system_start_clock = clock();
	#ifdef __include_mpi
	double process_time = MPI_Wtime(); 
	#endif

	#ifdef __include_mpi
	solution = find_solution_in_parallel(argc, args, instance, 10000000);
	#else
	solution = find_solution_sequentially(instance, 10000000);
	#endif

	system_end_clock = clock();
	#ifdef __include_mpi
	process_time = MPI_Wtime() - process_time;
	#endif

	// outputs solution
	printf("\ninstance { n: %d, a: %d, k: %d }", 
		instance.n, 
		instance.a, 
		instance.k
	);
	printf("\nsolution { cut-cost: %f, split: ", solution.cost);
	for(int i = 0; i < solution.size; i++)
	{
		printf(" %d", solution.array[i]);
	}
	printf(" }");

	double ellapsed_seconds = (double) (system_end_clock - system_start_clock) / CLOCKS_PER_SEC; 

	#ifdef __include_mpi
	printf("\nsystem time: %f seconds.\nreal time: %f seconds.\n", ellapsed_seconds, process_time);
	#else
	printf("\nreal time: %f seconds.\n", ellapsed_seconds);
	#endif

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

#ifdef __include_mpi
struct ProblemSolution find_solution_in_parallel
(
	int argc, char** args, 
	struct ProblemInstance instance,
	unsigned int iteration_size
)
{
	struct ProblemSolution solution;

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

#if defined(_OPENMP)
	printf("[P:%d] number of available threads: %d\n", 
		process_id, 
		omp_get_max_threads()
	);
#endif

	// * recursive OMP approach
	// solution = run_recursive_depth_first_search(&instance);
	
	// * recursive and data parallelism OMP approach  
	// solution = search_best_solution_using_iterational_dps(instance, 4000);

	// * mpi and omp approach
	solution = run_mpi_omp_iterative_brute_force(instance, iteration_size);

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

	// ends up connection with other processes
	MPI_Finalize();

	if (process_id != 0) 
	{
		free(solution.array);
		exit(0);
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

	return solution;
}
#endif

struct ProblemSolution find_solution_sequentially
(
	struct ProblemInstance instance, unsigned int iteration_size
)
{
	return run_iterative_brute_force(instance, iteration_size);	
}