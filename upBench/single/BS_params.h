#ifndef _PARAMS_H_
#define _PARAMS_H_

#include "BS_support/common.h"

typedef struct Params {
  long  num_querys;
  unsigned   n_warmup;
  unsigned   n_reps;
}Params;

void usage();

struct Params input_params(int argc, char **argv);
#endif
