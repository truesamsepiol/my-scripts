#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>

#ifndef DPU_BINARY
#define DPU_BINARY "./factoriel"
#endif

#define NR_DPUS 12


int main(void){
	struct dpu_set_t set, dpu;

	DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
	DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
	DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
	
	int i = 0;
	DPU_FOREACH(set, dpu, i){
		printf("round %d\n", i);
		DPU_ASSERT(dpu_log_read(dpu, stdout));
	}

	DPU_ASSERT(dpu_free(set));

	return 0;
}
