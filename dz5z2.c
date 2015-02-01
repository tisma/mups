/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 2. zadatak
 *******************************/

/**
 * 2. Prosiriti prethodni program tako da radi sa bilo kojim brojem redova i kolona. Proces sa rangom 0
 * treba da ucita dimenzije i elemente matrice, a zatim da posalje svakom od ostalih procesa sto je
 * moguce slicniji broj vrsta matrice.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 32	// maksimalan broj procesa u sistemu
#define MAX_ROWS 1000		// maksimalan broj vrsta matrice
#define MAX_COLUMNS 1000	// maksimalan broj kolona matrice
#define FINISH 100

int matrix[MAX_ROWS][MAX_COLUMNS];

int main(int argc, char *argv[]) {

	int i, j, rank, size, slice, m, n, msg = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// deklaracija novog korisnickog tipa
	MPI_Datatype row_matrix;

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		++msg;
		printf("00:00%d:Unesite broj vrsta matrice (max 1000):\n", msg);
		scanf("%d", &m);
		++msg;
		printf("00:00%d:Unesite broj kolona matrice (max 1000):\n", msg);
		scanf("%d", &n);
		if (m < 0 || n < 0 || m > MAX_ROWS || n > MAX_COLUMNS) {
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < m; i++) {
			for (j = 0; j < n; j++) {
				++msg;
				printf("00:00%d:Unesite element matrice matrix[%d][%d]:\n", msg, i, j);
				scanf("%d", &matrix[i][j]);
			}
		}

		++msg;
		printf("\n00:00%d:Ispis matrice u master procesu\n\n", msg);
		for (i = 0; i < m; i++) {
			for (j = 0; j < n; j++) {
				printf("%d ", matrix[i][j]);
			}
			printf("\n");
		}
		printf("\n");

		// definisanje novog korisnickog tipa
		MPI_Type_contiguous(n, MPI_INT, &row_matrix);

		// uvodjenje u MPI svet novog korisnickog tipa
		MPI_Type_commit(&row_matrix);

		slice = m / (size - 1);
		MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&slice, 1, MPI_INT, 0, MPI_COMM_WORLD);

		j = 1;
		for(i = 0; i < m; i++) {
			MPI_Send(matrix[i], 1, row_matrix, j, 0, MPI_COMM_WORLD);
			j = (j+1) % size;
			if (j == 0)
				j = 1;
		}

	}
	else {
		int m, n;

		MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&slice, 1, MPI_INT, 0, MPI_COMM_WORLD);

		int row[n];

		while (slice > 0) {
			MPI_Recv(row, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (i = 0; i < n; i++)
				printf("%d0:00%d:matrix[%d][%d]=%d\n", rank, i+1, rank-1, i, row[i]);
			slice--;
		}
	}

	MPI_Type_free(&row_matrix);

	MPI_Finalize();

	return EXIT_SUCCESS;
}
