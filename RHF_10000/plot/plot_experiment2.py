import matplotlib.pyplot as plt
import numpy as np
import xlrd
import math
import sys
import csv
import matplotlib.ticker as mtick

def main():
    plt.rcParams['font.family'] = 'Times New Roman'

    # f_input = xlrd.open_workbook(r"./comparison.xlsx")
    # time_sheet = f_input.sheet_by_name('Space')
    num_dataset = 17

    max_storage = np.zeros((2, num_dataset), dtype=np.float32)
    avg_storage = np.zeros((2, num_dataset), dtype=np.float32)
    max_time = np.zeros((2, num_dataset), dtype=np.float32)
    avg_time = np.zeros((2, num_dataset), dtype=np.float32)

    with open('Result_experiment2.txt', 'r') as f:
        list1 = f.readlines()
        line_n = 0
        for i in range(2, len(list1)):
            temp_list = list1[i].split()

            max_storage[0][line_n] = float(temp_list[1])/1000
            avg_storage[0][line_n] = float(temp_list[2])/1000
            max_time[0][line_n] = float(temp_list[3])*1000
            avg_time[0][line_n] = float(temp_list[4])*1000

            line_n += 1

    xlabel = [l for l in range(4,num_dataset+4,1)]
    positions = np.arange(num_dataset)

    fig, ax1 = plt.subplots()

    ax1.bar(xlabel, max_storage[0][:], width=0.8, align='center', color='y', label=u"Max storage")
    ax1.bar(xlabel, avg_storage[0][:], width=0.8, align='center', color='c', label=u"Avg storage")
    ax1.set_xticks(xlabel)
    ax1.set_xticklabels(xlabel)
    ax1.set_xlabel('Num of output interface',fontsize=24)
    ax1.set_ylabel('Function Size (Kbit)',fontsize=24)
    ax1.set_yticklabels(['0','20','40','60','80','100'], fontsize=20)
    ax1.tick_params(labelsize=20)
    
    # max storage label
    max_max_score = max(max_storage[0][:])
    ax1.set_ylim(0, int(max_max_score * 1.2))
    for x,y in zip(xlabel, max_storage[0][:]):
        ax1.text(x, y + max_max_score * 0.04, y, ha='center', va='center', fontsize=11)

    # avg storage label
    max_avg_score = max(avg_storage[0][:])
    # ax1.set_ylim(0, int(max_avg_score * 1.2))
    for x,y in zip(xlabel, avg_storage[0][:]):
        ax1.text(x, y - max_avg_score * 0.08, y, ha='center', va='center', fontsize=11)

    # line chart of time
    ax2 = ax1.twinx()
    ax2.plot(xlabel,max_time[0][:],'ro-',ms=4,label='Max time')
    ax2.plot(xlabel,avg_time[0][:],'ko-',ms=4,label='Avg time')
    ax2.tick_params(labelsize=20)
    ax2.set_ylabel('Construction Time (ms)',fontsize=24)

    # legend
    handles1, labels1 = ax1.get_legend_handles_labels()
    handles2, labels2 = ax2.get_legend_handles_labels()
    plt.legend(handles1+handles2, labels1+labels2, loc='upper right', fontsize=15)
    plt.show()

if __name__ == '__main__':
    main()