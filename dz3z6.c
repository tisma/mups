/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 6. zadatak
 *******************************/

/**
 * 6. Sastaviti program koji određuje vrednost broja PI. Proces sa rangom 0 treba da obavesti ostale procese
 * o broju tacaka koje treba da obrade, prikupi podatke od ostalih procesa i ne treba da ucestvuje u
 * racunanju. Za komunikaciju koristiti samo MPI pozive Send i Receive.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int i, rank, size, number;
	double pi, x, y;
	int in_circle = 0;
	int all = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		printf("00:001:Unesite broj tacaka za generisanje:\n");
		scanf("%d", &number);
		for (i = 1; i < size; i++)
			MPI_Send(&number, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

		for (i = 1; i < size; i++) {
			MPI_Recv(&in_circle, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			all += in_circle;
		}
		pi = (4.0 * all) / (double) ((size - 1) * number);
		printf("00:002:PI = %lf\n", pi);
	}
	else {
		MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		srand(time(NULL));
		for (i = 0; i < number; i++) {
			x = (double)rand() / RAND_MAX;
			y = (double)rand() / RAND_MAX;
			if ((x*x + y*y) <= 1)
				in_circle++;
		}
		MPI_Send(&in_circle, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
