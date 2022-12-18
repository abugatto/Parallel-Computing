#include <stdlib.h>
#include <stddef.h>

const char* dgemm_desc = "Blocked L1-L2-L3 dgemm.";

//Define cache sizes (in units of double)
#ifndef L1_BLOCK_SIZE
  #define L1_BLOCK_SIZE 32
#endif
#ifndef L2_BLOCK_SIZE
  #define L2_BLOCK_SIZE 128
#endif
#ifndef L3_BLOCK_SIZE
  #define L3_BLOCK_SIZE 1028
#endif

//define index inline functions for readability:
#define min(a,b) (((a)<(b)) ? (a) : (b))

//TODO STRASSEN

//to be used in the case of n or m being odd (vector x matrix multiplication)
//TODO: loop unrolling
static void naive_dgemm(int n, int M, int N, int K, double* A, double* B, double* C) {
  for(size_t j = 0; j < N; j++) {
    for(size_t i = 0; i < M; i++) {
      double cij = C[i + j*n];
      for(size_t k = 0; k < K; k++) {
        cij += A[i + k*n] * B[k + j*n];
      }

      C[i + j*n] = cij;
    }
  }
}

//TODO: LOOP UNROLLING

//to be used in the case of n or m being odd (vector x matrix multiplication)
//Easier for loop unrolling without branching
static void naive_dgemm_no_branching(int n, double* A, double* B, double* C) {
    const size_t blocksize = L1_BLOCK_SIZE;
    for(size_t j = 0; j < blocksize; j++) {
        for(size_t i = 0; i < blocksize; i++) {
            double cij = C[i + j*n];
            for(size_t k = 0; k < blocksize; k++) {
                //TODO: MKL Intrinsics
                cij += A[i + k*n] * B[k + j*n];
            }

            C[i + j*n] = cij;
        }
    }
}

/* This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are lda-by-lda matrices stored in column-major format.
 * On exit, A and B maintain their input values. */    
void square_dgemm(int n, double* A, double* B, double* C) {
    //Transpose B so that it's memory is accessed in contiguous blocks
    double* temp = (double*) malloc(n * n * sizeof(double));
    for(size_t i = 0; i < n; i++) {
        for(size_t j = 0; j < n; j++) {
            temp[j + i*n] = A[i + j*n];
        }
    }

    //RECURSIVE AND CHECK FOR SIZE IN BEGINNING

    //
    // L1 CACHE
    //

    /* For each row i of C */
    const size_t blocksizeL3 = L3_BLOCK_SIZE;
    for(size_t z = 0; z < n; z += blocksizeL3) {
        /* For each column j of B */
        for(size_t y = 0; y < n; y += blocksizeL3) {
            /* For each column k of A */
            for(size_t x = 0; x < n; x += blocksizeL3) {
                //accound for non even lda value
                size_t K3 = min(blocksizeL3, n-z);
                size_t N3 = min(blocksizeL3, n-y);
                size_t M3 = min(blocksizeL3, n-x);

                //
                // L2 CACHE
                //

                /* For each row i of C */
                const size_t blocksizeL2 = L2_BLOCK_SIZE;
                for(size_t r = 0; r < K3; r += blocksizeL2) {
                    /* For each column j of B */
                    for(size_t q = 0; q < N3; q += blocksizeL2) {
                        /* For each column k of A */
                        for(size_t p = 0; p < M3; p += blocksizeL2) {
                            //accound for non even lda value
                            size_t K2 = min(blocksizeL2, n-r);
                            size_t N2 = min(blocksizeL2, n-q);
                            size_t M2 = min(blocksizeL2, n-p);

                            //
                            // L3 CACHE
                            //

                            /* For each row i of C */
                            const size_t blocksizeL1 = L1_BLOCK_SIZE;
                            for(size_t k = 0; k < K2; k += blocksizeL1) {
                                /* For each column j of B */
                                for(size_t j = 0; j < N2; j += blocksizeL1) {
                                    /* For each column k of A */
                                    for(size_t i = 0; i < M2; i += blocksizeL1) {
                                        //accound for non even lda value
                                        size_t K = min(blocksizeL1, n-k);
                                        size_t N = min(blocksizeL1, n-j);
                                        size_t M = min(blocksizeL1, n-i);

                                        //compute block
                                        naive_dgemm(n, M, N, K, A + i + k*n, B + k + j*n, C + i + j*n);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //free memory for temp
    free(temp);
    temp = NULL;
}