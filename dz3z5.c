/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 5. zadatak
 *******************************/

/**
 * 5. Sastaviti program koji ucitava broj u procesu sa rangom 0, a zatim taj broj salje svim procesima. Svaki
 * od procesa treba da ispise neizmenjenu vrednost poslatog broja. Koristiti MPI poziv Bcast.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int size, rank, number;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size < 1 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		printf("00:000:Unesite jedan ceo broj:\n");
		scanf("%d", &number);
		MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
	else {
		MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);
		printf("0%d:001:Primljen broj: %d\n", rank, number);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
