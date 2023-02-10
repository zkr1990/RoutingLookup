# PHF benchmark
This is the benchmark code for (minimum) perfect hash function (PHF/MPHF) algorithms. BBHash, CHD, BDZ and SPHF are compared in terms of constructing time and theoretical storage cost.

All algorithms need compile first in each folder. Please refer to different README.md in each subfolder.
## bbhash
BBHash algorithm is MPHF generator mainly designed for massive key sets.

## cmph
cmph is a PHF/MPHF library with multiple different algorithms. We use the the CHD and BDZ in the experiment.

## Satellite Perfect Hash Function (SPHF)
SPHF is PHF designed for satellite applications. The construction of hash function is based on searching suitable parameters in the helper array.
For more details, please refer to the paper.

## SPHF-RL
SPHF-RL is PHF optimized according to hardware features, mainly for input key sets with size smaller than/equal to 10000.

# Dependancy
Ubuntu 18.04, g++, python3, cmake

# Input data generation
Input_data folder contains inputs used for generating PHF/MPHF functions and test the correctness of those functions.

`python3 generate_test_data.py`

The python3 program generates 1000 input files (mark as 1000 different folders). Example includes 0,1,2 folders.
By default, the size of input key set is 10000, please change n in line 16 for other input size.

# PHF Result
Construction phase:

1, For construction time, 1000 experiments are conducted. The max time and average time are calculated.

2, For storage cost, source code of algorithms is analyzed and storage is estimated with the consideration of data structures used in construction steps.
Optimizations for existing algorithms (BBHash, CHD, BDZ) on hardware pletform remains as a future research direction.

3, For function size, assume input are 10000 32-bit keys:

BBHash: Estimation based on 2 levels of hash. For input, we allocate a buffer with size 32*10000=320000 bit. The conflict mark array for each level needs 10000 bit. Total is 340000 bit.

CHD_PH, BDZ_PH: The storage is directly calculated by checking the size of function_file.

SPHF: The storage is estimated with consideration of D[].

# RL Result (10000 32-bit input IPs)
The analysis is similar to PHF results.

RL for these algorithms needs an output array with size m*2 bit (I[]).

For construction time, an Epsilon time is added.

For storage cost, m*2bit is added.

For function space, m*2bit is added.

Method       |AVG time(ms)	|MAX time(ms)	|Storage(Kb)	|Function Space(Kb)
-------------|------------------|:-------------:|:-------------:|:---------:
BDZ-PH-RL   |3.73+e	        |6.85+e		|3457		|39.6
CHD-PH-RL   |2.69+e	        |4.79+e		|957		|30
BBHash-RL   |7.64+e	        |28.62+e	|352		|39.2
SPHF-RL     |3.65+e	        |5.96+e		|538		|50

# Citation
The current work is for IEEE TRANSACTIONS ON AEROSPACE AND ELECTRONIC SYSTEMS.
Any questions, please contact keran_zhang@outlook.com
