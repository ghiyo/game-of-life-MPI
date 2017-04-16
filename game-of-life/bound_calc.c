#include "life.h"

//calculate the boundary of each process' 2D subarray
void bound_calc(int * dim, int rows, int cols) {

  static int size[2];
  static int start[2];
  static int end[2];

  int previous_r = 0;
  int previous_c = 0;

  //Variables for available rows and columns left
  int leftover_rows = rows;
  int leftover_cols = cols;

  MPI_Request request;
  //Remainder rows and columns i.e if the row of the entire window is 11 and we have 3 processes then the most reasonable way to spread the work evenly is to use a remainder. In this case the number of rows for each process would be 4 4 3 where out remainder was 2.
  int r_rows = rows%dim[0];
  int r_cols = cols%dim[1];
  for(int i = 0; i < dim[0]; i++)
  {
    //if the number of left over rows is less than the qoutient then the row size will equal the left over rows
    if (rows/dim[0] > leftover_rows)
      size[0] = leftover_rows;
    else {
      size[0] = rows/dim[0];
      //If there is a remainder for rows then add one to the size of rows and subtract 1 from the remainder cells
      if (r_rows != 0) {
        size[0]++;
        r_rows--;
      }
    }
    //subtract the size of rows selected from the left over rows for the next process
    leftover_rows -= size[0];
    //Determine the start row for the same process that we are finding the bounds for
    start[0] = previous_r;
    //Determine the end row
    end[0] = previous_r + size[0];
    previous_r += size[0];

    //Same routine for columns as rows
    for (int j = 0; j < dim[1]; j++)
    {
      if (cols/dim[1] > leftover_cols)
        size[1] = leftover_cols;
      else {
        size[1] = cols/dim[1];
        if(r_cols != 0) {
          size[1]++;
          r_cols--;
        }
      }
      start[1] = previous_c;
      end[1] = previous_c + size[1];
      previous_c += size[1];
      //Send the boundary information to the correct process
      MPI_Isend(&size, 2, MPI_INT, i*dim[1] + j, 0, MPI_COMM_WORLD, &request);
      MPI_Isend(&start, 2, MPI_INT, i*dim[1] + j, 1, MPI_COMM_WORLD, &request);
      MPI_Isend(&end, 2, MPI_INT, i*dim[1] + j, 2, MPI_COMM_WORLD, &request);
    }
    //Repeat everything for the next process
    previous_c = 0;
    leftover_cols = cols;
    r_cols = cols % dim[1];
  }
}
