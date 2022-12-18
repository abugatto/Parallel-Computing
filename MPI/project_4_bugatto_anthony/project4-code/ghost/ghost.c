/****************************************************************
 *                                                              *
 * This file has been written as a sample solution to an        *
 * exercise in a course given at the CSCS-USI Summer School     *
 * It is made freely available with the understanding that      *
 * every copy of this file must include this header and that    *
 * CSCS/USI take no responsibility for the use of the enclosed  *
 * teaching material.                                           *
 *                                                              *
 * Purpose: Exchange ghost cell in 2 directions using a topology*
 *                                                              *
 * Contents: C-Source                                           *
 *                                                              *
 ****************************************************************/

/* Use only 16 processes for this exercise
 * Send the ghost cell in two directions: left<->right and top<->bottom
 * ranks are connected in a cyclic manner, for instance, rank 0 and 12 are connected
 *
 * process decomposition on 4*4 grid
 *
 * |-----------|
 * | 0| 1| 2| 3|
 * |-----------|
 * | 4| 5| 6| 7|
 * |-----------|
 * | 8| 9|10|11|
 * |-----------|
 * |12|13|14|15|
 * |-----------|
 *
 * Each process works on a 6*6 (SUBDOMAIN) block of data
 * the D corresponds to data, g corresponds to "ghost cells"
 * xggggggggggx
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * gDDDDDDDDDDg
 * xggggggggggx
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DIMS 2
#define SUBDOMAIN 6
#define DOMAINSIZE (SUBDOMAIN+2)
#define at(i, j) ((i)*DOMAINSIZE + (j))

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

int main(int argc, char *argv[])
{
    MPI_ERR_CHECK(MPI_Init(&argc, &argv));

    int rank, size;
    MPI_ERR_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    MPI_ERR_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

    if (size!=16) {
        printf("please run this with 16 processors\n");
        MPI_ERR_CHECK(MPI_Finalize());
        exit(1);
    }

    // initialize the domain
    double data[DOMAINSIZE*DOMAINSIZE];
    for (int i=0; i<DOMAINSIZE*DOMAINSIZE; i++) {
        data[i]=rank;
    }

    // TODO: set the dimensions of the processor grid and periodic boundaries in both dimensions
    int dims[DIMS] = {4, 4};
    int periods[DIMS] = {1, 1};

    // TODO: Create a Cartesian communicator (4*4) with periodic boundaries (we do not allow
    // the reordering of ranks) and use it to find your neighboring
    // ranks in all dimensions in a cyclic manner.
    MPI_Comm cart;
    MPI_ERR_CHECK(MPI_Cart_create(MPI_COMM_WORLD, DIMS, dims, periods, 0, &cart));

    // TODO: find your top/bottom/left/right neighbor using the new communicator, see MPI_Cart_shift()
    int north, south, east, west;
    MPI_ERR_CHECK(MPI_Cart_shift(cart, 0, 1, &north, &south));
    MPI_ERR_CHECK(MPI_Cart_shift(cart, 1, 1, &west, &east));

    // TODO: create derived datatype data_ghost, create a datatype for sending the column, see MPI_Type_vector() and MPI_Type_commit()
    MPI_Datatype row, col;
    MPI_ERR_CHECK(MPI_Type_vector(1, SUBDOMAIN, 1, MPI_DOUBLE, &row));
    MPI_ERR_CHECK(MPI_Type_commit(&row));
    MPI_ERR_CHECK(MPI_Type_vector(SUBDOMAIN, 1, DOMAINSIZE, MPI_DOUBLE, &col));
    MPI_ERR_CHECK(MPI_Type_commit(&col));

    //  TODO: ghost cell exchange with the neighbouring cells in all directions
    //  use MPI_Irecv(), MPI_Send(), MPI_Wait() or other viable alternatives
    MPI_Request requests[4];
    MPI_Status status;

    //Asynchronous sends (ALL FIRST OR DEADLOCK)
    MPI_ERR_CHECK(MPI_Isend(&data[at(1, 1)], 1, row, north, rank, cart, &requests[2]));
    MPI_ERR_CHECK(MPI_Isend(&data[at(DOMAINSIZE - 2, 1)], 1, row, south, rank, cart, &requests[3]));
    MPI_ERR_CHECK(MPI_Isend(&data[at(1, 1)], 1, col, west, rank, cart, &requests[0]));
    MPI_ERR_CHECK(MPI_Isend(&data[at(1, DOMAINSIZE - 2)], 1, col, east, rank, cart, &requests[1]));

    //Recieve
    MPI_ERR_CHECK(MPI_Recv(&data[at(0, 1)], 1, row, north, north, cart, &status));
    MPI_ERR_CHECK(MPI_Recv(&data[at(DOMAINSIZE - 1, 1)], 1, row, south, south, cart, &status));
    MPI_ERR_CHECK(MPI_Recv(&data[at(1, 0)], 1, col, west, west, cart, &status));
    MPI_ERR_CHECK(MPI_Recv(&data[at(1, DOMAINSIZE - 1)], 1, col, east, east, cart, &status));

    //output results
    if (rank==9) {
        printf("data of rank 9 after communication\n");
        for (int j=0; j<DOMAINSIZE; j++) {
            for (int i=0; i<DOMAINSIZE; i++) {
                printf("%.1f ", data[i+j*DOMAINSIZE]);
            }
            printf("\n");
        }
    }

    //Free all variables and exit MPI
    MPI_Type_free(&row);
    MPI_Type_free(&col);
    MPI_Comm_free(&cart);
    MPI_Finalize();

    return 0;
}
