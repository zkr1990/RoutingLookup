import matplotlib.pyplot as plt
import numpy as np
import xlrd
import math
import sys
import csv

f_input = xlrd.open_workbook(r"./comparison.xlsx")
time_sheet = f_input.sheet_by_name('Time')
# input_row = time_sheet.row_values(4)

xlabel = ['1k','2k','3k','4k','5k','6k','7k','8k','9k','10k']

SPHF_matrix = np.zeros((2, 10), dtype=np.float32)
BDZ_matrix = np.zeros((2, 10), dtype=np.float32)
CHD_matrix = np.zeros((2, 10), dtype=np.float32)
BBHash_matrix = np.zeros((2, 10), dtype=np.float32)

SPHF_matrix[0][:] = time_sheet.row_values(3)[2:12]
BDZ_matrix[0][:] = time_sheet.row_values(9)[2:12]
CHD_matrix[0][:] = time_sheet.row_values(15)[2:12]
BBHash_matrix[0][:] = time_sheet.row_values(21)[2:12]

plt.plot(xlabel,BDZ_matrix[0][:],'go-',ms=8,label='BDZ_PH')
plt.plot(xlabel,CHD_matrix[0][:],'yo-',ms=8,label='CHD_PH')
plt.plot(xlabel,BBHash_matrix[0][:],'ro-',ms=8,label='BBHash')
plt.plot(xlabel,SPHF_matrix[0][:],'ko-',ms=8,label='SPHF')

plt.xlabel('Number of Input IP Addresses',fontsize=18)
plt.ylabel('Average Construction Time (ms)',fontsize=18)
plt.tick_params(labelsize=15)
plt.legend(fontsize=15)

plt.show()