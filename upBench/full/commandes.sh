#!/bin/bash

apps="fac.o sum.o BFS_app.o BS_app.o GEMV_app.o HST-L_app.o HST-S_app.o MPL_app.o NW_app.o RED_app.o SCAN-RSS_app.o"

gcc generate_trace.c -o generate_trace -lm && echo -e '\e[1;32m[ OK ] for generate_trace\e[m' \ &&

dpu-upmem-dpurte-clang -o factoriel_dpu factoriel.c && echo -e '\e[1;32m[ OK ] for factoriel_dpu\e[m' \ &&

dpu-upmem-dpurte-clang -o sum_elt_in_vector_dpu sum_elt_in_vector.c && echo -e '\e[1;32m[ OK ] for sum_elt_in_vector\e[m' \ &&

cc -c fac.c -Wall -Wextra -g -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` && echo -e '\e[1;32m[ OK ] for fac\e[m' \ &&

cc -c sum.c -Wall -Wextra -g -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` && echo -e '\e[1;32m[ OK ] for sum\e[m' \ &&

make -f BFS_Makefile && echo -e '\e[1;32m[ OK ] for BFS\e[m' \ &&

make -f BS_Makefile && echo -e '\e[1;32m[ OK ] for BS\e[m' \ &&

make -f GEMV_Makefile && echo -e '\e[1;32m[ OK ] for GEMV\e[m' \ &&

make -f HST-L_Makefile && echo -e '\e[1;32m[ OK ] for HST-L\e[m' \ &&

make -f HST-S_Makefile && echo -e '\e[1;32m[ OK ] for HST-S\e[m' \ &&

make -f MPL_Makefile && echo -e '\e[1;32m[ OK ] for MPL\e[m' \ &&

make -f NW_Makefile && echo -e '\e[1;32m[ OK ] for NW\e[m' \ &&

make -f RED_Makefile && echo -e '\e[1;32m[ OK ] for RED\e[m' \ &&

make -f SCAN-RSS_Makefile && echo -e '\e[1;32m[ OK ] for SCAN-RSS\e[m' \ &&

gcc -O0 -g --std=c99 -o full_scheduler full_scheduler.c $apps -lm -I/usr/include/dpu -ldpu && echo -e '\e[1;32m[ OK ] for full_scheduler\e[m' \ &&

rm *.o && echo "" \ &&

echo "USAGE: " \ && 
echo "		./generate_trace -f lis_of_applications" \ &&
echo "" \ &&
echo "		./full_scheduler -f traces"
