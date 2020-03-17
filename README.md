# min_graph_cut
The program is written on C language. Developed using VS Code IDE. 

Input: *file containing data about edges of the initial graph*. 

- n - number of nodes
- k - maximum node degree(for-now: does not affect porgram, but must be there)
- a - number of nodes in one of the resulted graphs
- e0 - the first node of an edge
- e1 - the second node of an edge
- ew - the weight of an edge

Format:
* first line: `n k a`
* next lines: `e0 e1 ew`

> example file: mhr_20_10_5.txt

Output: *console message*

- cut-cost - total weight of the cutted edges.
- solution - binary sequence, 1/0 represents to which cut resulted graph it belongs. 

Format:
    `cut-cost <float number> solution <binary sequence>` 
