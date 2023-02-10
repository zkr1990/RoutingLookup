# RHF_10000
Routing Hash Function(RHF) is used to hash from input addresses to output interface numbers.
Compared with other RTLP approaches, RHF-10000 is designed to support lookup for 32-bit address sets with size = 10000. 

The RHF_10000 design (RHF1) is implemented with respect to hardware constraints and different balance points in construction time, lookup latency and resource cost.

# Implementation
## Compile program
```
cd router_mega
cmake .
make
```
# Step1: generate data set for experiment 0/1/2
We design lookup experiments and verify the performance (mainly in space consumption) of RHF_10000 designs.

Input addresses are generated 1000 times. For each address set, we do the following 3 experiment.
Experiment 0 and 1 are for distributions of mapping.
Experiment 2 is for different output interface count.

## Experiment 0: Simulate different distributions of routing lookups.
-Dataset generation:
There may be various mappings from input address to Output interface numbers(IN).
We first fix the IN from 1-4, which means there are at most 4 INs (A,B,C,D) for the address set.
n_a addresses should be mapped to IN-A.
n_b addresses should be mapped to IN-B.
n_c addresses should be mapped to IN-C.
n_d addresses should be mapped to IN-D.
To save time, n_a, n_b, n_c and n_d is chosen as a multiple of 500 from 0 to 10000.
Therefore, the dataset generation is transferred to a 4-sum problem.
We don't care how A-D is assigned to IN 1-4 in real network applications because the difference doesn't affect the functionality and performance of RHF construction algorithm. Therefore, n_a >= n_b >= n_c >= n_d can be assumed to eliminate dataset duplication.

By running:
`cd pn_data`
`python3 Config_exp0.py`
The Config_exp0.txt contains 108 cases of mapping. The format of each line in Config_exp0.txt represents:
{n_a, n_b, n_c, n_d}, respectively.
Since the input addresses are randomly generated, the mappings from any one group of addresses (with size n_a/n_b/n_c/n_d) to any one of the interface number (A/B/C/D) are symmetric.

-Experiment:
For an address set, the algorithm finds the parameters that can reach minimum space consumption. The worse-case space consumption of 1000 address sets is reported for one experiment.
For 108 experiments, the paper analyze the distribution of the minimum space consumption according to the size of n_a, n_b, n_c and n_d.

Firstly, change the name of "Config_exp0.txt" to "Config.txt".
By running:
```
cd router_mega
./Generate 1 ../Input_data/Data_10000/
```

Result format:
trial   n_a     n_b     n_c     n_d     maxStorageCost  avgStorageCost  maxTime     avgTime
0       2500    2500    2500    2500    44032           38963           0.005427    0.00391031

## Experiment 1: Interface number dataset of an emulated polar constellation
This data set shows the distribution of interface number (pn) in LEO backbone network with 10000 nodes.
The pn is then summarized as four numbers: n_a, n_b, n_c and n_d, representing the 4 forwarding directions.
n_a + n_b + n_c + n_d = 10000

Several assumptions are made:
1, 10000 nodes in total. Among the 10000 nodes, x nodes have 1-to-1 connection with x ground stations. 
The rest (10000 - 2x) nodes are satellite nodes without ground connection.

2, We assume each ISL in the constellation are always open. 
The topology of the (10000 - x) satellite nodes are assumed as 2-D torus at the current stage.

3, The polar constellation has r orbits. Each orbit has c satellites. 
Parameters satisfy the following fomular:
10000 - x = r * c

4, We name satellite as "s". The route resolving only refers to the destination address.
For routing lookup (RL) task, a satellite node only need to know the interface number (IN) for a given destination address.
There are (10000 - x) satellites in total. We start from s(0) to s(10000 - x - 1).
We treat the satellite s(temp) as the source of packet. For s(temp), there are 10000 destination satellites (including s(temp) itself).
The destination address iterate from s(0) to s(9999), which gives 10000 source-destination (s-d) pairs.

5, We assume each s-d pair only has one route. This means the packet in the source satellite only has one output interface number (pn).
We choose the shortest path to determine the pn for each source satellite in the function: get_pn().
The distance between any two neighboring satellites is assumed to be the same. 
Since the topology is 2-d torus, the direction of going up vs bottom, left vs right is determined by the number of hops from source to destination.
Two scenarios are implemented for balanced and unbalanced distributions.

get_pn1() (Since the generated pn data is unbalanced, it is not used in the experiment):
We assume the ISL between intra-orbit satellites is stable, which may infer broadband communication.
Therefore, the next hop pn of source satellite always prefer the top-bottom directions against left-right directions.
Besides, when multiple paths have the same distance, we assign priority as follows:
1st (highest priority): pn = 1 , to the top
2nd: pn = 3 , to the bottom
3rd: pn = 2 , to the right
4th: pn = 0 (including the case when source and destination nodes are the same satellite) , to the left

get_pn2() (The generated pn data is used in the experiment):
The next hop of source satellite always prefer the direction with longer distance.
This tends to generate diagonal route for most pairs of s-d, and may bring more balanced loads for the constellation network.
For example, 
source:(0,0), dest:(4,6),the next hop would be top
source:(8,8), dest:(2,4),the next hop would be left
When multiple paths have the same distance, we assign priority as follows:
1st (highest priority): pn = 1 , to the top
2nd: pn = 3 , to the bottom
3rd: pn = 2 , to the right
4th: pn = 0 (including the case when source and destination nodes are the same satellite) , to the left

Same as experiment 0,
Since the input addresses are randomly generated, the mappings from any one group of addresses (with size n_a/n_b/n_c/n_d) to any one of the interface number (A/B/C/D) are symmetric.

The command for experiment 1 is the same as experiment 0.
By running:
`cd pn_data`
`python3 Config_exp1.py`

The c and x iterate from ranges that may apply to real constellation networks.
The max and min address count, max and min variance of a group of address count mapping to (A,B,C,D) is printed.

In order to provide the data set for later experiment such as experiment 0, please use fixed c and x, and uncomment line 189, 220, 234.
The generated Config_exp1.txt contains (10000-x) cases of mapping. The format of each line in Config1.txt represents:
{n_a, n_b, n_c, n_d}, respectively. Please change the name of "Config_exp1.txt" to "Config.txt", accordingly.

## Experiment 2: Simulate different sizes of output interface ports
-Dataset generation:
Do experiments for 10000-4, 10000-5, ... , 10000-10 lookup tasks. In each experiment, the number of output interface ports is equally divided and mapped with input addresses. For example, in 10000-4 experiment, n_a = n_b = n_c = n_d = 10000/4; in 10000-5 experiment, n_a = n_b = n_c = n_d = n_e = 10000/5
The worse-case space consumption of 1000 address sets is reported for each experiment.

By running:
```
cd router_mega
./Generate 2 ../Input_data/Data_10000/
```
output_count    maxStorageCost  avgStorageCost  maxTime     avgTime
4               44032           38963           0.00691     0.00387679

## Step2: run RHF_10000 algorithm for experiment 0/2


## Plot the result
For experiment 1, run:
`cd plot`
`python3 plot_experiment1.py`
`python3 sort_variance_experiment1.py`

For experiment 2, run:
`cd plot`
`python3 plot_experiment2.py`

# Citation
The current work is for IEEE TRANSACTIONS ON AEROSPACE AND ELECTRONIC SYSTEMS.
Any questions, please contact keran_zhang@outlook.com
