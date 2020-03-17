#include <stdio.h>

#include "problem.h"

int main(const int argc, const char** args) 
{
    if (argc == 1) 
    {
        printf("Panic! expects filepath as first argument.");
        exit(-1);
    }

    FILE* file = fopen(args[1], "r");

    if (file == NULL) 
    {
        printf("Panic! could not open file at the path: %s", args[1]);
        exit(-1);
    }

    struct ProblemInstance instance = read_problem_instance_from(file);

	printf("problem instance: ");
	print_problem_instance(&instance);
	printf("\n [Done]");

	free(instance.graph.edges);
}