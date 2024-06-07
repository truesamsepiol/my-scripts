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

struct args{
	int start;
	int end;
};

void *sum(void *);
void *fac(void *);
void *bfs(void *);
void *bs(void *);
void *gemv(void *);
void *hst_l(void *);
void *hst_s(void *);
void *mlp(void *);
void *nw(void *);
void *red(void *);
void *scan_rss(void *);
void *scan_ssa(void *);
void *sel(void *);
void *spmv(void *);
void *trns(void *);
void *ts(void *);
void *uni(void *);
void *va(void *);
