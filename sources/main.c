#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "graph.h"
#include "problem.h"
#include "depth_first_search.h"

FILE* open_file(const char* file_path);


int main(const int argc, const char** args)
{
	__logging_is_graph_cyclic = false;

	// panics when no filepath is given
	if (argc == 1) 
    {
        printf("Panic! expects filepath as first argument.\n");
        exit(-1);
    }

	FILE* file = open_file(args[1]);

	struct ProblemInstance instance = read_problem_instance_from(file);
	//print_problem_instance(&instance);

	struct ProblemSolution solution = run_depth_first_search(&instance);
	printf("Solution: { cut cost: %f, configuration:", solution.cost);
	for(int i = 0; i < solution.size; i++)
		printf(" %d", solution.array[i]);
	printf(" }\n");

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