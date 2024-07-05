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

void populate_mram(int start, int end, uint32_t nr_dpus, int nb_loop) {
  uint64_t *buffer = malloc(8 * nr_dpus);
  uint32_t each_dpu = 0;
  for(uint32_t id = start; id <= end; id++, each_dpu++) {
    buffer[each_dpu] = nb_loop; // each dpu will compute a different checksum
    DPU_ASSERT(dpu_prepare_xfer(sub_set_of_dpus[id], &buffer[each_dpu]));
  }
  
  for(int id = start; id <= end; id++)
  	DPU_ASSERT(dpu_push_xfer(sub_set_of_dpus[id], DPU_XFER_TO_DPU, "nb_loop", 0, 8, DPU_XFER_DEFAULT));
  free(buffer);
}

void *simple_bench(void *args) {

  struct timeval time_begin, time_end, off;
  struct args *tmp = (struct args *)args;
  int start   = tmp->start;
  int end     = tmp->end;
  int nb_loop = tmp->loop;
  int nb_dpu  = (end - start + 1);
  printf("J'ai %d dpus\n", nb_dpu);

  gettimeofday(&time_begin, NULL);

  for(int id = start; id <= end; id++)
  	DPU_ASSERT(dpu_load(sub_set_of_dpus[id], DPU_BINARY, NULL));

  populate_mram(start, end, nb_dpu, nb_loop);

  gettimeofday(&time_end, NULL);

  timersub(&time_end, &time_begin, &off);
  printf("Total time := %lis %li \n", off.tv_sec, off.tv_usec);
}
