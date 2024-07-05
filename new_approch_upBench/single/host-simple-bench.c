#define _DEFAULT_SOURCE
#include "time.h"
#include <assert.h>
#include <dpu.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "header.h"

#ifndef DPU_BINARY
#define DPU_BINARY "./simple-bench"
#endif

void populate_mram(struct dpu_set_t set, uint32_t nr_dpus, int nb_loop) {
  struct dpu_set_t dpu;
  uint32_t each_dpu;
  uint64_t *buffer = malloc(8 * nr_dpus);
  DPU_FOREACH(set, dpu, each_dpu) {
    buffer[each_dpu] = nb_loop; // each dpu will compute a different checksum
    DPU_ASSERT(dpu_prepare_xfer(dpu, &buffer[each_dpu]));
  }
  DPU_FOREACH(set, dpu)
	  DPU_ASSERT(dpu_push_xfer(dpu, DPU_XFER_TO_DPU, "nb_loop", 0, 8, DPU_XFER_DEFAULT));
  free(buffer);
}

void simple_bench(int nr_dpus, int loop){

  pthread_t thread;

  struct timeval time_begin, time_end, off;
  int nb_dpu = nr_dpus;
  int nb_loop = loop == 0 ? 1 : loop;

  gettimeofday(&time_begin, NULL);
  DPU_ASSERT(dpu_alloc(nb_dpu, NULL, &dpu_set));
  struct dpu_set_t dpu;
  DPU_FOREACH(dpu_set, dpu)
  	DPU_ASSERT(dpu_load(dpu, DPU_BINARY, NULL));
  
  populate_mram(dpu_set, nb_dpu, nb_loop);

  DPU_ASSERT(dpu_launch(dpu_set, DPU_ASYNCHRONOUS));

  pthread_create(&thread, NULL, check_dpus_running, NULL);

  DPU_ASSERT(dpu_sync(dpu_set));

  pthread_join(thread, NULL);

  DPU_ASSERT(dpu_free(dpu_set));

  gettimeofday(&time_end, NULL);

  timersub(&time_end, &time_begin, &off);
  printf("Total time := %lis %li \n", off.tv_sec, off.tv_usec);
}
