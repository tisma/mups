/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 2. zadatak
 *******************************/

/**
 * 2. Prosiriti prethodni program tako da ispise rang svog procesa i ukupan broj procesa u MPI svetu.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int size, rank;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size < 1 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	printf("0%d:001:Hello world!\n", rank);

	printf("0%d:002:Pozdrav od procesa sa rangom %d od ukupno %d procesa u MPI svetu!\n", rank, rank, size);

	MPI_Finalize();

	return EXIT_SUCCESS;
}
