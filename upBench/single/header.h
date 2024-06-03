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
#include <unistd.h>

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
#define MICROSECONDES 1000000


time_t start_time;

struct dpu_set_t dpu_set, dpu;

struct parameter{
	char *path;
};

struct p{
	char *program[NR_PARAMETERS];
};

void *check_dpus_running(void *arg);

void bfs(int nr_dpus);
void bs(int nr_dpus);
void gemv(int nr_dpus);
void hst_l(int nr_dpus);
void hst_s(int nr_dpus);
void mpl(int nr_dpus);
void nw(int nr_dpus);
void red(int nr_dpus);
void scan_rss(int nr_dpus);
void scan_ssa(int nr_dpus);
void sel(int nr_dpus);
void spmv(int nr_dpus);
void trns(unsigned int nr_dpus);
void ts(int nr_dpus);
void uni(int nr_dpus);
void va(int nr_dpus);
