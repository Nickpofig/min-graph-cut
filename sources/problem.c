#include "problem.h"

struct ProblemInstance read_problem_instance_from(FILE *file) 
{
    // declares instance variable
    struct ProblemInstance instance =  
    {
        .n = 0,
        .a = 0,
        .k = 0,
        .graph = 
        { 
            .node_count = 0,
            .edge_count = 0,
            .nodes = NULL, 
            .edges = NULL
        },
    };

    // reads n (count of nodes in the original graph)
    instance.n = atoi(read_word_from(file));
    instance.graph.node_count = instance.n;
    
    // sets node indecies.
    instance.graph.nodes = malloc(sizeof(int) * instance.n);
    for (int i = 0; i < instance.n; i++)
    {
        instance.graph.nodes[i] = i;
    }

    // reads k (maximum degree of any node)
    instance.k = atoi(read_word_from(file));
    // reads a (count of nodes in a graph getted from cut of the original graph)
    instance.a = atoi(read_word_from(file));

    const size_t edge_size = sizeof(struct Edge);
    const int graph_grow_step = 16;
    
    char* word;
    int graph_capacity = graph_grow_step;
    int edge_count = 0;
    
    // allocates graph 
    instance.graph.edge_count = 0;
    instance.graph.edges = malloc(graph_capacity * edge_size);

    // reads edges:
    while(true)
    {
        // reads a node "a" index
        word = read_word_from(file);

        if (word == NULL) 
        {
            // the behaviour is alright, meaning that:
            // when the "first" word is null
            // than stop edge parsing 
            // because no edges are left
            break;
        }
        // parses node "a" from string to float
        instance.graph.edges[edge_count].node_a = atoi(word);

        // reads a node "b" index
        word = read_word_from(file);

        if (word == NULL) 
        {
            printf("Error: Expected \"node index\", found nothing. Invalid format of problem instance file.\n");
            exit(-1);
        }
        // parses node "b" from string to float
        instance.graph.edges[edge_count].node_b = atoi(word);

        // reads a weight
        word = read_word_from(file);

        if (word == NULL) 
        {
            printf("Error: Expected \"weight\", found nothing. Invalid format of problem instance file.\n");
            exit(-1);
        }
        // parses weight from string to float
        instance.graph.edges[edge_count].weight = atof(word);

        // continues counting
        edge_count++;
 
        // expands graph's edge capacity
        // when limit has been reached 
        if (edge_count == graph_capacity) 
        {
            graph_capacity += graph_grow_step;
            instance.graph.edges = realloc
            (
                instance.graph.edges, 
                graph_capacity * edge_size
            );
        }
    }

    instance.graph.edge_count = edge_count;

    // ensures that graph's edge size 
    // equals to the count of edges
    if (edge_count != graph_capacity) 
    {
        instance.graph.edges = realloc
        (
            instance.graph.edges,
            edge_count * edge_size
        );
    }

    return instance;
}

void validate_solution
(
    struct ProblemSolution* solution,
    const struct ProblemInstance* instance
)
{
    solution->is_valid = false;

    // checks for valid size of the graph[a]
    int target_graph_size = instance->a;
    for (int i = 0; i < instance->n; i++) 
    {
        if (solution->array[i] == 1) 
        {
            target_graph_size--;
        }
    }

    // returns false when graph[a] has insufficient size
    if (target_graph_size > 0) return;

    // inits graph[a]
    struct Graph graph_a = 
    {
        .node_count = instance->a,
        .edge_count = 0,
        .nodes = malloc(sizeof(int) * instance->a),
        .edges = malloc(sizeof(struct Edge) * 16)
    };

    // inits graph[b]
    struct Graph graph_b = 
    {
        .node_count = instance->n - instance->a,
        .edge_count = 0,
        .nodes = malloc(sizeof(int) * (instance->n - instance->a)),
        .edges = malloc(sizeof(struct Edge) * 16)
    };

    // fills graphs
    for (int a = 0, b = 0, i = 0; i < instance->n; i++) 
    {
        if (solution->array[i] == 1) 
        {
            graph_a.nodes[a] = instance->graph.nodes[i];
            a++;
        }
        else 
        {
            graph_b.nodes[b] = instance->graph.nodes[i];
            b++;
        }
    }

    struct Edge edge;

    // populates both graph[a] and graph[b]
    for (int node_a, node_b, i = 0; i < instance->graph.edge_count; i++) 
    {
        edge = instance->graph.edges[i];
        node_a = solution->array[edge.node_a];
        node_b = solution->array[edge.node_b];  

        if (node_a == 1 && node_b == 1) 
        {
            // adds the edge to the graph [a]
            graph_a.edges[graph_a.edge_count] = edge;
            graph_a.edge_count++;

            // reallocate edges sequence of graph[a] 
            // when its capacity has been reached 
            if (graph_a.edge_count % 16 == 0) 
            {
                graph_a.edges = realloc
                (
                    graph_a.edges, 
                    sizeof(struct Edge) * 
                    (graph_a.edge_count + 16)
                );
            }
        }
        else if (node_a == 0 && node_b == 0) 
        {
            // adds the edge to the graph [a]
            graph_b.edges[graph_b.edge_count] = edge;
            graph_b.edge_count++;

            // reallocate edges sequence of graph[a] 
            // when its capacity has been reached 
            if (graph_b.edge_count % 16 == 0) 
            {
                graph_b.edges = realloc
                (
                    graph_b.edges, 
                    sizeof(struct Edge) * 
                    (graph_b.edge_count + 16)
                );
            }
        }
    }

    // get rid of the graphs' memory excess
    // for valid <is_graph_cyclic> function call 
    graph_a.edges = realloc(graph_a.edges, sizeof(struct Edge) * graph_a.edge_count);
    graph_b.edges = realloc(graph_b.edges, sizeof(struct Edge) * graph_b.edge_count);
    
    // checks for solution's graphs being cyclic
    solution->is_valid = is_graph_cyclic(&graph_a) && is_graph_cyclic(&graph_b);
    
    // printf("...\ngraphs are cyclic: ");
    // if (solution->is_valid) printf("[yes]\n");
    // else printf("[no]\n");

    // clears memory on the heap 
    free(graph_a.edges);
    free(graph_a.nodes);
    free(graph_b.edges);
    free(graph_b.nodes);
}

void calculate_cut_cost
(
    struct ProblemSolution* solution,
    const struct ProblemInstance* instance
)
{
    struct Edge edge;
    float cost = 0;

    for (int i = 0; i < instance->graph.edge_count; i++) 
    {
        edge = instance->graph.edges[i];

        if (solution->array[edge.node_a] != solution->array[edge.node_b]) 
        {
            cost += edge.weight;
        }
    }

    solution->cost = cost;
}

void deeply_copy_solution
(
    const struct ProblemSolution* from,
    struct ProblemSolution* into 
) 
{
    into->cost = from->cost;
    into->is_valid = from->is_valid;
    into->size = from->size;

    for (int i = 0; i < from->size; i++) 
    {
        into->array[i] = from->array[i];
    }
}

struct ProblemSolution* malloc_solution(int size, int* array)
{
    struct ProblemSolution* solution = malloc(sizeof(struct ProblemSolution));
    
    solution->size = size;
    solution->cost = 0;
    solution->is_valid = false;
    solution->array = array == NULL ? malloc(sizeof(int) * size) : array; 

    for (int i = 0; i < size; i++) 
    {
        solution->array[i] = 0;
    }

    return solution;
}

void print_problem_instance(const struct ProblemInstance* instance) 
{
    printf
    (
        "(n:%d, a: %d, k: %d, graph: { ", 
        instance->n,
        instance->a,
        instance->k
    );
    print_graph(&instance->graph);
    printf("})");
}

void print_problem_solution(const struct ProblemSolution* solution) 
{
    printf("(cost: %f", solution->cost);
    printf(", is-valid: ");
    if (solution->is_valid) printf("true");
    else printf("false");
    printf(", cut: [ ");
    for (int i = 0; i < solution->size; i++) 
    {
        if (i > 0) printf(", ");
        printf("%d", solution->array[i]);
    }
    printf("])");
}

void minimally_print_problem_solution(const struct ProblemSolution* solution)
{
    int a = 0;
    for (int i = 0; i < solution->size; i++) 
    {
        if (solution->array[i] == 1)
        {
            a++;
            printf("!");
        }
        else printf(".");
    }
    printf("[%d]", a);
}