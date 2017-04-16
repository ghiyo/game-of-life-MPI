#include "life.h"
//Same routine as readserial_curses but with normal print routine
int main() {

  FILE *fp;
  cell *buf;
  buf = (cell *)calloc(ROWNUM*COLNUM, sizeof(cell));

  fp = fopen(FILENAME, "r");

  for (int counter = 0; counter < IMAGENUM; counter++) {
    fseek(fp, SEEK_SET, counter*ROWNUM*COLNUM);
    fread(buf, sizeof(cell), ROWNUM*COLNUM, fp);
    for (int i = 0; i < ROWNUM*COLNUM; i++) {
      if (buf[i].newstate == true)
        printf("%c",CELL_CHAR);
      else
        printf("%c",EMPTY);
      if (i%COLNUM==0)
        printf("\n");
    }
  }
  fclose(fp);
  free(buf);

  return 0;
}
