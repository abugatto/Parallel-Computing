#ifndef CONSTS_H_
#define CONSTS_H_

// maximum number of iterations
#define MAX_ITERS     35207

// image size
#define DIMS 2
#define IMAGE_WIDTH   4096
#define IMAGE_HEIGHT  4096

// the extent of the parameter plane ( MIN_X + iMIN_Y <= c < MAX_X + iMAX_Y ) 
#define MIN_X   -2.1
#define MAX_X    0.7
#define MIN_Y   -1.4
#define MAX_Y    1.4

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

typedef struct
{
    long nx;
    long ny;
    long startx;
    long starty;
    long endx;
    long endy;
} Domain;

typedef struct
{
    int y;
    int x;
    int nx;
    int ny;
    MPI_Comm comm;
} Partition;

/**
Structure Partition represents the layout of the grid of processes
organized in the Cartesian communicator (p.comm) that needs to be created (MPI_Cart_create)
and contains information such as number of processes in x and y direction (p.nx, p.ny)
and the coordinates of the current MPI process (p.x, p.y).

*/
Partition createPartition(int mpi_rank, int mpi_size)
{
  MPI_Comm cart;
  
  int dims[DIMS] = {0, 0};
  const int periods[DIMS] = {1, 1};
  int coordinates[DIMS];

  // TODO: determine size of the grid of MPI processes (p.nx, p.ny), see MPI_Dims_create()
  MPI_ERR_CHECK(MPI_Dims_create(mpi_size, DIMS, dims));

  // TODO: Create cartesian communicator (p.comm), we do not allow the reordering of ranks here, see MPI_Cart_create()
  MPI_ERR_CHECK(MPI_Cart_create(MPI_COMM_WORLD, DIMS, dims, periods, 0, &cart));
  
  // TODO: Determine the coordinates in the Cartesian grid (p.x, p.y), see MPI_Cart_coords()
  MPI_ERR_CHECK(MPI_Cart_coords(cart, mpi_rank, DIMS, coordinates));

  //define partition
  Partition p = {
    .x = coordinates[0], 
    .y = coordinates[1], 
    .nx = dims[0], 
    .ny = dims[1], 
    .comm = cart,
  };

  return p;
}

/**
Updates Partition structure to represent the process mpi_rank.
Copy the grid information (p.nx, p.ny and p.comm) and update
the coordinates to represent position in the grid of the given
process (mpi_rank)
*/
Partition updatePartition(Partition p_old, int mpi_rank)
{
  int coordinates[DIMS];
  MPI_ERR_CHECK(MPI_Cart_coords(p_old.comm, mpi_rank, DIMS, coordinates));
  
  //define partition
  Partition p = {
    .x = coordinates[0], 
    .y = coordinates[1], 
    .nx = p_old.nx, 
    .ny = p_old.ny, 
    .comm = p_old.comm,
  };

  return p;
}

/**
Structure Domain represents the information about the local domain of the current MPI process.
It holds information such as the size of the local domain (number of pixels in each dimension - d.nx, d.ny)
and its global indices (index of the first and the last pixel in the full image of the Mandelbrot set
that will be computed by the current process d.startx, d.endx and d.starty, d.endy).
*/
Domain createDomain(Partition p)
{
  const int nx = IMAGE_WIDTH / p.nx;
  const int ny = IMAGE_HEIGHT / p.ny;

  const long endx = (p.x + 1) * nx;
  const long endy = (p.y + 1) * ny;
  Domain domain = {
    .nx = nx, 
    .ny = ny, 
    .startx = p.x * nx, 
    .starty = p.y * ny, 
    .endx = endx, 
    .endy = endy,
  };

  return domain;
}


#endif /*CONSTS_H_*/
