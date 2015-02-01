/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 4. zadatak
 *******************************/

/**
 * 4. Sastaviti program koji ucitava broj u procesu sa rangom 0, pa taj broj salje ostalim procesima, koji
 * ispisuju njegovu neizmenjenu vrednost. Koristiti MPI pozive Send i Receive.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int i, rank, size, number;

	// inicijalizacija MPI programa
	MPI_Init(&argc, &argv);

	// dohvatamo rank procesa
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// dohvatamo velicinu MPI sveta (broj procesa)
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	// ako je moj rang nula
	// ucitavam broj sa ulaza i saljem ostalima
	if (rank == 0) {
		printf("00:001:Unesite jedan broj:\n");
		scanf("%d", &number);
		for (i = 1; i < size; i++)
			MPI_Send(&number, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
	// u suprotnom primam broj i
	// ispisujem ga na standardnom izlazu
	else {
		MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("0%d:001:Primljen broj: %d\n", rank, number);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
