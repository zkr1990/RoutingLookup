import sys
import math
import os
import random
import numpy as np

# You need to generate different input IP files
# In this file, the input is set to IPV6 address as a string
# eg: "x.x.x.x.x.x.x.x", each "x" is a number from str(0) to str(15)
# "." does not appear in the string
# Total number of possible IP addesses is: d1 = 16*16*16*16*16*16*16*16
# The function rand_ip(8) casts this IPv6 string with 8 chars into an integer
# maximum possible integer of rand_ip(8) would be: (2^32 - 1)
max_ip_num = 16
# Set n value first
n = 10000 # each set has n input keys
z = 8 ## the name of the file would be 0000_1_54_4.txt

# set the total number of key set
max_key_set = 1000
# set the total number of test files
max_itr = 1

# generate a random IP address with length = l
# the random number for each 4bits follows uniform distribution
# In hardware, the input IPs do not need to be cast between data types, 
# since they are always binary data
def rand_ip(l):
    tmp_ip = int(random.uniform(0,max_ip_num))
    for s in range(l-1):
        tmp_ip = tmp_ip*16 + int(random.uniform(0,max_ip_num))
    return tmp_ip
# # numpy random
# def np_rand_ip(l):
#     tmp_ip = numpy.random.uniform(0,high=max_ip_num,dtype='uint32')
#     for s in range(l-1):
#         tmp_ip = tmp_ip*16 + numpy.random.uniform(0,high=max_ip_num,dtype='uint32')
#     return tmp_ip

if __name__=='__main__':
    print('begin')
    ################################
    ## curent folder
    for folder_name in range(max_key_set):
        cur_dir = "./Data_%d/%d"%(n,folder_name)
        if not os.path.exists(cur_dir):
            os.makedirs(cur_dir)
        ip_list = []
        i = 0
        while(i < n):
        ##for i in range(n):
            ## generate 1 random IP
            # 4 xxx IP strings
            tmp_ip = str(rand_ip(z))
            if (tmp_ip not in ip_list):
                # tmp_ip = np_rand_ip(4) # numpy version
                ip_list.append(tmp_ip)
                i += 1
        print('size of ip set is: ', len(ip_list))
        print('writing to txt file for folder %d...'%(folder_name))
        # divide the ip list into z group
        # shuffle -> divide -> output
        for itr in range(max_itr):
            random.shuffle(ip_list)
            fin = open("%s/key_0000_1_%d_%d__%s.txt"%(cur_dir,n,z,str(itr).zfill(6)),'w')
            s = '\n'
            fin.write(s.join(ip_list))
            fin.close()
    print('finish all folders.')


