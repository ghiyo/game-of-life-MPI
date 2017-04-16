

//Author(s): Omid Ghiyasian, Ensieh Mollazadeh, Ryan Mak

#include "life.h"
int main(int argc, char* argv[])
{
  //Process variables for their grid sizes, where theey start and where they end on the overall window
  int row_col[2];
  int start[2];
  int end[2];

  int p, my_rank, q;

  double begin, finish;

  //MPI cartesian variables
  MPI_Comm grid_comm;
  int *dim_sizes;
  int wrap_around[2];
  int coordinates[2];
  int reorder = 1;
  int my_grid_rank, grid_rank;

  MPI_Datatype mpi_cell;
  MPI_Status status;

  char filename[128];
  MPI_File myfile;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  //Create the MPI cell type for writing reading and communication of processes
  int n = 0;
  Build_cell_type(&n, &n, &n, &n, &mpi_cell);

  //Calculate the dimension of the grid size of processes. E.g. If input is 12 processes the most reasonable grid size is 3x4 or 4x3. In this program 3x4 is chosen (the rows are always less than the columns).
  dim_sizes = dim_calc(p);

  wrap_around[0] = wrap_around[1] = 1;

  MPI_Cart_create(MPI_COMM_WORLD, 2, dim_sizes, wrap_around, reorder, &grid_comm);
  MPI_Comm_rank(grid_comm, &my_grid_rank);
  MPI_Cart_coords(grid_comm, my_grid_rank, 2, coordinates);
  MPI_Cart_rank(grid_comm, coordinates, &grid_rank);

  //Process 0 will calculate and send the boundaries of which the sizes of the 2D arrays is determined, where they start, and where they end on the overall window of cells to the other processes.
  if (my_rank == 0)
  {
    bound_calc(dim_sizes, ROWNUM, COLNUM);

    //If the dimensions of the processes is greater than the dimensions of the overall window then the window cannot be disected into reasonable pieces so the program will exit.
    if (dim_sizes[0] > ROWNUM || dim_sizes[1] > COLNUM)
    {
      printf("Invalid sizes! Process Row x Col: %d x %d and Window size for cells is %d x %d   Either Process Row %d > Window row %d OR Process Col %d > Window col %d\n",dim_sizes[0],dim_sizes[1],ROWNUM,COLNUM,dim_sizes[0],ROWNUM,dim_sizes[1],COLNUM);
      MPI_Abort(MPI_COMM_WORLD,1);
    }
  }
  //Receive the boundary information
  MPI_Recv(&row_col, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
  MPI_Recv(&start, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
  MPI_Recv(&end, 2, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

  //Begin timer
  begin = MPI_Wtime();

  int r = row_col[0];
  int c = row_col[1];

  //Calculate where each process will start reading from
  int offset = (start[0]*COLNUM + start[1]) * sizeof(cell);
  int readoffset;

  //Create the 2D array for the cells
  cell **readingArray;

  readingArray = (cell **)calloc(r+2, sizeof(cell *));
  for(int i = 0;i < r+2; ++i)
  	readingArray[i] = (cell *)calloc(c+2, sizeof(cell));

  MPI_File_open(MPI_COMM_SELF, FILENAME, MPI_MODE_RDWR, MPI_INFO_NULL, &myfile);

  //Read based on boundary of each process. i.e if process 0 has row x col 3x4 then it will read 4 cells starting at the offset in the file, and loop 3 times changing the offset to read again at the correct location to fill its 2D array.
  for (int i = 0; i < r; i++)
  {
    readoffset = offset + i*COLNUM*sizeof(cell);
    MPI_File_set_view(myfile,readoffset, mpi_cell, mpi_cell, "native", MPI_INFO_NULL);
    MPI_File_read_all(myfile,&readingArray[i+1][1],c*sizeof(cell)/4,mpi_cell, &status);
  }

  //Create arrays for the ghost cells that are to be passed and received (on the sides not corners)
  cell *sendLeftSideArray;
  cell *sendRightSideArray;
  cell *recvLeftSideArray;
  cell *recvRightSideArray;

  sendLeftSideArray = (cell *)calloc(r, sizeof(cell));
  sendRightSideArray = (cell *)calloc(r, sizeof(cell));
  recvLeftSideArray = (cell *)calloc(r, sizeof(cell));
  recvRightSideArray = (cell *)calloc(r, sizeof(cell));

  //Update the 2D array and write them IMAGENUM of times to the file by appending each write
  for (int counter = 1; counter < IMAGENUM; counter++)
  {
    //Fill the left and right side arrays for ghost points
    for (int i = 0; i < r; i++)
    {
      sendLeftSideArray[i] = readingArray[i+1][1];
      sendRightSideArray[i] = readingArray[i+1][c];
    }
    //Send the left side ghost cells and receive on the right side
    MPI_Sendrecv(sendLeftSideArray, r*sizeof(cell)/4, mpi_cell, coordinates[0]*(dim_sizes[1]) + ((coordinates[1] + dim_sizes[1] - 1) % dim_sizes[1]), 0, recvRightSideArray, r*sizeof(cell)/4, mpi_cell, coordinates[0]*(dim_sizes[1]) + ((coordinates[1] + 1) % dim_sizes[1]), 0, MPI_COMM_WORLD, &status);
    //Send top left side ghost cell and receive the bottom right ghost cell
    MPI_Sendrecv(&readingArray[1][1] , 1*sizeof(cell)/4, mpi_cell, ((coordinates[0] + dim_sizes[0] - 1) % dim_sizes[0])*(dim_sizes[1]) + ((coordinates[1] + dim_sizes[1] - 1) % dim_sizes[1]) , 0, &readingArray[r+2-1][c+2-1], 1*sizeof(cell)/4, mpi_cell, ((coordinates[0] + 1) % dim_sizes[0])*(dim_sizes[1]) + ((coordinates[1] + 1) % dim_sizes[1]), 0, MPI_COMM_WORLD, &status);
    //Send the top side ghost cells and receive on the bottom side
    MPI_Sendrecv(&readingArray[1][1], c*sizeof(cell)/4, mpi_cell, ((coordinates[0] + dim_sizes[0] - 1) % dim_sizes[0])*(dim_sizes[1]) + coordinates[1], 0, &readingArray[r+2-1][1] , c*sizeof(cell)/4, mpi_cell, ((coordinates[0] + 1) % dim_sizes[0])*(dim_sizes[1]) + coordinates[1], 0, MPI_COMM_WORLD, &status);
    //Send top right side ghost cell and receive the bottom left ghost cell
    MPI_Sendrecv(&readingArray[1][c], 1*sizeof(cell)/4, mpi_cell, ((coordinates[0] + dim_sizes[0] - 1) % dim_sizes[0])*(dim_sizes[1]) + ((coordinates[1] + 1) % dim_sizes[1]) , 0, &readingArray[r+1][0], 1*sizeof(cell)/4, mpi_cell, ((coordinates[0] + 1) % dim_sizes[0])*(dim_sizes[1]) + ((coordinates[1] + dim_sizes[1] - 1) % dim_sizes[1]) , 0, MPI_COMM_WORLD, &status);
    //Send the right side ghost cells and receive on the left side
    MPI_Sendrecv(sendRightSideArray, r*sizeof(cell)/4, mpi_cell, coordinates[0]*(dim_sizes[1]) + ((coordinates[1] + 1) % dim_sizes[1]) , 0, recvLeftSideArray, r*sizeof(cell)/4, mpi_cell, coordinates[0]*(dim_sizes[1]) + ((coordinates[1] + dim_sizes[1] - 1) % dim_sizes[1]), 0, MPI_COMM_WORLD, &status);
    //Send bottom right side ghost cell and receive the top left ghost cell
    MPI_Sendrecv(&readingArray[r][c], 1*sizeof(cell)/4, mpi_cell, ((coordinates[0] + 1) % dim_sizes[0])*(dim_sizes[1]) + ((coordinates[1] + 1) % dim_sizes[1]) , 0, &readingArray[0][0] , 1*sizeof(cell)/4, mpi_cell, ((coordinates[0] + dim_sizes[0] - 1) % dim_sizes[0])*(dim_sizes[1]) + ((coordinates[1] + dim_sizes[1] - 1) % dim_sizes[1]), 0, MPI_COMM_WORLD, &status);
    //Send the bottom side ghost cells and receive on the top side
    MPI_Sendrecv(&readingArray[r][1], c*sizeof(cell)/4, mpi_cell, ((coordinates[0] + 1) % dim_sizes[0])*(dim_sizes[1]) + coordinates[1] , 0, &readingArray[0][1] , c*sizeof(cell)/4, mpi_cell, ((coordinates[0] + dim_sizes[0] - 1) % dim_sizes[0])*(dim_sizes[1]) + coordinates[1] , 0, MPI_COMM_WORLD, &status);
    //Send bottom left side ghost cell and receive the top right ghost cell
    MPI_Sendrecv(&readingArray[r][1], 1*sizeof(cell)/4, mpi_cell, ((coordinates[0] + 1) % dim_sizes[0])*(dim_sizes[1]) + ((coordinates[1] + dim_sizes[1] - 1) % dim_sizes[1]) , 0, &readingArray[0][c+1] , 1*sizeof(cell)/4, mpi_cell, ((coordinates[0] + dim_sizes[0] - 1) % dim_sizes[0])*(dim_sizes[1]) + ((coordinates[1] + 1) % dim_sizes[1]) , 0, MPI_COMM_WORLD, &status);

    //Put the received right and left arrays on the 2D array (If you're wondering why I did with extra arrays, the reason is data in memory is 1 dimensional)
    for(int i = 0; i < r; i++)
    {
      readingArray[i+1][c+2-1] = recvRightSideArray[i];
      readingArray[i+1][0] = recvLeftSideArray[i];
    }

    //Update the old state of every cell with the new state
    for(int i = 0; i < r+2; i++)
      for(int j = 0; j < c+2; j++)
        readingArray[i][j].oldstate = readingArray[i][j].newstate;

    //Calculate the future state of every cell based on the old state
    for(int i = 1; i <= r; i++)
			for(int j = 1; j <= c; j++)
        cell_calc(readingArray, i, j);


    int writeoffset;
    //Similar to reading each process will write a certain amount at a certain offset
    for (int i = 0; i < r; i++)
    {
      writeoffset = offset + i*COLNUM*sizeof(cell) + counter*ROWNUM*COLNUM*sizeof(cell);
      MPI_File_set_view(myfile, writeoffset , mpi_cell, mpi_cell, "native", MPI_INFO_NULL);
      MPI_File_write_all(myfile,&readingArray[i+1][1],c*sizeof(cell)/4,mpi_cell, MPI_STATUS_IGNORE);
    }
  }

  MPI_File_close(&myfile);
  //End of timer
  finish = MPI_Wtime();

  if(my_rank==0){
    printf("Number of processes: %d Grid Dimensions (Row x Col): %d x %d Time taken: %.06f\n",p,dim_sizes[0],dim_sizes[1],finish-begin);
  }
  //Free arrays
  free(sendLeftSideArray);
  free(sendRightSideArray);
  free(recvLeftSideArray);
  free(recvRightSideArray);

  for(int i = 0;i < r+2; ++i)
    free(readingArray[i]);
  free(readingArray);
  MPI_Finalize();
  return 0;
}
