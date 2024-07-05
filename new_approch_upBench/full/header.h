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
#include <sys/time.h>

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
#define NR_DPUS_MAX_SIMULATION 20
#define NR_DPUS_MAX_PRODUCTION 64

#define SUM "./sum_elt_in_vector_dpu"

#define MICROSECONDES 500000 

struct timeval start_time;

struct dpu_set_t set, dpu;

struct dpu_set_t sub_set_of_dpus[NR_DPUS_MAX_SIMULATION];

struct parameter{
	char *path;
	char *out;
};

struct p{
	char *program[NR_PARAMETERS];
};

struct args{
	int start;
	int end;
	int loop;
};

void *simple_bench(void *);
