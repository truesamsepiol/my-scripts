#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <assert.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>

#include <dpu.h>
#include <dpu_log.h>
#include <dpu_error.h>
#include <dpu_types.h>
#include <dpu_runner.h>
#include <dpu_management.h>
#include <dpu_target_macros.h>
#include <dpu_program.h>
#include <dpu_management.h>
#include <dpu_config.h>


#define MAX_TRACES 10000
#define NR_PARAMETERS 7
#define SIZE_OF_COMMAND_PARAMETERS 30
#define NR_DPUS_MAX 20

#define SUM "./sum_elt_in_vector_dpu"

#define MICROSECONDES 1000000 

time_t start_time;

struct dpu_set_t set, dpu;
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
void spmv(int, int);
void trns(int, int);
void ts(int, int);
void uni(int, int);
void va(int, int);
