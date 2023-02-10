import matplotlib.pyplot as plt
import numpy as np
# import xlrd
import xlrd2
import math
import sys
import csv




xlabel = ['1k','2k','3k','4k','5k','6k','7k','8k','9k','10k']


def lc1():
    a1 = plt.subplot(131)
    time_sheet = f_input.sheet_by_name('Time')
    plt.title('Average Construction Time',fontsize=18)

    SPHF_matrix = np.zeros((2, 10), dtype=np.float32)
    BDZ_matrix = np.zeros((2, 10), dtype=np.float32)
    CHD_matrix = np.zeros((2, 10), dtype=np.float32)
    BBHash_matrix = np.zeros((2, 10), dtype=np.float32)

    SPHF_matrix[0][:] = time_sheet.row_values(3)[2:12]
    BDZ_matrix[0][:] = time_sheet.row_values(9)[2:12]
    CHD_matrix[0][:] = time_sheet.row_values(15)[2:12]
    BBHash_matrix[0][:] = time_sheet.row_values(21)[2:12]

    a1.plot(xlabel,BDZ_matrix[0][:],'go-',ms=8,label='BDZ_PH')
    a1.plot(xlabel,CHD_matrix[0][:],'yo-',ms=8,label='CHD_PH')
    a1.plot(xlabel,BBHash_matrix[0][:],'ro-',ms=8,label='BBHash')
    a1.plot(xlabel,SPHF_matrix[0][:],'ko-',ms=8,label='SPHF')
    a1.set_xlabel('Num of Keys',fontsize=18)
    a1.set_ylabel('ms',fontsize=18)

    a1.tick_params(labelsize=15)
    a1.legend(fontsize=15)


def lc2():
    a1 = plt.subplot(132)
    time_sheet = f_input.sheet_by_name('Time')
    plt.title('Max Construction Time',fontsize=18)

    SPHF_matrix = np.zeros((2, 10), dtype=np.float32)
    BDZ_matrix = np.zeros((2, 10), dtype=np.float32)
    CHD_matrix = np.zeros((2, 10), dtype=np.float32)
    BBHash_matrix = np.zeros((2, 10), dtype=np.float32)

    SPHF_matrix[0][:] = time_sheet.row_values(5)[2:12]
    BDZ_matrix[0][:] = time_sheet.row_values(11)[2:12]
    CHD_matrix[0][:] = time_sheet.row_values(17)[2:12]
    BBHash_matrix[0][:] = time_sheet.row_values(23)[2:12]

    a1.plot(xlabel,BDZ_matrix[0][:],'go-',ms=8,label='BDZ_PH')
    a1.plot(xlabel,CHD_matrix[0][:],'yo-',ms=8,label='CHD_PH')
    a1.plot(xlabel,BBHash_matrix[0][:],'ro-',ms=8,label='BBHash')
    a1.plot(xlabel,SPHF_matrix[0][:],'ko-',ms=8,label='SPHF')
    a1.set_xlabel('Num of Keys',fontsize=18)
    a1.set_ylabel('ms',fontsize=18)

    a1.tick_params(labelsize=15)
    a1.legend(fontsize=15)

def lc3():
    a1 = plt.subplot(133)
    time_sheet = f_input.sheet_by_name('Space')
    plt.title('Storage Estimation',fontsize=18)

    SPHF_matrix = np.zeros((2, 10), dtype=np.float32)
    BDZ_matrix = np.zeros((2, 10), dtype=np.float32)
    CHD_matrix = np.zeros((2, 10), dtype=np.float32)
    BBHash_matrix = np.zeros((2, 10), dtype=np.float32)

    SPHF_matrix[0][:] = time_sheet.row_values(6)[2:12]
    BDZ_matrix[0][:] = time_sheet.row_values(10)[2:12]
    CHD_matrix[0][:] = time_sheet.row_values(14)[2:12]
    BBHash_matrix[0][:] = time_sheet.row_values(18)[2:12]

    a1.plot(xlabel,BDZ_matrix[0][:],'go-',ms=8,label='BDZ_PH')
    a1.plot(xlabel,CHD_matrix[0][:],'yo-',ms=8,label='CHD_PH')
    a1.plot(xlabel,BBHash_matrix[0][:],'ro-',ms=8,label='BBHash')
    a1.plot(xlabel,SPHF_matrix[0][:],'ko-',ms=8,label='SPHF')
    a1.set_xlabel('Num of Keys',fontsize=18)
    a1.set_ylabel('Kb',fontsize=18)

    a1.tick_params(labelsize=15)
    a1.legend(fontsize=15)

if __name__ == '__main__':
    f_input = xlrd2.open_workbook(r"./comparison.xlsx")
    # input_row = time_sheet.row_values(4)

    plt.figure(figsize=(24, 4))
    plt.subplots_adjust(wspace=0.3, hspace=0.2)
    plt.rcParams['font.family'] = 'Times New Roman'
    plt.rcParams['axes.unicode_minus'] = False
    lc1()
    lc2()
    lc3()
    plt.subplots_adjust(left=0.05, right=0.97, top=0.9, bottom=0.15)
    plt.savefig('constructionPHF.png')
    plt.show()

    f_input.close()

