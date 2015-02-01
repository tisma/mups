/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 1. zadatak
 *******************************/

/**
 * 1. Sastaviti program koji ispisuje Hello World!. Od MPI poziva, program treba da sadrzi samo Init i
 * Finalize.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int rank, size;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size < 1 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	printf("0%d:001:Hello world!\n", rank);

	MPI_Finalize();

	return EXIT_SUCCESS;
}
