// begins header guard
#ifndef __include_problem 
#define __include_problem
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "file_parse.h"
#include "graph.h"

struct ProblemInstance 
{
    int n; // count of nodes in the original graph
    int a; // count of nodes in a resulted graph after cut
    int k; // maximum degree of any node
    struct Graph graph; // graph itself
};

struct ProblemSolution 
{
    int* array;
    int size;
    float cost;
    bool is_valid;
};

struct ProblemInstance read_problem_instance_from(FILE *file);

struct ProblemSolution* malloc_solution(int size, int* array);

void validate_solution(
    struct ProblemSolution* solution,
    const struct ProblemInstance* instance
);

void calculate_cut_cost(
    struct ProblemSolution* solution,
    const struct ProblemInstance* instance
);

void deeply_copy_solution
(
    const struct ProblemSolution* from,
    struct ProblemSolution* into 
);

void clear_problem_solution(struct ProblemSolution *solution);
void print_problem_instance(const struct ProblemInstance* instance);
void print_problem_solution(const struct ProblemSolution* solution);
void minimally_print_problem_solution(const struct ProblemSolution* solution);

#endif // ends header guard