
# Step1: Compile the cmph library and copy the libcmph folder into the main directory

```export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/libcmph/lib
make
```
# Step2: Run program
make sure to add "/" at the end of FOLDER_PATH
For example, FOLDER_PATH="../Input_data"

To use BDZ_PH as phf algorithm:

`./benchmphf FOLDER_PATH/ CMPH_BDZ_PH`

To use CHD_PH as phf algorithm:

`./benchmphf FOLDER_PATH/ CMPH_CHD_PH`

Above commands will test the input files in FOLDER_PATH, and provide the time and space result.

