#ifndef _PARAMS_H_
#define _PARAMS_H_

#include "GEMV_support/common.h"

typedef struct Params {
    unsigned int  m_size;
    unsigned int  n_size;
    unsigned int  n_warmup;
    unsigned int  n_reps;
}Params;

static void usage();

struct Params input_params(int argc, char **argv);
#endif
