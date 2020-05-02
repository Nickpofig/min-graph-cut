// begins header guard
#ifndef __inlcude_graph
#define __inlcude_graph
// 

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct Edge 
{
    int node_a;
    int node_b;
    float weight; 
};

struct Graph 
{
    int node_count;
    int edge_count;
    int* nodes;
    struct Edge* edges;
};

void print_edge(const struct Edge* edge);
void print_graph(const struct Graph* graph);
bool is_graph_cyclic(const struct Graph* graph);

#endif // ends header guard 