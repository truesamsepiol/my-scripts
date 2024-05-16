#!/bin/bash

gcc generate_trace.c -o generate_trace -lm && echo '[ OK ] for generate_trace'

dpu-upmem-dpurte-clang -o factoriel_dpu factoriel.c && echo '[ OK ] for factoriel'

dpu-upmem-dpurte-clang -o sum_elt_in_vector_dpu sum_elt_in_vector.c && echo '[ OK ] for sum_elt_in_vector'

make -f BFS_Makefile && echo '[ OK ] for BFS'

gcc -O0 -g --std=c99 -o full_scheduler full_scheduler.c BFS_app.o -I/usr/include/dpu -ldpu && echo '[ OK ] for full_scheduler'
