#include "header.h"

#define SUM "./sum_elt_in_vector_dpu"

void *sum(void *args){
	struct args *tmp = (struct args *) args;
	int start = tmp->start;
	int end   = tmp->end;
	for(int id = start; id <= end; id++)
		DPU_ASSERT(dpu_load(sub_set_of_dpus[id], SUM, NULL));
}
