

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#include <curses.h>

#define ROWNUM 20
#define COLNUM 60
#define CELL_CHAR '#'
#define EMPTY ' '
#define TIME_OUT 150
#define FILENAME "images"
#define IMAGENUM 1000

typedef struct _cell {
	int c;
	int r;
	int oldstate;
	int newstate;
} cell;

int* dim_calc(int);
void bound_calc(int *, int, int);
void Build_cell_type(int*, int*, int*, int*, MPI_Datatype*);
void cell_calc(cell **, int , int );
