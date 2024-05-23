#include "header.h"

#define SUM "./sum_elt_in_vector_dpu"

void sum(int start, int end){
	for(int id = start; id <= end; id++)
		DPU_ASSERT(dpu_load(sub_set_of_dpus[id], SUM, NULL));
}
