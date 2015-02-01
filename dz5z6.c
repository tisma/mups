/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 6. zadatak
 *******************************/

/**
 * 6. Sastaviti program koji raspolozive procese deli u dve grupe, prema parnosti ranga procesa unutar
 * komunikatora COMM_WORLD. Za podelu koristiti dva poziva  MPI_Group_incl. Svaki proces treba da
 * ispise svoj rang unutar MPI sveta, svoj rang unutar novog komunikatora i zbir kvadrata rangova
 * unutar novog komunikatora svih procesa obuhvacenih novim komunikatorom.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int i, j, k, rank, size;
	int comm_rank, comm_size;
	MPI_Group world_group, even_group, odd_group;
	MPI_Comm even_comm, odd_comm;

	int *even;
	int *odd;
	int num_even, num_odd, sum = 0, square;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	// broj parnih
	num_even = (size + 1) / 2;
	// broj neparnih
	num_odd = size - num_even;
	even = (int*)malloc(num_even * sizeof(int));
	odd = (int*)malloc(num_odd * sizeof(int));

	j = k = 0;
	for (i = 0; i < size; i++) {
		if (i % 2) {
			odd[j++] = i;
		} else {
			even[k++] = i;
		}
	}

	// dohvatanje world grupe
	MPI_Comm_group(MPI_COMM_WORLD, &world_group);

	if (rank % 2) {
		// ukljucujemo sve procese sa neparnim rangom u novu grupu
		MPI_Group_incl(world_group, num_odd, odd, &odd_group);
		// kreiramo komunikator za neparnu grupu
		MPI_Comm_create(MPI_COMM_WORLD, odd_group, &odd_comm);

		MPI_Comm_size(odd_comm, &comm_size);
		MPI_Comm_rank(odd_comm, &comm_rank);

		// racunamo kvadrat ranga komunikatora u grpui
		square = comm_rank * comm_rank;

		// svima saljemo i pritom sabiramo sve kvadrate rangova
		MPI_Allreduce(&square, &sum, 1, MPI_INT, MPI_SUM, odd_comm);

		printf("0%d:001:Rang procesa u MPI_COMM_WORLD komunikatoru: %d\n", rank, rank);
		printf("0%d:002:Rang procesa unutar novog komunikatora: %d\n", rank, comm_rank);
		printf("0%d:003:Zbir kvadrata rangova procesa unutar novog komunikatora: %d\n", rank, sum);

		MPI_Comm_free(&odd_comm);
		MPI_Group_free(&odd_group);
	} else {
		// ukljucujemo sve procese sa parnim rangom u novu grupu
		MPI_Group_incl(world_group, num_even, even, &even_group);
		// kreiramo komunikator za parnu grupu
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

	free(even);
	free(odd);

	MPI_Finalize();

	return EXIT_SUCCESS;
}
