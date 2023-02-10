# RHF_64
Routing Hash Function(RHF) is used to hash from 64-bit input IPs to output interface numbers.
The algorithm is used as the reference by HLS.
The construction result is not used by the original paper. 

# Implementation
## Compile program
```
cd router_mega
cmake ..
make
```

## Commands for single input file in the router_mega folder
Specify the number of router(n = 1) first. 
Only CTimer1 is working, which provides the RHF1 strategy for 64-bit input.
CTimer2 and CTimer3 are not working for now.

To report construction time:

`./Generate n FolderPath`

For example, FOLDER_PATH="../Input_data/Data_10000"
To generate function: Specify an input file (input.txt) first, function.txt will be generated with parameters of RHF.

`./Generate n input.txt function.txt`

Test generated function:

`./Generate n input.txt function.txt -t`

The program use input.txt as test case, and check whether the generated RHF can map correctly.

## Commands for multiple input files in the RHF folder
Generate all function files in routerFuncData/ folder. By default, n is set to 1.

`sh generate.sh`

Test all function files for files in Input_data/, n is set to 1.

`sh test.sh`

# RTLP Results for RHF1 (10000 64 bit input IPs)
RHF algorithms are calculated as foolow:

1, For construction time, 1000 experiments are conducted. The max time and average time are calculated.

2, For storage cost, the source code of algorithms is analyzed and storage is estimated considering data structures used in construction steps.

3, For function space, The storage is estimated with consideration of D[] and I[].

Method       |AVG time(ms)	|MAX time(ms)	|Storage(Kb)	|Function Space(Kb)
-------------|------------------|:-------------:|:-------------:|:---------:


The 64-bit RHF is designed only for hardware implementation on FPGA at the current stage.
The construction time of RHF1-64 is 3.36 ms.
More comparisons remains as a research direction in the future.

# Citation
The current work is for IEEE TRANSACTIONS ON AEROSPACE AND ELECTRONIC SYSTEMS.
Any questions, please contact keran_zhang@outlook.com
