#!/bin/bash

apps="BFS_app.o BS_app.o GEMV_app.o timer.o HST-L_app.o HST-S_app.o NW_app.o RED_app.o SCAN-RSS_app.o SCAN-SSA_app.o SEL_app.o SpMV_app.o TRNS_app.o TS_app.o UNI_app.o"

gcc generate_trace.c -o generate_trace -lm && echo -e '\e[1;32m[ OK ] for generate_trace\e[m' \ &&

dpu-upmem-dpurte-clang -o factoriel_dpu factoriel.c && echo -e '\e[1;32m[ OK ] for factoriel\e[m' \ &&

dpu-upmem-dpurte-clang -o sum_elt_in_vector_dpu sum_elt_in_vector.c && echo -e '\e[1;32m[ OK ] for sum_elt_in_vector\e[m' \ &&

make -f BFS_Makefile && echo -e '\e[1;32m[ OK ] for BFS\e[m' \ &&

make -f BS_Makefile &&  echo -e '\e[1;32m[ OK ] for BS\e[m' \ &&

make -f GEMV_Makefile &&  echo -e '\e[1;32m[ OK ] for GEMV\e[m' \ &&

make -f HST-L_Makefile &&  echo -e '\e[1;32m[ OK ] for HST-L\e[m' \ &&

make -f HST-S_Makefile &&  echo -e '\e[1;32m[ OK ] for HST-S\e[m' \ &&

make -f NW_Makefile &&  echo -e '\e[1;32m[ OK ] for NW\e[m' \ &&

make -f RED_Makefile &&  echo -e '\e[1;32m[ OK ] for RED\e[m' \ &&

make -f SCAN-RSS_Makefile &&  echo -e '\e[1;32m[ OK ] for SCAN-RSS\e[m' \ &&

make -f SCAN-SSA_Makefile &&  echo -e '\e[1;32m[ OK ] for SCAN-SSA\e[m' \ &&

make -f SEL_Makefile &&  echo -e '\e[1;32m[ OK ] for SEL\e[m' \ &&

make -f SpMV_Makefile &&  echo -e '\e[1;32m[ OK ] for SpMV\e[m' \ &&

make -f TRNS_Makefile &&  echo -e '\e[1;32m[ OK ] for TRNS\e[m' \ &&

make -f TS_Makefile &&  echo -e '\e[1;32m[ OK ] for TS\e[m' \ &&

make -f UNI_Makefile &&  echo -e '\e[1;32m[ OK ] for UNI\e[m' \ &&

gcc -c timer.c &&  echo -e '\e[1;32m[ OK ] for timer\e[m' \ &&

gcc -O0 -g --std=c99 -o single_scheduler single_scheduler.c $apps -lm -I/usr/include/dpu -ldpu && echo -e '\e[1;32m[ OK ] for single_scheduler\e[m' \ &&

./single_scheduler -f traces
