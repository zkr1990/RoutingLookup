import sys
import os
import random
import numpy as np

if __name__=='__main__':
    f = open("Config_exp0.txt","w")
    count = 0
    for a in range(2500,10001,500):
        for b in range(a,-1,-500):
            if (a+b) > 10000:
                continue
            for c in range(b,-1,-500):
                if (a+b+c) > 10000:
                    continue
                for d in range(c,-1,-500):
                    if (a+b+c+d) == 10000:
                        count += 1
                        print(str(a)+" "+str(b)+" "+str(c)+" "+str(d))
                        f.write(str(a)+" "+str(b)+" "+str(c)+" "+str(d)+"\n")
                    else:
                        continue
    print("finish")
    print(count)
    f.close()


