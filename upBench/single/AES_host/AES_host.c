#include "../AES_PIM-common/common/include/common.h"
#include "../AES_PIM-common/host/include/host.h"
#include "common.h"
#include "crypto.h"
#include <dpu.h>
#include <time.h>
#include <assert.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <openssl/conf.h>
#include <openssl/evp.h>

#include "../header.h"

#define PIM_MODE 1
#define HOST_MODE 2
#define AESNI_MODE 3

#define TEST_KEY "hello world hello world"

#define USAGE                                                                  \
  "usage: %s\n"                                                                \
  "       %s dpu <number_of_dpus> <encrypt|decrypt> <data_length>\n"           \
  "       %s host <encrypt|decrypt> <data_length>\n"                           \
  "       %s aesni <encrypt|decrypt> <data_length>\n"                          \
  "Encrypt or decrypt a buffer of the specified size and output performance "  \
  "information.\n"                                                             \
  "By default, 16KB is encrypted using one DPU.\n\n"                           \
  ""                                                                           \
  "number_of_dpus:\tNumber of DPUs to spread data across. Must be larger "     \
  "than 0.\n"                                                                  \
  "data_length:\tAmount of data to use. Defaults to bytes, but may use K, M, " \
  "or G instead.\n\n"                                                          \
  ""                                                                           \
  "Available modes:\n"                                                         \
  "\tdpu\tPerform encryption in memory, using the specified number of DPUs\n"  \
  "\thost\tPerform encryption on the CPU\n"                                    \
  "\taesni\tPerform encryption on the CPU using AES-NI hardware"               \
  " acceleration\n"

#define PRINT_USAGE() printf("CONFERE USAGE\n");//ERROR(USAGE, argv[0], argv[0], argv[0], argv[0])

int dpu_AES_ecb(void *in, void *out, unsigned long length, const void *key_ptr,
                int operation, unsigned int nr_of_dpus) {

   pthread_t thread;
  if (operation != OP_ENCRYPT && operation != OP_DECRYPT) {
    ERROR("Invalid operation\n");
    return -1;
  }

  if (length % AES_BLOCK_SIZE_BYTES != 0) {
    ERROR("Length is not a multiple of block size\n");
    return -1;
  }

  struct timespec times[9];

  //struct dpu_set_t dpu_set, dpu;

  //clock_gettime(CLOCK_MONOTONIC_RAW, times); // start

  DPU_ASSERT(dpu_alloc(nr_of_dpus, NULL, &dpu_set));

  //clock_gettime(CLOCK_MONOTONIC_RAW, times+1); // DPUs allocated

  DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));
  int chunk_size = length / nr_of_dpus;

  if (chunk_size > MRAM_SIZE) { // More data than will fit in MRAM
    ERROR("Data does not fit in MRAM (%ld bytes into %d DPUs)\n", length, nr_of_dpus);
    DPU_ASSERT(dpu_free(dpu_set));
    return -1;
  }

  if (chunk_size % AES_BLOCK_SIZE_BYTES != 0) { // Some blocks are not whole
    ERROR("Length is not a multiple of block size when split across %d DPUs\n", nr_of_dpus);
    DPU_ASSERT(dpu_free(dpu_set));
    return -1;
  }

  if (length % chunk_size != 0) { // Data does not fit evenly onto DPUs
    ERROR("%ld bytes cannot be split evenly across %d DPUs\n", length, nr_of_dpus);
    DPU_ASSERT(dpu_free(dpu_set));
    return -1;
  }

  //DEBUG("Using %4.d DPU(s) %2.d tasklets, ", nr_of_dpus, NR_TASKLETS);

  if (operation == OP_ENCRYPT) {
	  DPU_FOREACH(dpu_set, dpu)
    		DPU_ASSERT(dpu_load(dpu, DPU_ENCRYPT_BINARY, NULL));
  } else {
	  DPU_FOREACH(dpu_set, dpu)
    		DPU_ASSERT(dpu_load(dpu, DPU_DECRYPT_BINARY, NULL));
  }

  //clock_gettime(CLOCK_MONOTONIC_RAW, times+2); // DPUs loaded

  uint64_t offset = 0;

  //struct dpu_set_t dpu;
  DPU_FOREACH(dpu_set, dpu) {

#ifndef NOBULK
    DPU_ASSERT(dpu_prepare_xfer(dpu, in + offset));
#else
    DPU_ASSERT(
        dpu_copy_to(dpu, XSTR(DPU_DATA_BUFFER), 0, in + offset, chunk_size));
#endif

    offset += chunk_size;
  }

#ifndef NOBULK
  DPU_FOREACH(dpu_set, dpu)
	  dpu_push_xfer(dpu, DPU_XFER_TO_DPU, XSTR(DPU_DATA_BUFFER), 0, chunk_size, DPU_XFER_DEFAULT);
#endif

  //clock_gettime(CLOCK_MONOTONIC_RAW, times+3); // Data transferred to DPUs

  DPU_FOREACH(dpu_set, dpu)
	  DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_KEY_BUFFER), 0, key_ptr, DPU_KEY_BUFFER_SIZE));
  DPU_FOREACH(dpu_set, dpu)
  	DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_LENGTH_BUFFER), 0, &chunk_size, sizeof(chunk_size)));

  //clock_gettime(CLOCK_MONOTONIC_RAW, times+4); // Key and data size copied to DPUs

  DPU_ASSERT(dpu_launch(dpu_set, DPU_ASYNCHRONOUS));

  pthread_create(&thread, NULL, check_dpus_running, NULL);

  DPU_ASSERT(dpu_sync(dpu_set));
    
  pthread_join(thread, NULL);

  //clock_gettime(CLOCK_MONOTONIC_RAW, times+5); // DPUs launched, data encrypted

  /*DEBUG("%s took %3.2fs ",
         (operation == OP_ENCRYPT) ? "encryption" : "decryption",
         TIME_DIFFERENCE(times[0], times[5]) );

  uint64_t perfcount_min = 0;
  uint64_t perfcount_max = 0;
  uint64_t perfcount_avg = 0;
  offset = 0;

  DPU_FOREACH(dpu_set, dpu) {

#ifndef NOBULK
    DPU_ASSERT(dpu_prepare_xfer(dpu, out + offset));
#else
    DPU_ASSERT(
        dpu_copy_from(dpu, XSTR(DPU_DATA_BUFFER), 0, out + offset, chunk_size));
#endif

    offset += chunk_size;
  }

#ifndef NOBULK
  dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, XSTR(DPU_DATA_BUFFER), 0, chunk_size, DPU_XFER_DEFAULT);
#endif

  clock_gettime(CLOCK_MONOTONIC_RAW, times+6); // Encrypted data copied from DPUs

  DPU_FOREACH(dpu_set, dpu) {
    uint64_t perfcount;
    dpu_copy_from(dpu, "dpu_perfcount", 0, &perfcount, sizeof(perfcount));

    perfcount_min = (perfcount_min == 0) ? perfcount : perfcount_min;
    perfcount_min = (perfcount_min < perfcount) ? perfcount_min : perfcount;
    perfcount_max = (perfcount_max > perfcount) ? perfcount_max : perfcount;
    perfcount_avg += perfcount;
  }

  perfcount_avg /= nr_of_dpus;
  DEBUG("Performance count %s %10.ld avg, %10.ld min, %10.ld max\n",
        XSTR(PERFCOUNT_TYPE), perfcount_avg, perfcount_min, perfcount_max);

  clock_gettime(CLOCK_MONOTONIC_RAW, times+7); // Performance counts retrieved
  */

  DPU_ASSERT(dpu_free(dpu_set));

  /*clock_gettime(CLOCK_MONOTONIC_RAW, times+8); // DPUs freed

  // Parse and output the data for experiments
  // 
  // TODO: This is dirty, it shouldn't be done here. The plan is to use shell
  // scripts to run the binary multiple times, so the header needs to be
  // printed by the script instead. But having the header and data format
  // string in different places makes them prone to misalignment. This also
  // makes it hard to change timing sections. A better way might be a
  // function with static arrays of timing data which automatically records
  // each consecutive measurement to the next index, plus another function
  // for processing and outputting everything.

  // MEASURE("Tasklets,DPUs,Operation,Data size,Allocation time,Loading
  // time,Data copy in,Parameter copy in,Launch,Data copy out, Performance count
  // copy out,Free DPUs,Performance count type,Performance count min, max,
  // average\n");

#ifdef EXPERIMENT
  double times_adjusted[9];
  for (int i = 1; i < 9; i++) {
    times_adjusted[i] = TIME_DIFFERENCE(times[i-1], times[i]);
  }

  MEASURE("%d,%d,%d,%ld,%.12f,%.12f,%.12f,%.12f,%.12f,%.12f,%.12f,%.12f,%s,%ld,%ld,%ld\n", NR_TASKLETS,
          nr_of_dpus, operation, length, times_adjusted[1], times_adjusted[2],
          times_adjusted[3], times_adjusted[4], times_adjusted[5],
          times_adjusted[6], times_adjusted[7], times_adjusted[8],
          XSTR(PERFCOUNT_TYPE), perfcount_min, perfcount_max, perfcount_avg);
#endif
*/
  return 0;
}

void handleErrors(void) {
  ERR_print_errors_fp(stderr);
  abort();
}

int aesni_AES_ecb(void *in, void *out, unsigned long length,
                  const void *key_ptr, int operation) {
  struct timespec start, end;
  //clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  // OpenSSL only accepts an integer length, so we reject lengths any longer
  // than that
  if (length > INT_MAX) {
    return EINVAL;
  }

  if (operation == OP_ENCRYPT) {
    operation = 1;
  } else if (operation == OP_DECRYPT) {
    operation = 0;
  } else {
    return EINVAL;
  }

  EVP_CIPHER_CTX *ctx;

  if (!(ctx = EVP_CIPHER_CTX_new())) {
    handleErrors();
  }

  if (1 != EVP_CipherInit_ex(ctx, EVP_aes_128_ecb(), NULL, key_ptr, NULL,
                             operation)) {
    handleErrors();
  }

  int outl;

  if (1 != EVP_CipherUpdate(ctx, out, &outl, in, length)) {
    handleErrors();
  }

  if (length != (unsigned int)outl) {
    ERROR("Error: OpenSSL did not encrypt all data\n");
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }

  EVP_CIPHER_CTX_free(ctx);

  //clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  double execution_time = TIME_DIFFERENCE(start, end);

  // TODO: add a cycle count
  // Operation, Data size, Execution time
  MEASURE("%d,%ld,%.12f\n", operation, length, execution_time);

  DEBUG("%sed %ld bytes in %fs\n", (operation == 1) ? "Encrypt" : "Decrypt",
        length, execution_time);

  return 0;
}

void aes(int nr_dpus) {

  unsigned long test_data_size;
  // Assigned to suppress a "may be uninitialized" warning
  //unsigned int nr_of_dpus = 0;
  //int mode;
  //int operation;
  //const char **mandatory_args;

  // Parse the mode

  // Default - no arguments
  //mode = PIM_MODE;
  //nr_of_dpus = nr_dpus;
  /*if (argc == 1) {
    test_data_size = KILOBYTE(16); // good for simulation
    nr_of_dpus = 1;
    mode = PIM_MODE;
    operation = OP_ENCRYPT;
    DEBUG("Performing encryption with %lu bytes and %d DPUs\n\n",
          test_data_size, nr_of_dpus);
  } else {
    if (strcmp(argv[1], "dpu") == 0) {
      mode = PIM_MODE;
    } else if (strcmp(argv[1], "host") == 0) {
      mode = HOST_MODE;
    } else if (strcmp(argv[1], "aesni") == 0) {
      mode = AESNI_MODE;
    } else {
      PRINT_USAGE();
      exit(1);
    }*/

    /*if (mode == PIM_MODE) {
      // PIM mode
      if (argc != 5) {
        PRINT_USAGE();
        return 1;
      }
      mandatory_args = argv + 3;

      nr_of_dpus = nr_dpus;

      if (nr_of_dpus == 0) {
        PRINT_USAGE();
        return 1;
      }
    } else {
      // Host and AES-NI modes
      if (argc != 4) {
        PRINT_USAGE();
        return 1;
      }
      mandatory_args = argv + 2;
    }
  }*/
  /*if (argc != 1) {
    // Parse <encrypt|decrypt>
    if (strcmp(mandatory_args[0], "encrypt") == 0) {
      operation = OP_ENCRYPT;
    } else if (strcmp(mandatory_args[0], "decrypt") == 0) {
      operation = OP_DECRYPT;
    } else {
      PRINT_USAGE();
      return 1;
    }*/

    // Parse <data_length>
    //char *unit;
    //test_data_size = strtol(mandatory_args[1], &unit, 0);
    test_data_size = 64;
    char unit = 'M';

    //if (test_data_size == 0) {
      //PRINT_USAGE();
      //return 1;
    //}

    switch (unit) {
    case '\0':
      break; // default: bytes
    case 'K':
      test_data_size = KILOBYTE(test_data_size);
      break;
    case 'M':
      test_data_size = MEGABYTE(test_data_size);
      break;
    case 'G':
      test_data_size = GIGABYTE(test_data_size);
      break;
    default:
      PRINT_USAGE();
      //return 1;
    }

    /*if (mode == PIM_MODE) {
      DEBUG("Performing %sion with %lu bytes and %d DPUs\n\n",
            (argc == 1) ? "encrypt" : mandatory_args[0], test_data_size,
            nr_of_dpus);
    } else {
      DEBUG("Performing %sion in %s mode with %lu bytes\n\n", mandatory_args[0],
            argv[1], test_data_size);
    }*/
  //}

  unsigned long long *buffer, *out = NULL; // fix -Werror=maybe-uninitialized
  //if (mode == AESNI_MODE && test_data_size > INT_MAX) {
    // We can't do an AES-NI operation this large all at once, so we break
    // it into 1GB chunks and re-use most of the same buffer
    //
    // This also makes it possible to do 32GB AES-NI operations on
    // upmemcloud1, which can't fit two 32GB buffers because it only has
    // 64GB of non-PIM memory
    //buffer = malloc(test_data_size + GIGABYTE(1));
  //} else {
    buffer = malloc(test_data_size);
  //}

  // OpenSSL doesn't allow overlapping in and out buffers
  //if (mode == AESNI_MODE && test_data_size <= INT_MAX) {
    //out = malloc(test_data_size);
    //if (out == NULL) {
      //ERROR("Could not allocate output buffer.\n");
      //return 1;
    //}
  //}

  if (buffer == NULL) {
    ERROR("Could not allocate test data buffer.\n");
    exit(1);
  }

  unsigned char key[DPU_KEY_BUFFER_SIZE];
  memcpy(key, TEST_KEY, DPU_KEY_BUFFER_SIZE);

  int error = dpu_AES_ecb(buffer, buffer, test_data_size, key, OP_ENCRYPT, nr_dpus);
  /*switch (mode) {
  case PIM_MODE:
    error =
        dpu_AES_ecb(buffer, buffer, test_data_size, key, operation, nr_of_dpus);
    break;
  case HOST_MODE:
    error = host_AES_ecb(buffer, buffer, test_data_size, key, operation);
    break;
  case AESNI_MODE:
    if (test_data_size <= INT_MAX) {
      error = aesni_AES_ecb(buffer, out, test_data_size, key, operation);
    } else {
      int chunk_size = GIGABYTE(1) / sizeof(unsigned long long);
      unsigned long long *buffer_end =
          buffer + (test_data_size + GIGABYTE(1)) / sizeof(unsigned long long);
      unsigned long long *inp = buffer + chunk_size, *outp = buffer;

      while (inp + chunk_size <= buffer_end) {
        aesni_AES_ecb(inp, outp, GIGABYTE(1), key, operation);
        inp += chunk_size;
        outp += chunk_size;
      }

      unsigned long long extra_data = test_data_size % GIGABYTE(1);
      if (extra_data != 0) {
        aesni_AES_ecb(inp, outp, extra_data, key, operation);
      }
    }
    break;
  default:
    ERROR("Unknown mode! This should be an unreachable state!");
    exit(1);
  }

  if (error != 0) {
    ERROR("Operation failed (errno %d)\n", error);
    return 1;
  }*/

  free(buffer);

  /*if (mode == AESNI_MODE && test_data_size <= INT_MAX) {
    free(out);
  }

  return 0;*/
}
