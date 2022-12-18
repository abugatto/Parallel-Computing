/****************************************************************
 *                                                              *
 * This file has been written as a sample solution to an        *
 * exercise in a course given at the CSCS-USI Summer School.    *
 * It is made freely available with the understanding that      *
 * every copy of this file must include this header and that    *
 * CSCS/USI take no responsibility for the use of the enclosed  *
 * teaching material.                                           *
 *                                                              *
 * Purpose: : Parallel matrix-vector multiplication and the     *
 *            and power method                                  *
 *                                                              *
 * Contents: C-Source                                           *
 *                                                              *
 ****************************************************************/


#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hpc-power.h"

#define ROOT_RANK 0
#define MPI_ERR_CHECK(call)                                                    \
  {                                                                            \
    do {                                                                       \
      int err = call;                                                          \
      if (err != MPI_SUCCESS) {                                                \
        char errstr[MPI_MAX_ERROR_STRING];                                     \
        int szerrstr;                                                          \
        MPI_Error_string(err, errstr, &szerrstr);                              \
        fprintf(stderr, "MPI error at %s:%i : %s\n", __FILE__, __LINE__,       \
                errstr);                                                       \
        MPI_Abort(MPI_COMM_WORLD, 1);                                          \
      }                                                                        \
    } while (0);                                                               \
  }

///////////////////////////////////////////////////////////////////////////////////

void printVector(double *X, int n) {
  printf("  [");
  for(int i = 0; i < n; i++) {
    printf("%.2lf ", X[i]);
  }
  printf("]\n");
}

void printMatrix(double *A, int m, int n) {
  printf("[\n");
  for(int i = 0; i < m; i++) {
    printf("  ");
    printVector(&A[i * n], n);
  }
  printf("]\n");
}

double normL2(double *X, int n) {
  double sum = 0.0;
  for(int i = 0; i < n; i++) {
    sum += X[i] * X[i];
  }

  return sqrt(sum);
}

void vectorScalarMultiply(double *X, double c, int n) {
  for(int i = 0; i < n; i++) {
    X[i] *= c;
  }
}

double innerProduct(double *X, double *Y, int n) {
  double sum = 0;
  for(int i = 0; i < n; i++) {
    sum += X[i] * Y[i];
  }

  return sum;
}

void matrixVectorMultiply(double *Y, double *A, double *X, int m, int n) {
  for(int i = 0; i < m; i++) {
    Y[i] = innerProduct(&A[i * n], X, n);
  }
}

///////////////////////////////////////////////////////////////////////////////////

//Run one iteration of the power method
void powerMethodUpdate(int rank, double *Y, double *A, double *X, int m, int n) {
    //Compute normalized X from root
    if(rank == ROOT_RANK) {
        vectorScalarMultiply(X, (double) 1.0 / normL2(X, n), n);
    }

    //Broadcast X to all processes from root
    MPI_ERR_CHECK(MPI_Bcast(X, n, MPI_DOUBLE, ROOT_RANK, MPI_COMM_WORLD));

    //Compute intermediate Y = AX
    matrixVectorMultiply(Y, A, X, m, n);

    //Reduce Y from all processes into the root process
    MPI_ERR_CHECK(MPI_Gather(Y, m, MPI_DOUBLE, X, m, MPI_DOUBLE, ROOT_RANK, MPI_COMM_WORLD));
}

void powerMethod(double *lambda, double *runtime, int rank, int size, int n, int its) {
    //Initialize Y, A, X
    const int rows = n / size;
    double *A = hpc_generateMatrix(n, rank * rows, rows);
    double *X = malloc(n * sizeof(double));
    double *Y = NULL;

    //Declare X and Y depending on root or other process
    srand(time(NULL));
    if(rank == ROOT_RANK) {
        for(int i = 0; i < n; i++) {
            X[i] = (rand() % 100) / 100.0;
        }

        Y = malloc(n * sizeof(double));
    } else {
        Y = malloc(rows * sizeof(double));
    }

    //Compute Power Method updates
    const double t0 = (rank == ROOT_RANK) ? MPI_Wtime() : 0;
    for(int i = 0; i < its; i++) {
        powerMethodUpdate(rank, Y, A, X, rows, n);
    }

    const double t1 = (rank == ROOT_RANK) ? MPI_Wtime() : 0;

    //Compute Lambda and runtime in root process
    if(rank == ROOT_RANK) {
        *runtime = t1 - t0;
        *lambda = normL2(X,n);
    }

    //Free vars
    free(X);
    free(Y);
    free(A);
}

///////////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
    //Initialize MPI and process args
    int n, its;
    MPI_ERR_CHECK(MPI_Init(&argc, &argv));
    sscanf(argv[1], "%d", &n);
    sscanf(argv[2], "%d", &its);

    //Get rank and size
    int rank, size;
    MPI_ERR_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    MPI_ERR_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

    //Run power method (lambda and runtime will be greater than 0 only in root)
    double lambda = 0.0;
    double runtime = 0.0;
    powerMethod(&lambda, &runtime, rank, size, n, its);

    //Print and append data to file
    if(rank == ROOT_RANK) {
        printf("lambda : %lf\nruntime : %lf\n", lambda, runtime);

        FILE *file = fopen("data.csv", "a");
        fprintf(file, "%d, %d, %lf,\n", n, size, runtime);
        fclose(file);
    }

    //Quit MPI
    MPI_Finalize();
    return 0;
}
