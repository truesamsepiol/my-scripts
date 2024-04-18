# authors 
# . Eric Okala 
# . Nguetchouang Ngongang Kevin

import matplotlib
import numpy as np
import os 
import sys

from matplotlib import pyplot as plt 
from matplotlib.backends.backend_pdf import PdfPages

black      = "#000"
colors     = ["#7F00FF", "#00008B"] #violet and dark blue

width      = 0.44
space_bar  = [0.22, -0.22]

CPU_DPU    = "CPU-DPU"
DPU_CPU    = "DPU-CPU"
exec_time  = "Execution Time"

metric     = "Time in ms"

file_name  = "result-of-evalutaions.pdf"

labels = ["PrIM", "vPrIM"]
hatchs = ["/", "\\"]



def usage():
    if len(sys.argv) != 2 or not os.path.isfile(sys.argv[1]):
        print("Command for launch the script: |\n")
        print("                               |-> python3 ", sys.argv[0], "file_has_metric")
        exit(1)

def create_figures(arg_file):
    plt.style.use('dark_background') # black background
   
    f = open(arg_file, "r")
    metrics = [[], []]
    bench_name = ""
    while True:
        line = f.readline()
        if not line:
            break
        bench_name = line.replace("\n", "")
        for i in range(2):
            line = f.readline().replace("\n", "")
            metrics[i] = [float(el) for el in line.split(",")]

        fig = plt.figure() #fig = plt.figure(figsize=plt.figaspect(1)) # hauteur/largeur=0.33
        ax = fig.add_axes([0,0,1,1])
        X=np.arange(3) # number of bars
        for i in range(2):
            ax.bar(X-space_bar[i], metrics[i], width=width, label=labels[i], color=colors[i], edgecolor=black, hatch=hatchs[i])
        ax.set_xticklabels([CPU_DPU, DPU_CPU, exec_time])
        ax.set_xticks(X)
        plt.xlabel(bench_name)
        plt.ylabel(metric)
        plt.legend()
    f.close()

def save_images(file_name):
    p = PdfPages(file_name)
    fig_nums = plt.get_fignums()
    figs = [plt.figure(n) for n in fig_nums]

    for fig in figs:
        fig.savefig(p, format="pdf", bbox_inches="tight")
    p.close()

if __name__ == "__main__":
    usage()
    create_figures(sys.argv[1])
    save_images(file_name)
