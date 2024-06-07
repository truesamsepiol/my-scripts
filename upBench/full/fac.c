#include "header.h"

#define FAC "./factoriel_dpu"

void *fac(void *args){

	struct args *args_tmp = (struct args *) args; 
	int start = args_tmp->start;
	int end   = args_tmp->end;
	for(int id = start; id <= end; id++)
		DPU_ASSERT(dpu_load(sub_set_of_dpus[id], FAC, NULL));
}
