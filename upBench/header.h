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

#define BFS_BINARY "./BFS_bin/dpu_code"

struct dpu_set_t set, dpu;

struct parameter{
	char *path;
};

struct p{
	char *program[NR_PARAMETERS];
};

void bfs(int argc, char *argv[]);
