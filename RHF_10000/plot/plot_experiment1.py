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

    max_storage = np.zeros((2, num_dataset), dtype=np.float32)
    avg_storage = np.zeros((2, num_dataset), dtype=np.float32)
    max_time = np.zeros((2, num_dataset), dtype=np.float32)
    avg_time = np.zeros((2, num_dataset), dtype=np.float32)

    # with open('Result_experiment1.txt', 'r') as f:
    with open('Sorted_result_experiment1.txt', 'r') as f:
        list1 = f.readlines()
        line_n = 0
        for i in range(2, len(list1)):
            temp_list = list1[i].split()

            max_storage[0][line_n] = float(temp_list[5])/1000
            avg_storage[0][line_n] = float(temp_list[6])/1000
            max_time[0][line_n] = float(temp_list[7])*1000
            avg_time[0][line_n] = float(temp_list[8])*1000

            line_n += 1


    xlabel = [l for l in range(num_dataset)]
    positions = np.arange(num_dataset)

    fig, ax1 = plt.subplots()

    # histogram of storage
    ax1.bar(xlabel, max_storage[0][:], width=0.8, align='center', color='y', label=u"Max storage")
    ax1.bar(xlabel, avg_storage[0][:], width=0.8, align='center', color='c', label=u"Avg storage")
    ax1.set_xticks(positions)
    ax1.set_xticklabels(xlabel,rotation=90,fontsize=9)
    ax1.set_xlabel('Dataset sorted by variance',fontsize=24)
    ax1.set_ylabel('Function Size (Kbit)',fontsize=24)
    ax1.set_yticklabels(['0','10','20','30','40','50','60','70','80'], fontsize=18)

    # line chart of time
    ax2 = ax1.twinx()
    ax2.plot(xlabel,max_time[0][:],'ro-',ms=2,label='Max time')
    ax2.plot(xlabel,avg_time[0][:],'ko-',ms=2,label='Avg time')
    ax2.tick_params(labelsize=18)
    ax2.set_ylabel('Construction Time (ms)',fontsize=24)

    # legend
    handles1, labels1 = ax1.get_legend_handles_labels()
    handles2, labels2 = ax2.get_legend_handles_labels()
    plt.legend(handles1+handles2, labels1+labels2, loc='upper right', fontsize=18)
    plt.show()

    # plt.show()

if __name__ == '__main__':
    main()