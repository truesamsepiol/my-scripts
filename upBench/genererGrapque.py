#!/usr/bin/python3.6

from matplotlib.backends.backend_pdf import PdfPages

import matplotlib.pyplot as plt
import numpy as np

import os
import sys

#plt.style.use('dark_background') # black background

max_dpus = 0
max_time = 0

labelFull   = "full scheduler"
labelSingle = "single scheduler"

uniteX = "Time in seconde"
uniteY = "# DPUs"


x1 = list()
y1 = list()
x2 = list()
y2 = list()

def usage():
    if len(sys.argv) != 3 or not os.path.isfile(sys.argv[1]) or not os.path.isfile(sys.argv[2]):
        print("Command for launch the script: |\n")
        print("                               |-> python3", sys.argv[0], "out_of_full out_of_single")
        exit(1)

def bluid_coordonate(file_name, xAxis, yAxis):
    global max_dpus
    global max_time
    f = open(file_name, "r")
    while True:
        time = f.readline().replace("\n", "")
        if not time:
            break
        xAxis.append(int(time))
        yAxis.append(int(f.readline().replace("\n", "")))
        max_dpus        = int(f.readline().replace("\n", ""))
        max_time        = max(max_time, int(time))

def build_graphe():
    xSingle = [i for i in range(max_time + 1)]
    ySingle = [0 for i in range(max_time + 1)]

    xFull   = [i for i in range(max_time + 1)]
    yFull   = [0 for i in range(max_time + 1)]

    for i in range(len(y1)):
        yFull[x1[i]] = y1[i]

    for i in range(len(y2)):
        ySingle[x2[i]] = y2[i]
    
    plt.grid(True)
    plt.plot(xFull, yFull, linewidth=1, marker=".", label=labelFull)
    plt.plot(xSingle, ySingle, linewidth=1, marker=".", label=labelSingle)
    plt.xlabel(uniteX, labelpad=4.0, fontsize='small', fontweight='bold')
    plt.ylabel(uniteY, labelpad=4.0, fontsize='small', fontweight='bold')
    plt.legend()
   
    plt.xticks(np.arange(0, max_time + 1, 75))
    plt.yticks(np.arange(0, max_dpus + 1, 2))

    file_name  = "evalutaion_of_single_vs_full.pdf"
    plt.savefig(file_name, dpi='figure', format="pdf", bbox_inches="tight", pad_inches=0.1)

if __name__ == "__main__":
    usage()
    bluid_coordonate(sys.argv[1], x1, y1)
    bluid_coordonate(sys.argv[2], x2, y2)
    
    build_graphe()
