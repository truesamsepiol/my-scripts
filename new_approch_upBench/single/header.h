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
#define NR_DPUS_MAX 64
#define MICROSECONDES 500000


struct timeval start_time;

struct dpu_set_t dpu_set, dpu;

struct parameter{
	char *path;
	char *out;
};

struct p{
	char *program[NR_PARAMETERS];
};

void *check_dpus_running(void *arg);

void simple_bench(int nr_dpus, int loop);
