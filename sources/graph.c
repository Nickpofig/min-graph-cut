#include "graph.h"

bool is_graph_cyclic(const struct Graph* graph) 
{
    // when the graph has no edges
    // it considers as cyclic
    if (graph->node_count == 1 && graph->edge_count == 0) 
    {   
        printf("Info: given graph has no edges, so it considers as cyclic.\n");
        return true;
    }

    // PANICS! when graph does not have any node
    if (graph->node_count == 0) 
    {
        printf("Panic! graph has no nodes!\n");
        exit(-1);
    }

    // declares structure to organize a stack process
    struct Stack 
    {
        int* pointer;
        int capacity;
        int end;
    };

    // declares structure to organize check process
    struct CheckFlag
    {
        int node;
        bool value;
    };

    struct CheckFlag* check_list = malloc
    (
        sizeof(struct CheckFlag) * 
        graph->node_count
    );

    // inits check list
    for (int i = 0; i < graph->node_count; i++) 
    {
        check_list[i] = (struct CheckFlag)
        {
            .node = graph->nodes[i],
            .value = false,
        }; 
    }
    
    struct Stack search_stack = 
    {
        .pointer = malloc(sizeof(int) * 16),
        .capacity = 16,
        .end = 0, 
    };
        
    // pushes the first node into stack
    search_stack.pointer[0] = graph->nodes[0];
    search_stack.end++;

    // logs..
    // if (__logging_is_graph_cyclic) 
    // {
    //     printf("[actions: push(>>) pop(<<) skip(--) search(++) unknow(!!) ]\n");
    //     printf(">>%d  ", graph->nodes[0]);
    // }

    int current_node;

    do 
    {
        // pops the last node from the stack
        search_stack.end--;
        current_node = search_stack.pointer[search_stack.end];

        // logs...
        // if (__logging_is_graph_cyclic) 
        // {
        //     printf("<<%d  ", current_node);
        // }

        // skips the node 
        // when it has been checked 
        for (int i = 0; i < graph->node_count; i++) 
        {
            if (check_list[i].node == current_node)
            {
                if (check_list[i].value == true) 
                {
                    // logs...
                    // if (__logging_is_graph_cyclic) 
                    // {
                    //     printf("--%d  ", current_node);
                    // }

                    goto END_SEARCH;
                }
                else 
                {
                    // marks it in check list
                    check_list[i].value = true;
                    
                    // logs...
                    // if (__logging_is_graph_cyclic) 
                    // {
                    //     printf("++%d  ", current_node);
                    // }

                    goto BEGIN_SEARCH;
                }
            }
        }

        // logs...
        // if (__logging_is_graph_cyclic) 
        // {
        //     printf("!!%d  ", current_node);
        // }

        goto END_SEARCH;

        BEGIN_SEARCH:
        // goes through each edge with that node
        for (int node, i = 0; i < graph->edge_count; i++) 
        {
            if (current_node == graph->edges[i].node_a)
            {
                node = graph->edges[i].node_b;
                goto PUSH;
            }
            if (current_node == graph->edges[i].node_b) 
            {
                node = graph->edges[i].node_a;
                goto PUSH;
            }
            continue;

            PUSH:
            search_stack.pointer[search_stack.end] = node;
            search_stack.end++;
            if (search_stack.end == search_stack.capacity) 
            {
                search_stack.capacity += 16;
                search_stack.pointer = realloc
                (
                    search_stack.pointer, 
                    sizeof(unsigned int) * search_stack.capacity
                );
            }

            // logs...
            // if (__logging_is_graph_cyclic) 
            // {
            //     printf(">>%d  ", node);
            // }
        }
        END_SEARCH:;
    }
    while(search_stack.end > 0);

    bool final_check = true;

    // adds together all node cheks
    // when there is a node 
    // that has not been checked
    // it means that the given graph is not cylic 
    for (int i = 0; i < graph->node_count; i++) 
    {
        final_check = final_check & check_list[i].value;
    }

    // logs...
    // if (__logging_is_graph_cyclic) 
    // {
    //     if (final_check) printf("....yes\n");
    //     else printf("....no\n");
    // }

    free(check_list);
    free(search_stack.pointer);

    return final_check;
}

void print_graph(const struct Graph* graph)
{
    // prints nodes
    printf("nodes[%d]: [", graph->node_count);
    for (int i = 0; i < graph->node_count; i++) 
    {
        if (i > 0) printf(", ");
        printf("%d", graph->nodes[i]);
    }
    printf("]\n");

    // prints edges
    printf("edges[%d]:\n[", graph->edge_count);
    for (int i = 0; i < graph->edge_count; i++) 
    {
        if (i > 0) printf(", ");

        printf("\n%d: ", i);
        print_edge(&graph->edges[i]);
    }
    printf("\n]\n");
}

void print_edge(const struct Edge* edge)
{
    printf
    (
        "(a: %d, b: %d, weight: %f)", 
        edge->node_a, 
        edge->node_b, 
        edge->weight
    );
}