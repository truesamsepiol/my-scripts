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
#define NR_DPUS_MAX 20

#define SUM "./sum_elt_in_vector_dpu"

struct dpu_set_t sub_set_of_dpus[NR_DPUS_MAX];

struct parameter{
	char *path;
};

struct p{
	char *program[NR_PARAMETERS];
};

void sum(int, int);
void fac(int, int);
void bfs(int, int);
void bs(int, int);
void gemv(int, int);
void hst_l(int, int);
void hst_s(int, int);
void mlp(int, int);
void nw(int, int);
void red(int, int);
void scan_rss(int, int);
void scan_ssa(int, int);
void sel(int, int);
