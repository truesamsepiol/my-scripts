#!/bin/bash

apps="fac.o sum.o BFS_app.o BS_app.o"

gcc generate_trace.c -o generate_trace -lm && echo -e '\e[1;32m[ OK ] for generate_trace\e[m' \ &&

dpu-upmem-dpurte-clang -o factoriel_dpu factoriel.c && echo -e '\e[1;32m[ OK ] for factoriel_dpu\e[m' \ &&

dpu-upmem-dpurte-clang -o sum_elt_in_vector_dpu sum_elt_in_vector.c && echo -e '\e[1;32m[ OK ] for sum_elt_in_vector\e[m' \ &&

cc -c fac.c -Wall -Wextra -g -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` && echo -e '\e[1;32m[ OK ] for fac\e[m' \ &&

cc -c sum.c -Wall -Wextra -g -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` && echo -e '\e[1;32m[ OK ] for sum\e[m' \ &&

make -f BFS_Makefile && echo -e '\e[1;32m[ OK ] for BFS\e[m' \ &&

make -f BS_Makefile && echo -e '\e[1;32m[ OK ] for BS\e[m' \ &&

gcc -O0 -g --std=c99 -o full_scheduler full_scheduler.c $apps -lm -I/usr/include/dpu -ldpu && echo -e '\e[1;32m[ OK ] for full_scheduler\e[m' \ &&

rm *.o && echo "" \ &&

echo "USAGE: " \ && 
echo "		./generate_trace -f lis_of_applications" \ &&
echo "" \ &&
echo "		./full_scheduler -f traces"
