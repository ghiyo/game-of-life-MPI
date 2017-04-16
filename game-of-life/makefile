all: bound_calc.c dim_calc.c mpi_cell.c MPI_gameoflife.c updategrid.c initial_image.c readserial.c
	mpicc bound_calc.c dim_calc.c mpi_cell.c MPI_gameoflife.c updategrid.c -lm -o run_mpi
	mpicc initial_image.c -o create_initial_image
	mpicc readserial_curses.c -lncurses -o read_file_curses
	mpicc readserial.c -o read_file
