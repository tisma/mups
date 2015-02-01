/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 8. zadatak
 *******************************/

/**
 * 8. Sastaviti program koji kvadrira sve elemente niza celih brojeva. Proces sa rangom 0 treba da obavlja
 * svu komunikaciju sa korisnikom. Za komunikaciju koristiti MPI pozive Scatter i Gather. Ako broj
 * elemenata niza nije celobrojni umnozak broja procesa u MPI svetu, prekinuti izvrsavanje.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int i, n, size, rank, slice;
	int *array, *buffer;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size < 1 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		printf("00:001:Unesite velicinu niza:\n");
		scanf("%d", &n);
		if (n % size) {
			printf("Greska! Broj elemenata mora biti deljiv sa brojem procesa u sistemu!\n");
			MPI_Abort(MPI_COMM_WORLD, size);
			exit(EXIT_FAILURE);
		}

		slice = n / size;
		MPI_Bcast(&slice, 1, MPI_INT, 0, MPI_COMM_WORLD);
		array = (int*) malloc(n*sizeof(int));
		buffer = (int*) malloc(slice*sizeof(int));

		for (i = 0; i < n; i++) {
			printf("00:00%d:Unesite %d. element niza:\n", i+2, i);
			scanf("%d", &array[i]);
		}

		MPI_Scatter(array, slice, MPI_INT, buffer, slice, MPI_INT, 0, MPI_COMM_WORLD);

		for (i = 0; i < slice; i++)
			buffer[i] = buffer[i] * buffer[i];

		MPI_Gather(buffer, slice, MPI_INT, array, slice, MPI_INT, 0, MPI_COMM_WORLD);

		printf("00:00%d:Kvadrirani niz\n", n+2);
		for (i = 0; i < n; i++)
			printf("%d ", array[i]);
		printf("\n");

		free(buffer);
		free(array);
	}
	else {
		int slice;

		MPI_Bcast(&slice, 1, MPI_INT, 0, MPI_COMM_WORLD);

		buffer = (int*) malloc(slice * sizeof(int));

		MPI_Scatter(buffer, slice, MPI_INT, buffer, slice, MPI_INT, 0, MPI_COMM_WORLD);

		for (i = 0; i < slice; i++) {
			buffer[i] = buffer[i]*buffer[i];
		}

		MPI_Gather(buffer, slice, MPI_INT, buffer, slice, MPI_INT, 0, MPI_COMM_WORLD);

		free(buffer);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
