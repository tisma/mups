/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 7. zadatak
 *******************************/

/**
 * 7. Izmeniti prethodni program tako da koristi jedan poziv MPI_Group_incl i jedan poziv MPI_Group_excl.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// makismalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int i, j, rank, size;
	int comm_rank, comm_size;
	MPI_Group world_group, even_group, odd_group;
	MPI_Comm even_comm, odd_comm;

	int *ranks;
	int n, square, sum = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	n = (size + 1) / 2;
	ranks = (int*)malloc(n * sizeof(int));

	j = 0;
	for (i = 0; i < size; i++) {
		if (i % 2 == 0) {
			ranks[j++] = i;
		}
	}

	MPI_Comm_group(MPI_COMM_WORLD, &world_group);

	if (rank % 2) {
		MPI_Group_excl(world_group, n, ranks, &odd_group);
		MPI_Comm_create(MPI_COMM_WORLD, odd_group, &odd_comm);

		MPI_Comm_size(odd_comm, &comm_size);
		MPI_Comm_rank(odd_comm, &comm_rank);

		square = comm_rank * comm_rank;

		MPI_Allreduce(&square, &sum, 1, MPI_INT, MPI_SUM, odd_comm);

		printf("0%d:001:Rang procesa u MPI_COMM_WORLD komunikatoru: %d\n", rank, rank);
		printf("0%d:002:Rang procesa unutar novog komunikatora: %d\n", rank, comm_rank);
		printf("0%d:003:Zbir kvadrata rangova procesa unutar novog komunikatora: %d\n", rank, sum);

		MPI_Comm_free(&odd_comm);
		MPI_Group_free(&odd_group);
	}
	else {
		MPI_Group_incl(world_group, n, ranks, &even_group);
		MPI_Comm_create(MPI_COMM_WORLD, even_group, &even_comm);

		MPI_Comm_size(even_comm, &comm_size);
		MPI_Comm_rank(even_comm, &comm_rank);

		square = comm_rank * comm_rank;

		MPI_Allreduce(&square, &sum, 1, MPI_INT, MPI_SUM, even_comm);

		printf("0%d:001:Rang procesa u MPI_COMM_WORLD komunikatoru: %d\n", rank, rank);
		printf("0%d:002:Rang procesa unutar novog komunikatora: %d\n", rank, comm_rank);
		printf("0%d:003:Zbir kvadrata rangova procesa unutar novog komunikatora: %d\n", rank, sum);

		MPI_Comm_free(&even_comm);
		MPI_Group_free(&even_group);
	}

	free(ranks);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
