import matplotlib.pyplot as plt
import numpy as np
import xlrd
import math
import sys
import csv
import matplotlib.ticker as mtick

def main():

    # f_input = xlrd.open_workbook(r"./comparison.xlsx")
    # time_sheet = f_input.sheet_by_name('Space')
    num_dataset = 108

    fo = open('Sorted_result_experiment1.txt', 'w')

    with open('Result_experiment1.txt', 'r') as f:
        list1 = f.readlines()
        fo.write(list1[0])
        fo.write(list1[1])
        line_n = 0
        variance_list = np.zeros((num_dataset), dtype=np.float32)
        for i in range(2, len(list1)):
            temp_list = list1[i].split()
            temp_var = np.var([int(temp_list[0]),int(temp_list[1]),int(temp_list[2]),int(temp_list[3])])
            variance_list[i-2] = temp_var
            line_n += 1
        print(line_n)
        ind_list = np.argsort(variance_list)
    for ind in ind_list:
        fo.write(list1[ind+2])
    fo.close()

    # plt.show()

if __name__ == '__main__':
    main()