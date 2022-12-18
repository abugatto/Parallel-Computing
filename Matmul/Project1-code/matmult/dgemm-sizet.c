/* 
    Please include compiler name below (you may also include any other modules you would like to be loaded)

COMPILER= gnu

    Please include All compiler flags and libraries as you want them run. You can simply copy this over from the Makefile's first few lines
 
CC = cc
OPT = -O3
CFLAGS = -Wall -std=gnu99 $(OPT)
MKLROOT = /opt/intel/composer_xe_2013.1.117/mkl
LDLIBS = -lrt -Wl,--start-group $(MKLROOT)/lib/intel64/libmkl_intel_lp64.a $(MKLROOT)/lib/intel64/libmkl_sequential.a $(MKLROOT)/lib/intel64/libmkl_core.a -Wl,--end-group -lpthread -lm

*/

#include <stdlib.h>
#include <stddef.h>

const char* dgemm_desc = "Simple size_t dgemm.";

//Define cache sizes (in units of double)
#ifndef L1_BLOCK_SIZE
  #define L1_BLOCK_SIZE 18
#endif

//define index inline functions for readability:
#define min(a,b) (((a)<(b)) ? (a) : (b))

//function for computing a block
static void compute_block(int n, int M, int N, int K, double* A, double* B, double* C) {
  for(size_t j = 0; j < N; j++) {
    for(size_t i = 0; i < M; i++) {
      double cij = C[i + j*n];
      for(size_t k = 0; k < K; k++) {
        //TODO: STRASSEN
        //TODO: MKL Intrinsics

        cij += A[i + k*n] * B[k + j*n];
      }

      C[i + j*n] = cij;
    }
  }
}

//SIZE_T uses loop unrolling so having the mins at each loop results in less computations

/* This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are lda-by-lda matrices stored in column-major format.
 * On exit, A and B maintain their input values. */    
void square_dgemm(int n, double* A, double* B, double* C) {
  //TODO: transpose

  //TODO: block size

  /* For each row i of C */
  for (size_t k = 0; k < n; k += L1_BLOCK_SIZE) {
    /* For each column j of B */
    int K = min(L1_BLOCK_SIZE, n-k);
    for (size_t j = 0; j < n; j += L1_BLOCK_SIZE) {
      /* For each column k of A */
      int N = min(L1_BLOCK_SIZE, n-j);
      for(size_t i = 0; i < n; i += L1_BLOCK_SIZE) {
        //accound for non even lda value
        int M = min(L1_BLOCK_SIZE, n-i);

        //TODO: compute or recurse to next cache

        //compute block 
        compute_block(n, M, N, K, A + i + k*n, B + k + j*n, C + i + j*n);
      }
    }
  }
}

