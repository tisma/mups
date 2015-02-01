/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 8. zadatak
 *******************************/

/**
 * 3. Izmeniti prethodni program tako da koristi MPI_Comm_split.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int rank, size, split;
	int comm_rank, comm_size;
	MPI_Comm comm_split;

	int square, sum = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	// kako se vrsi podela
	split = rank % 2;

	MPI_Comm_split(MPI_COMM_WORLD, split, rank, &comm_split);

	MPI_Comm_size(comm_split, &comm_size);
	MPI_Comm_rank(comm_split, &comm_rank);

	square = comm_rank * comm_rank;

	MPI_Allreduce(&square, &sum, 1, MPI_INT, MPI_SUM, comm_split);

	printf("0%d:001:Rang procesa u MPI_COMM_WORLD komunikatoru: %d\n", rank, rank);
	printf("0%d:002:Rang procesa unutar novog komunikatora: %d\n", rank, comm_rank);
	printf("0%d:003:Zbir kvadrata rangova procesa unutar novog komunikatora: %d\n", rank, sum);

	MPI_Comm_free(&comm_split);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
