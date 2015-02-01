/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 7. zadatak
 *******************************/

/**
 * 7. Sastaviti program koji odredjuje vrednost broja PI. Proces sa rangom 0 treba da obavesti ostale procese
 * o broju tacaka koje treba da obrade, prikupi podatke od ostalih procesa i treba da ucestvuje u
 * racunanju. Za komunikaciju koristiti samo MPI pozive Bcast i Reduce.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6		// maksimalan broj procesa u sistemu

int main(int argc, char *argv[]) {

	int i, rank, size;
	double pi, x, y;
	int number, in_circle = 0;
	int all = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 1 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		printf("00:001:Unesite broj tacaka za generisanje:\n");
		scanf("%d", &number);
		MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);

		srand(time(NULL));
		for (i = 0; i < number; i++) {
			x = (double)rand() / RAND_MAX;
			y = (double)rand() / RAND_MAX;
			if ((x*x + y*y) <= 1)
				in_circle++;
		}
		MPI_Reduce(&in_circle, &all, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

		pi = (4.0 * all) / (double)(size * number);
		printf("00:002:PI = %lf\n", pi);
	}
	else {
		MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);
		srand(time(NULL));
		for (i = 0; i < number; i++) {
			x = (double)rand() / RAND_MAX;
			y = (double)rand() / RAND_MAX;
			if ((x*x + y*y) <= 1)
				in_circle++;
		}
		MPI_Reduce(&in_circle, &all, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
