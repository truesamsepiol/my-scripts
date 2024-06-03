/**
* app.c
* SCAN-RSS Host Application Source File
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dpu.h>
#include <dpu_log.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "../SCAN-RSS_support/common.h"
#include "../SCAN-RSS_support/timer.h"
#include "../SCAN-RSS_support/params.h"

#include "../header.h"

// Define the DPU Binary path as DPU_BINARY here
#ifndef DPU_BINARY
#define DPU_BINARY "./SCAN-RSS_bin/dpu_code"
#endif

#if ENERGY
#include <dpu_probe.h>
#endif

// Pointer declaration
static T* A;
static T* C;
static T* C2;

// Create input arrays
static void read_input(T* A, unsigned int nr_elements, unsigned int nr_elements_round) {
    srand(0);
    printf("nr_elements\t%u\t", nr_elements);
    for (unsigned int i = 0; i < nr_elements; i++) {
        A[i] = (T) (rand());
    }
    for (unsigned int i = nr_elements; i < nr_elements_round; i++) {
        A[i] = 0;
    }
}

// Compute output in the host
static void scan_host(T* C, T* A, unsigned int nr_elements) {
    C[0] = A[0];
    for (unsigned int i = 1; i < nr_elements; i++) {
        C[i] = C[i - 1] + A[i];
    }
}

int argc;
char **argv;
// Main of the Host Application
void scan_rss(int nr_dpus) {

    pthread_t thread;
    struct Params p = scan_rss_input_params(argc, argv);

    //struct dpu_set_t dpu_set, dpu;
    uint32_t nr_of_dpus;
    
#if ENERGY
    struct dpu_probe_t probe;
    DPU_ASSERT(dpu_probe_init("energy_probe", &probe));
#endif

    // Allocate DPUs and load binary
    DPU_ASSERT(dpu_alloc(nr_dpus, NULL, &dpu_set));
    DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));
    DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));
    printf("Allocated %d DPU(s)\n", nr_of_dpus);

    unsigned int i = 0;
    T accum = 0;

    const unsigned int input_size = p.exp == 0 ? p.input_size * nr_of_dpus : p.input_size; // Total input size (weak or strong scaling)
    const unsigned int input_size_dpu_ = divceil(input_size, nr_of_dpus); // Input size per DPU (max.)
    const unsigned int input_size_dpu_round = 
        (input_size_dpu_ % (NR_TASKLETS * REGS) != 0) ? roundup(input_size_dpu_, (NR_TASKLETS * REGS)) : input_size_dpu_; // Input size per DPU (max.), 8-byte aligned

    // Input/output allocation
    A = malloc(input_size_dpu_round * nr_of_dpus * sizeof(T));
    C = malloc(input_size_dpu_round * nr_of_dpus * sizeof(T));
    C2 = malloc(input_size_dpu_round * nr_of_dpus * sizeof(T));
    T *bufferA = A;
    T *bufferC = C2;

    // Create an input file with arbitrary data
    read_input(A, input_size, input_size_dpu_round * nr_of_dpus);

    // Timer declaration
    Timer timer;

    printf("NR_TASKLETS\t%d\tBL\t%d\n", NR_TASKLETS, BL);

    // Loop over main kernel
    for(int rep = 0; rep < p.n_warmup + p.n_reps; rep++) {

        // Compute output on CPU (performance comparison and verification purposes)
        if(rep >= p.n_warmup)
            scan_rss_start(&timer, 0, rep - p.n_warmup);
        scan_host(C, A, input_size);
        if(rep >= p.n_warmup)
            scan_rss_stop(&timer, 0);

        printf("Load input data\n");
        if(rep >= p.n_warmup)
            scan_rss_start(&timer, 1, rep - p.n_warmup);
        // Input arguments
        const unsigned int input_size_dpu = input_size_dpu_round;
        unsigned int kernel = 0;
        dpu_arguments_t input_arguments = {input_size_dpu * sizeof(T), kernel, 0};
        // Copy input arrays
        i = 0;
        DPU_FOREACH(dpu_set, dpu, i) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &input_arguments));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments), DPU_XFER_DEFAULT));
        DPU_FOREACH(dpu_set, dpu, i) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, bufferA + input_size_dpu * i));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, input_size_dpu * sizeof(T), DPU_XFER_DEFAULT));
        if(rep >= p.n_warmup)
            scan_rss_stop(&timer, 1);

        printf("Run program on DPU(s) \n");
        // Run DPU kernel
        if(rep >= p.n_warmup) {
            scan_rss_start(&timer, 2, rep - p.n_warmup);
            #if ENERGY
            DPU_ASSERT(dpu_probe_start(&probe));
            #endif
        }
 
        DPU_ASSERT(dpu_launch(dpu_set, DPU_ASYNCHRONOUS));
        pthread_create(&thread, NULL, check_dpus_running, NULL);
        if(rep >= p.n_warmup) {
            scan_rss_stop(&timer, 2);
            #if ENERGY
            DPU_ASSERT(dpu_probe_stop(&probe));
            #endif
        }

#if PRINT
        {
            unsigned int each_dpu = 0;
            printf("Display DPU Logs\n");
            DPU_FOREACH (dpu_set, dpu) {
                printf("DPU#%d:\n", each_dpu);
                DPU_ASSERT(dpulog_read_for_dpu(dpu.dpu, stdout));
                each_dpu++;
            }
        }
#endif

        printf("Retrieve results\n");
        dpu_results_t results[nr_of_dpus];
        T* results_scan = malloc(nr_of_dpus * sizeof(T));
        i = 0;
        accum = 0;
		
        if(rep >= p.n_warmup)
            scan_rss_start(&timer, 3, rep - p.n_warmup);
        // PARALLEL RETRIEVE TRANSFER
        dpu_results_t* results_retrieve[nr_of_dpus];

        DPU_FOREACH(dpu_set, dpu, i) {
            results_retrieve[i] = (dpu_results_t*)malloc(NR_TASKLETS * sizeof(dpu_results_t));
            DPU_ASSERT(dpu_prepare_xfer(dpu, results_retrieve[i]));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "DPU_RESULTS", 0, NR_TASKLETS * sizeof(dpu_results_t), DPU_XFER_DEFAULT));

        DPU_FOREACH(dpu_set, dpu, i) {
            // Retrieve tasklet timings
            for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
                if(each_tasklet == 0)
                    results[i].t_count = results_retrieve[i][each_tasklet].t_count;
            }
            free(results_retrieve[i]);
            // Sequential scan
            T temp = results[i].t_count;
            results_scan[i] = accum;
            accum += temp;
#if PRINT
            printf("i=%d -- %lu,  %lu, %lu\n", i, results_scan[i], accum, temp);
#endif
        }

        // Arguments for scan kernel (2nd kernel)
        kernel = 1;
        dpu_arguments_t input_arguments_2[nr_dpus];
        for(i=0; i<nr_of_dpus; i++) {
            input_arguments_2[i].size=input_size_dpu * sizeof(T); 
            input_arguments_2[i].kernel=kernel;
            input_arguments_2[i].t_count=results_scan[i];
        }
        DPU_FOREACH(dpu_set, dpu, i) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &input_arguments_2[i]));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments_2[0]), DPU_XFER_DEFAULT));
        if(rep >= p.n_warmup)
            scan_rss_stop(&timer, 3);

        printf("Run program on DPU(s) \n");
        // Run DPU kernel
        if(rep >= p.n_warmup) {
            scan_rss_start(&timer, 4, rep - p.n_warmup);
            #if ENERGY
            DPU_ASSERT(dpu_probe_scan_rss_start(&probe));
            #endif
        }
        DPU_ASSERT(dpu_launch(dpu_set, DPU_ASYNCHRONOUS));
        if(rep >= p.n_warmup) {
            scan_rss_stop(&timer, 4);
            #if ENERGY
            DPU_ASSERT(dpu_probe_stop(&probe));
            #endif
        }
#if PRINT
        {
            unsigned int each_dpu = 0;
            printf("Display DPU Logs\n");
            DPU_FOREACH (dpu_set, dpu) {
                printf("DPU#%d:\n", each_dpu);
                DPU_ASSERT(dpulog_read_for_dpu(dpu.dpu, stdout));
                each_dpu++;
            }
        }
#endif

        printf("Retrieve results\n");
        if(rep >= p.n_warmup)
            scan_rss_start(&timer, 5, rep - p.n_warmup);
        i = 0;
        // PARALLEL RETRIEVE TRANSFER
        DPU_FOREACH(dpu_set, dpu, i) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, bufferC + input_size_dpu * i));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, input_size_dpu * sizeof(T), input_size_dpu * sizeof(T), DPU_XFER_DEFAULT));
        if(rep >= p.n_warmup)
            scan_rss_stop(&timer, 5);

        // Free memory
        free(results_scan);
    }

    // Print timing results
    printf("CPU ");
    scan_rss_print(&timer, 0, p.n_reps);
    printf("CPU-DPU ");
    scan_rss_print(&timer, 1, p.n_reps);
    printf("DPU Kernel Reduction ");
    scan_rss_print(&timer, 2, p.n_reps);
    printf("Inter-DPU (Scan) ");
    scan_rss_print(&timer, 3, p.n_reps);
    printf("DPU Kernel Scan ");
    scan_rss_print(&timer, 4, p.n_reps);
    printf("DPU-CPU ");
    scan_rss_print(&timer, 5, p.n_reps);

    #if ENERGY
    double energy;
    DPU_ASSERT(dpu_probe_get(&probe, DPU_ENERGY, DPU_AVERAGE, &energy));
    printf("DPU Energy (J): %f\t", energy);
    #endif	


    // Check output
    bool status = true;
    for (i = 0; i < input_size; i++) {
        if(C[i] != bufferC[i]){ 
            status = false;
#if PRINT
            printf("%d: %lu -- %lu\n", i, C[i], bufferC[i]);
#endif
        }
    }
    if (status) {
        printf("[" ANSI_COLOR_GREEN "OK" ANSI_COLOR_RESET "] Outputs are equal\n");
    } else {
        printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "] Outputs differ!\n");
    }

    DPU_ASSERT(dpu_sync(dpu_set));
    pthread_join(thread, NULL);
    // Deallocation
    free(A);
    free(C);
    free(C2);
    DPU_ASSERT(dpu_free(dpu_set));
}
