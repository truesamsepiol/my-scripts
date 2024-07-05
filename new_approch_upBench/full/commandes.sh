apps="host-simple-bench.o"

cc -c host-simple-bench.c -Wall -Wextra -g -IBFS_support -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` &&  echo -e '\e[1;32m[ OK ] for host-simple-bench\e[m' \ &&

dpu-upmem-dpurte-clang simple-bench.c -o simple-bench &&  echo -e '\e[1;32m[ OK ] for simple-bench\e[m' \ &&

gcc -O0 -g --std=c99 -o full_scheduler full_scheduler.c $apps -lm -I/usr/include/dpu -ldpu -lpthread && echo -e '\e[1;32m[ OK ] for full_scheduler\e[m' \ &&

rm *.o && echo "" \ &&
echo "USAGE: " \ && 
echo "		./full_scheduler -f traces > out"
