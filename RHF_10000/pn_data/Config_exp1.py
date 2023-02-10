import sys
import math
import os
import random
import numpy as np

# The 5 assumptions for experiment1:
# 1, 10000 nodes in total. Among the 10000 nodes, x nodes have 1-to-1 connection with x ground stations. 
# The rest (10000 - 2x) nodes are satellite nodes without ground connection.

# 2, The topology of the (10000 - x) satellite nodes are assumed as 2-D torus at the current stage.

# 3, The polar constellation has r orbits. Each orbit has c satellites. 
# Parameters satisfy the following fomular:
# 10000 - x = r * c

# 4, We name satellite as "s".
# There are (10000 - x) data sets in total. We start from s(0) to s(10000 - x - 1).
# For each data set, we pick the satellite s(temp) as the source of packet.
# The destination of packet iterate from s(0) to s(9999), which gives 10000 source-destination pairs.

# 5, Each pair only has one path. This means the packet in the source satellite only has one output interface number (pn).
# We choose the shortest path to determine the pn for each source satellite. Two scenarios are implemented for balanced and unbalanced distributions.

# The function get_pn1() calculates the pn value for each (source, destination) satellite pair
# The next hop of source satellite always prefer the direction with longer distance
# for example, 
# source:(0,0), dest:(4,6),the next hop would be top
# source:(8,8), dest:(2,4),the next hop would be left
# When multiple paths have the same distance, we assign priority as follows:
# 1st (highest priority): pn = 1 , to the top
# 2nd: pn = 3 , to the bottom
# 3rd: pn = 2 , to the right
# 4th: pn = 0 (including the case when source and destination nodes are the same satellite) , to the left

# the outcome of this file is to provide the distributions of the number of pn=0, pn=1, pn=2, pn=3 as:
# npn0 npn1 npn2 npn3
# for example:
# 3500 2500 2500 1500 means: 
# 3500 data paths go through interface 0, 2500 paths go through interface 1
# 2500 data paths go through interface 2, 1500 paths go through interface 3
# the result format is the same as experiment 0

def get_pn1(sx,sy,dx,dy):
    pn = 0
    # for distance in the inner direction
    x1 = abs(dx-sx)
    y1 = abs(dy-sy)
    # for distance in the outer direction
    x2 = c-abs(dx-sx) 
    y2 = r-abs(dy-sy)
    if(dy == sy):
        # in the same row, only decide to go left(pn=0) or right(pn=2)
        if(sx < dx):
            if(x1 <= x2):
                pn = 2
            else:
                pn = 0
        elif(sx > dx):
            if(x1 < x2):
                pn = 0
            else:
                pn = 2
        else:
            pn = 0 # source and dest are the same satellite
    elif(dx == sx):
        # in the same column, only decide to go top(pn=1) or bottom(pn=3)
        if(sy < dy):
            if(y1 <= y2):
                pn = 1
            else:
                pn = 3
        elif(sy > dy):
            if(y1 < y2):
                pn = 3
            else:
                pn = 1
    elif(min(x1, x2) < min(y1,y2)):
        # only decide to go left(pn=0) or right(pn=2)
        if(sx < dx):
            if(x1 <= x2):
                pn = 2
            else:
                pn = 0
        elif(sx > dx):
            if(x1 < x2):
                pn = 0
            else:
                pn = 2
    else:
        # only decide to go top(pn=1) or bottom(pn=3)
        if(sy < dy):
            if(y1 <= y2):
                pn = 1
            else:
                pn = 3
        elif(sy > dy):
            if(y1 < y2):
                pn = 3
            else:
                pn = 1
        else:
            pn = 1
    return pn

# get_pn2():
# If the intra-orbit satellite has much bigger bandwidth than inter-orbit satellite. We can choose the following function:
# The next hop of source satellite always prefer the top-bottom directions against left-right directions,
# this give us an unbalanced pn distribution.
# When multiple paths have the same distance, we assign priority as follows:
# 1st (highest priority): pn = 1 , to the top
# 2nd: pn = 3 , to the bottom
# 3rd: pn = 2 , to the right
# 4th: pn = 0 (including the case when source and destination nodes are the same satellite) , to the left
# def get_pn2(sx,sy,dx,dy):
#     pn = 0
#     x1 = abs(dx-sx)
#     x2 = c-abs(dx-sx)
#     y1 = abs(dy-sy)
#     y2 = r-abs(dy-sy)
#     # current dest satellite: (dx,dy),
#     # calculate the shortest path from (sx,sy) to (dx,dy)
#     # and determine the output interface number (pn) for the source satellite
#     if(dy == sy):
#         # in the same row, only decide to go left(pn=0) or right(pn=2)
#         if(sx < dx):
#             if(x1 <= x2):
#                 pn = 2
#             else:
#                 pn = 0
#         elif(sx > dx):
#             if(x1 < x2):
#                 pn = 0
#             else:
#                 pn = 2
#         else:
#             pn = 0 # source and dest are the same satellite
#     else:
#         # in different rows, first send to the neighbor in the same orbit
#         # go top or go bottom
#         if(sy < dy):
#             if(y1 <= y2):
#                 pn = 1
#             else:
#                 pn = 3
#         elif(sy > dy):
#             if(y1 < y2):
#                 pn = 3
#             else:
#                 pn = 1
#     return pn

def calculate(sx,sy,c,r, x_list):
    npn0 = 0
    npn1 = 0
    npn2 = 0
    npn3 = 0
    
    for dx in range(0, c, 1):
        for dy in range(0, r, 1):
            pn = get_pn1(sx,sy,dx,dy)
            if(pn == 0):
                npn0 += 1
            if(pn == 1):
                npn1 += 1
            if(pn == 2):
                npn2 += 1
            if(pn == 3):
                npn3 += 1
    # for the randomized x ground stations, determine their pn
    for x in x_list:
        dx_x = int(x/r)
        dy_x = int(x%r)
        pn = get_pn1(sx,sy,dx_x,dy_x)
        if(pn == 0):
            npn0 += 1
        if(pn == 1):
            npn1 += 1
        if(pn == 2):
            npn2 += 1
        if(pn == 3):
            npn3 += 1

    return npn0,npn1,npn2,npn3

n = 10000 # each set has n input addresses

if __name__=='__main__':
    # f = open("Config_exp1.txt","w")

    #for statistics
    #[npn0, npn1, npn2, npn3]
    max_var = 0
    min_var = sys.maxsize
    max_npn = 0
    min_npn = n
    # for the rest 10000 - x satellites, the number of orbits is c.
    for c in range(90,190,10):
        # we assume x increases from 100 to 1000, step = 100, this gives 10 cases.
        for x in range(100, 1001, 100): # x can not be larger than n/2
            # each orbit has r satellites: r = (10000 - x) / 100
            r = int((n - x) / c)
            # generate x random and unique numbers from 0 to (n - x - 1)
            x_list = []
            i = 0
            while(i < x):
                tmp_x = int(random.uniform(0, n-x))
                if (tmp_x not in x_list):
                    x_list.append(tmp_x)
                    i += 1
            
            # for source satellite
            # sx is the column number(orbit number), sy is the row number(satellite number in an orbit)
            for sx in range(0, c, 1):
                for sy in range(0, r, 1):
                    # the position of current source satellite is: (sx, sy)
                    # we calculate the 10000 pn for the current satellite, using dj
                    npn0,npn1,npn2,npn3 = calculate(sx, sy, c, r, x_list)
                    print(str(npn0)+" "+str(npn1)+" "+str(npn2)+" "+str(npn3))
                    # f.write(str(npn0)+" "+str(npn1)+" "+str(npn2)+" "+str(npn3)+"\n")
                    # get global info for statistics
                    var = np.var([npn0,npn1,npn2,npn3])
                    if(var>max_var): max_var = var
                    if(var<min_var): min_var = var
                    max_npn_cur = max(npn0,npn1,npn2,npn3)
                    if(max_npn_cur>max_npn): max_npn = max_npn_cur
                    min_npn_cur = min(npn0,npn1,npn2,npn3)
                    if(min_npn_cur<min_npn): min_npn = min_npn_cur
    print("max_var：",str(max_var))
    print("min_var： ",str(min_var))
    print("max_npn： ",str(max_npn))
    print("min_npn： ",str(min_npn))
    print("finish")
    # f.close()


##
# max_var： 3151005.5
# min_var：  4626.0
# max_npn：  4364
# min_npn：  708

