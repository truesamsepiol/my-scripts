#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <assert.h>
#include <getopt.h>
#include <pthread.h>

#include <dpu.h>
#include <dpu_log.h>

#define MAX_TRACES 10000
#define NR_PARAMETERS 7
#define SIZE_OF_COMMAND_PARAMETERS 30
#define NR_DPUS_MAX 480

struct dpu_set_t set, dpu;

struct parameter{
	char *path;
};

struct p{
	char *program[NR_PARAMETERS];
};

void bfs(int nr_dpus);
void bs(int nr_dpus);
void gemv(int nr_dpus);
void hst_l(int nr_dpus);
void hst_s(int nr_dpus);
void nw(int nr_dpus);
void red(int nr_dpus);
void scan_rss(int nr_dpus);
void scan_ssa(int nr_dpus);
void sel(int nr_dpus);
void spmv(int nr_dpus);
