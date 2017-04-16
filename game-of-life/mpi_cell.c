#include "life.h"
//Build a MPI_Datatype for the cell struct
void Build_cell_type(int* x, int* y, int* oldstate, int* newstate, MPI_Datatype* mpi_cell) {

  int block_lengths[4];
  MPI_Aint displacements[4];
  MPI_Datatype typelist[4];
  MPI_Aint start_address;
  MPI_Aint address;

  block_lengths[0] = block_lengths[1] = block_lengths[2] = block_lengths[3] = 1;

  typelist[0] = MPI_INT;
  typelist[1] = MPI_INT;
  typelist[2] = MPI_INT;
  typelist[3] = MPI_INT;

  displacements[0] = 0;

  MPI_Address(x, &start_address);
  MPI_Address(y, &address);

  displacements[1] = address - start_address;
  MPI_Address(oldstate, &address);

  displacements[2] = address - start_address;
  MPI_Address(newstate, &address);

  displacements[3] = address - start_address;
  
  MPI_Type_struct(4, block_lengths, displacements, typelist, mpi_cell);
  MPI_Type_commit(mpi_cell);
}
