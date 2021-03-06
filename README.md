# min-graph-cut
The program is written on C language, uses OpenMP for the parallelization. Developed in the Visual Studio Code. 

### Input:  
Value: *file containing data about edges of the initial graph*.

Variables:
- n - number of nodes
- k - maximum node degree(for-now: does not affect porgram, but must be there)
- a - number of nodes in one of the resulted graphs
- e0 - the first node of an edge
- e1 - the second node of an edge
- ew - the weight of an edge

Format:
* first line: `n k a`
* next lines: `e0 e1 ew`

### Output:
Value: *console message*

Variables:
- cut-cost - total weight of the cutted edges.
- solution - binary sequence, 1/0 represents to which cut resulted graph it belongs. 

Format:
    `cut-cost <float number> solution <binary sequence>` 

### Make

Inlcudes a makefile. Type `make help` to get information about available options.

### Visual Studio Code

Includes configurations in the `lanch.json` and `tasks.json` to debug, compile, and test the program.   


<details>
    <summary> Example of an input file </summary>

```
20 10 5
   0    5   0.3810
   0    6   0.9090
   0    8   0.8480
   0   14   0.9380
   0   17   0.9400
   0   19   0.4980
   1    2   0.2240
   1    3   0.1830
   1    4   0.8960
   1    6   0.3730
   1    8   0.8630
   1    9   0.1100
   1   12   0.6830
   1   13   0.2430
   1   17   0.1190
   1   18   0.4860
   2    5   0.7410
   2    7   0.9540
   2    9   0.3180
   2   11   0.3300
   2   18   0.2050
   2   19   0.9300
   3    4   0.5910
   3    5   0.3970
   3    6   0.9520
   3    7   0.8880
   3    8   0.3450
   3    9   0.3140
   3   10   0.5890
   3   11   0.8000
   3   12   0.4510
   3   13   0.2960
   3   15   0.3620
   3   18   0.2140
   4    5   0.5720
   4    6   0.4230
   4    8   0.2920
   4   11   0.9780
   4   15   0.6770
   4   16   0.5260
   4   17   0.4160
   4   19   0.2510
   5    6   0.9500
   5    7   0.9400
   5    8   0.8400
   5    9   0.9030
   5   11   0.1350
   5   14   0.5430
   5   18   0.9400
   5   19   0.6310
   6    7   0.7240
   6    9   0.5890
   6   10   0.2900
   6   11   0.8620
   6   13   0.3980
   6   14   0.3940
   6   17   0.9490
   6   18   0.3570
   7   13   0.7350
   7   18   0.5100
   7   19   0.4520
   8   10   0.3170
   8   11   0.4930
   8   12   0.5820
   8   13   0.1320
   9   10   0.5890
   9   11   0.2130
   9   13   0.8280
   9   14   0.2120
   9   19   0.7940
  10   11   0.4930
  10   12   0.1810
  10   14   0.9010
  10   15   0.9280
  10   16   0.4250
  10   18   0.3880
  11   13   0.1690
  11   14   0.6420
  11   15   0.5480
  11   16   0.9390
  11   17   0.8290
  12   14   0.8680
  12   15   0.1570
  12   16   0.5850
  12   17   0.2380
  12   18   0.8100
  12   19   0.8840
  13   15   0.4540
  13   16   0.5700
  13   18   0.8410
  13   19   0.6790
  14   16   0.5450
  14   18   0.8370
  14   19   0.2170
  16   17   0.5280
  16   18   0.7950
  16   19   0.3470
  17   18   0.4100
  17   19   0.4550
  18   19   0.7340
```
</details>
