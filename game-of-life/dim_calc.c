#include "life.h"
//calculates the dimensions of the cartesian grid for the processes
int * dim_calc(int p) {

  int i=2;
  static int dim[2];

  dim[0] = 1;
  dim[1] = p;
  //Find all divisors of p and pick the two that have the least difference. e.g 30 has divisors 2*15 3*10 5*6 clearly it makes sense to have 5 and 6 as our dimensions
  while(i <= sqrt(p)) {
    if (p%i == 0) {
      if (p/i - i < dim[1] - dim[0]) {
        dim[0] = i;
        dim[1] = p/i;
      }
    }
    i++;
  }
	return dim;
}
