#include "life.h"
//Calculates the future state of each cell based on the current state
void cell_calc(cell **grid, int i, int j) {

	int neighbours;
	int newstate;
	//Calculate the number of live processes around the cell
	neighbours = 	grid[(i - 1)][j].oldstate +
					grid[(i - 1)][(j - 1)].oldstate +
					grid[(i - 1)][(j + 1)].oldstate +
					grid[(i + 1)][j].oldstate +
					grid[(i + 1)][(j - 1)].oldstate +
					grid[(i + 1)][(j + 1)].oldstate +
					grid[i][(j - 1)].oldstate +
					grid[i][(j + 1)].oldstate;
	newstate = FALSE;
	//Update the cell's newstate based on the neighbours
	if(grid[i][j].oldstate == TRUE && (neighbours == 2 || neighbours == 3))
		newstate = TRUE;
	else
		if(grid[i][j].oldstate == FALSE && neighbours == 3)
			newstate = TRUE;
	grid[i][j].newstate = newstate;
}
