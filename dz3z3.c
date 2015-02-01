/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 3. zadatak
 *******************************/

/**
 * 3. Sastaviti program koji ucitava broj u procesu sa rangom 0, pa taj broj salje procesu sa rangom 1, koji
 * ispisuje njegovu neizmenjenu vrednost. Koristiti MPI pozive Send i Receive.
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

	if (size != 2) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		printf("00:001:Unesite jedan ceo broj:\n");
		scanf("%d", &number);
		MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	}
	else {
		MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("01:001:Primljeni broj je: %d\n", number);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
