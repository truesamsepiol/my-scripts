#include "header.h"

#define FAC "./factoriel_dpu"

void fac(int start, int end){
	for(int id = start; id <= end; id++)
		DPU_ASSERT(dpu_load(sub_set_of_dpus[id], FAC, NULL));
}
