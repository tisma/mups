/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 3 - 9. zadatak
 *******************************/

/**
 * 9. Sastaviti program koji racuna treci koren svakog elementa niza realnih brojeva. Proces sa rangom 0
 * (upravljac) treba da obavlja svu komunikaciju sa korisnikom i raspodelu poslova ostalim procesima
 * (radnicima). Upravljac treba da salje neobradjene elemente niza slobodnim radnicima i da prihvata
 * rezultate od radnika, sve dok ima neobradjenih elemenata. Radnici treba da prihvataju po jedan
 * element niza, obradjuju i vracaju nazad sve dok ih upravljac ne obavesti da vise nema posla.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 6		// maksimalan broj procesa u sistemu
#define FINISH 100

int main(int argc, char *argv[]) {

	int i, n, size, rank, msg = 0;
	double *array;
	int sent = 0, received = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		printf("00:001:Unesite velicinu niza:\n");
		scanf("%d", &n);
		array = (double *) malloc(n*sizeof(double));

		for (i = 0; i < n; i++) {
			++msg;
			printf("00:00%d:Unesite %d. element niza:\n", msg, i);
			scanf("%lf", &array[i]);
		}

		++msg;
		printf("00:00%d:Originalni niz:\n", msg);
		for (i = 0; i < n; i++)
			printf("%lf ", array[i]);

		printf("\n");

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (n <= size-1) {
			for (i = 0; i < n; i++)
				MPI_Send(&array[i], 1, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);

			for (i = n; i < size; i++)
				MPI_Send(&i, 1, MPI_INT, i, FINISH, MPI_COMM_WORLD);

			++msg;
			printf("00:00%d:Korenovani niz:\n", msg);
			for (i = 0; i < n; i++) {
				MPI_Recv(&array[i], 1, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("%lf ", array[i]);
			}
		} else {
			int dest = 1;
			while (sent <= n) {
				MPI_Send(&array[sent], 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
				sent++;
				dest = (dest + 1) % size;
				if (dest == 0)
					dest = 1;
			}
			while(sent > 0) {
				MPI_Recv(&array[sent], 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				sent--;
			}
			for (i = 0; i < size; i++)
				MPI_Send(&i, 1, MPI_INT, i+1, FINISH, MPI_COMM_WORLD);

			++msg;
			printf("00:00%d:Korenovani niz:\n", msg);
			for (i = 0; i < n; i++)
				printf("%lf ", array[i]);
			printf("\n");
		}

		free(array);
	}
	else {
		double root;
		MPI_Status status;

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (n <= size-1) {
			MPI_Recv(&root, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if (status.MPI_TAG != FINISH) {
				root = pow(root, 1.0/3.0);
				MPI_Send(&root, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
			}
		} else {
			while (1) {
				MPI_Recv(&root, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				if (status.MPI_TAG != FINISH) {
					root = pow(root, 1.0/3.0);
					MPI_Send(&root, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
				} else break;
			}
		}
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
