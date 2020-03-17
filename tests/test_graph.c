#include "graph.h"

void test_is_graph_cyclic();

int main()
{
    test_is_graph_cyclic();
}

void test_is_graph_cyclic()
{
    printf("staring graph cyclic test...\n");

    struct Graph graph = 
    {
        .node_count = 4,
        .edge_count = 4,
        .nodes = malloc(sizeof(int) * 4),
        .edges = malloc(sizeof(struct Edge) * 4)
    };

    // inits nodes
    graph.nodes[0] = 0;
    graph.nodes[1] = 1;
    graph.nodes[2] = 2;
    graph.nodes[3] = 3;

    // inits edges
    graph.edges[0] = (struct Edge) 
    {
        .node_a = 0,
        .node_b = 1,
        .weight = 1
    };

    graph.edges[1] = (struct Edge) 
    {
        .node_a = 1,
        .node_b = 2,
        .weight = 1
    };

    graph.edges[2] = (struct Edge) 
    {
        .node_a = 1,
        .node_b = 3,
        .weight = 1
    };

    graph.edges[3] = (struct Edge) 
    {
        .node_a = 2,
        .node_b = 3,
        .weight = 1
    };

    printf("Graph:\n");
    print_graph(&graph);

    bool is_cyclic = is_graph_cyclic(&graph);

    printf("\nGiven graph is cyclic: ");
    if (is_cyclic) 
    {
        printf("[true]\n");
    }
    else 
    {
        printf("[false]\n");
    }
    printf("\n[Done]\n");
}