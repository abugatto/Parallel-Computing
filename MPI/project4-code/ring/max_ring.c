/****************************************************************
 *                                                              *
 * This file has been written as a sample solution to an        *
 * exercise in a course given at the CSCS-USI Summer School.    *
 * It is made freely available with the understanding that      *
 * every copy of this file must include this header and that    *
 * CSCS/USI take no responsibility for the use of the enclosed  *
 * teaching material.                                           *
 *                                                              *
 * Purpose: Parallel maximum using a ping-pong                      *
 *                                                              *
 * Contents: C-Source                                           *
 *                                                              *
 ****************************************************************/


#include <stdio.h>
#include <mpi.h>

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


int main (int argc, char *argv[])
{
    int rank, size;
    int snd_buf, rcv_buf;
    int right, left;
    int max, i;

    MPI_Status status;
    MPI_Request request;

    MPI_ERR_CHECK(MPI_Init(&argc, &argv));

    MPI_ERR_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));

    MPI_ERR_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

    right = (rank - 1 + size) % size; /* get rank of neighbor to your right */
    left  = (rank + 1) % size; /* get rank of neighbor to your left */

    /* Implement ring maximum code
     * do not use if (rank == 0) .. else ..
     * every rank sends initialy its rank number to a neighbor, and then sends what
     * it receives from that neighbor, this is done n times with n = number of processes
     * all ranks will obtain the max.
     */

    max = 0;
    int maxtemp = (3 * rank) % (2 * size);
    for (int t = 0; t < size; t++) {
      //Sends messages asynchronously
      MPI_Request req;
      MPI_ERR_CHECK(MPI_Isend(&maxtemp, 1, MPI_INT, left, t, MPI_COMM_WORLD, &req));

      //Recieve the message synchronously
      MPI_Status status;
      MPI_ERR_CHECK(MPI_Recv(&maxtemp, 1, MPI_INT, right, t, MPI_COMM_WORLD, &status));
      
      //Compute max and wait for send and recieve to finish
      max = max > maxtemp ? max : maxtemp;
      MPI_ERR_CHECK(MPI_Wait(&req, &status));
    }

    printf ("Process %i:\tMax = %i\n", rank, max);

    MPI_Finalize();
    return 0;
}
