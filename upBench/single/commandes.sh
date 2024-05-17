#!/bin/bash

apps="BFS_app.o BS_app.o GEMV_app.o timer.o"

gcc generate_trace.c -o generate_trace -lm && echo -e '\e[1;32m[ OK ] for generate_trace\e[m'

dpu-upmem-dpurte-clang -o factoriel_dpu factoriel.c && echo -e '\e[1;32m[ OK ] for factoriel\e[m'

dpu-upmem-dpurte-clang -o sum_elt_in_vector_dpu sum_elt_in_vector.c && echo -e '\e[1;32m[ OK ] for sum_elt_in_vector\e[m'

make -f BFS_Makefile && echo -e '\e[1;32m[ OK ] for BFS\e[m'

make -f BS_Makefile &&  echo -e '\e[1;32m[ OK ] for BS\e[m'

make -f GEMV_Makefile &&  echo -e '\e[1;32m[ OK ] for GEMV\e[m'

gcc -c timer.c &&  echo -e '\e[1;32m[ OK ] for timer\e[m'

gcc -O0 -g --std=c99 -o single_scheduler single_scheduler.c $apps -I/usr/include/dpu -ldpu && echo -e '\e[1;32m[ OK ] for single_scheduler\e[m'

./single_scheduler -f traces