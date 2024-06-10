
#ifndef _PARAMS_H_
#define _PARAMS_H_

#include "common.h"
#include "utils.h"

static void usage() {
    PRINT(  "\nUsage:  ./program [options]"
            "\n"
            "\nBenchmark-specific options:"
            "\n    -f <F>    input matrix file name (default=SpMV_data/bcsstk30.mtx)"
            "\n"
            "\nGeneral options:"
            "\n    -v <V>    verbosity"
            "\n    -h        help"
            "\n\n");
}

typedef struct Params {
  const char* fileName;
  unsigned int verbosity;
} Params;

static struct Params spmv_input_params(int argc, char **argv, int nr_dpus) {
    struct Params p;


    int opt;
    while((opt = getopt(argc, argv, "f:v:h")) >= 0) {
        switch(opt) {
            case 'f': p.fileName    = optarg;       break;
            case 'v': p.verbosity   = atoi(optarg); break;
            case 'h': usage(); exit(0);
            default:
                      PRINT_ERROR("Unrecognized option!");
                      usage();
                      exit(0);
        }
    }

    if(nr_dpus == 1)
            p.fileName      = "SpMV_data/bcsstk30.mtx.1.mtx";
    else if(nr_dpus == 4)
            p.fileName      = "SpMV_data/bcsstk30.mtx.4.mtx";
    else if(nr_dpus == 16)
            p.fileName      = "SpMV_data/bcsstk30.mtx.16.mtx";
    else
            p.fileName      = "SpMV_data/bcsstk30.mtx.64.mtx";

    p.verbosity     = 0;

    return p;
}

#endif

