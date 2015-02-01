/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 1. zadatak
 *******************************/

/**
 * 1. Sastaviti program koji ucitava i ispisuje elemente matrice bez obrade elemenata. Broj vrsta odrediti na
 * osnovu broja procesa. Proces sa rangom 0 treba da ucita broj kolona (najviše 100) i elemente matrice,
 * a zatim da posalje svakom od ostalih procesa po jednu vrstu matrice. Za prenos jedne vrste koristiti
 * samo jedan MPI poziv Send i njemu odgovarajuci Receive.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 32	// maksimalan broj procesa u sistemu
#define MAX_COLUMNS 100	// maksimalan broj kolona matrice

int matrix[N][MAX_COLUMNS];

int main(int argc, char *argv[]) {

	int i, j, rank, size, n, msg = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		++msg;
		printf("00:00%d:Unesite broj kolona matrice:\n", msg);
		scanf("%d", &n);
		if (n < 1 || n > MAX_COLUMNS) {
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < size-1; i++) {
			for (j = 0; j < n; j++) {
				++msg;
				printf("00:00%d:Unesite element matrice matrix[%d][%d]:\n", msg, i, j);
				scanf("%d", &matrix[i][j]);
			}
		}

		// deklaracija novog korisnickog tipa
		MPI_Datatype row_matrix;

		// definisanje novog korisnickog tipa
		MPI_Type_contiguous(n, MPI_INT, &row_matrix);

		// uvodjenje u MPI svet novog korisnickog tipa
		MPI_Type_commit(&row_matrix);

		++msg;
		printf("\n00:00%d:Ispis matrice u master procesu\n\n", msg);
		for (i = 0; i < size-1; i++) {
			for (j = 0; j < n; j++) {
				printf("%d ", matrix[i][j]);
			}
			printf("\n");
		}

		printf("\n");

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (i = 0; i < size-1; i++)
			MPI_Send(matrix[i], 1, row_matrix, i+1, 0, MPI_COMM_WORLD);

		MPI_Type_free(&row_matrix);
	}
	else {
		int n;
		int row[MAX_COLUMNS];

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		MPI_Recv(row, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		for (i = 0; i < n; i++)
			printf("%d0:00%d:matrix[%d][%d]=%d\n", rank, i+1, rank-1, i, row[i]);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
