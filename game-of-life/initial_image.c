#include "life.h"

//Create an initial image for the MPI program to read from
int main() {

  FILE *fp;

  cell **cellarea;
  cellarea = (cell **)calloc(ROWNUM, sizeof(cell *));

  for(int i = 0; i < ROWNUM; i++)
    cellarea[i] = (cell *)calloc(COLNUM, sizeof(cell));

  //Populate the cell grid with cells
  for(int i = 0; i < ROWNUM; i++)
    for(int j = 0; j < COLNUM; j++) {
      cellarea[i][j].r = i;
      cellarea[i][j].c = j;
    }

  //Set the desired cells to alive
  cellarea[7][3].newstate = TRUE;
  cellarea[8][4].newstate = TRUE;
  cellarea[9][4].newstate = TRUE;
  cellarea[9][3].newstate = TRUE;
  cellarea[9][2].newstate = TRUE;
  fp = fopen(FILENAME, "w");
  //write to file
  for (int i = 0; i < ROWNUM; i++) {
    fwrite(&cellarea[i][0],sizeof(cell), COLNUM, fp);
  }
  fclose(fp);
  
  //Free memory
  for(int i = 0; i < ROWNUM; i++)
    free(cellarea[i]);
  free(cellarea);

  return 0;
}
