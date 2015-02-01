/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 3. zadatak
 *******************************/

/**
 * 3. Sastaviti program koji ucitava i ispisuje elemente matrice bez obrade elemenata. Broj kolona odrediti
 * na osnovu broja procesa. Proces sa rangom 0 treba da ucita broj vrsta (najvise 100) i elemente
 * matrice, a zatim da posalje svakom od ostalih procesa po jednu kolonu matrice. Za prenos jedne
 * kolone koristiti samo jedan MPI poziv Send i njemu odgovarajuci Receive.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 32	// maksimalan broj procesa u sistemu
#define MAX_ROWS 100	// maksimalan broj vrsta matrice

int matrix[MAX_ROWS][N];

int main(int argc, char *argv[]) {

	int i, j, size, rank, m, msg = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		++msg;
		printf("00:00%d:Unesite broj vrsta matrice:\n", msg);
		scanf("%d", &m);
		if (m < 1 || m > MAX_ROWS) {
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < m; i++) {
			for (j = 0; j < size-1; j++) {
				++msg;
				printf("00:00%d:Unesite element matrice matrix[%d][%d]:\n", msg, i, j);
				scanf("%d", &matrix[i][j]);
			}
		}

		// deklaracija novog korisnickog tipa
		MPI_Datatype column_matrix;

		// definisanje novog korisnickog tipa
		MPI_Type_vector(size-1, 1, N, MPI_INT, &column_matrix);

		// uvodjenje u MPI svet novog korisnickog tipa
		MPI_Type_commit(&column_matrix);

		++msg;
		printf("\n00:00%d:Ispis matrice u master procesu\n\n", msg);
		for (i = 0; i < m; i++) {
			for (j = 0; j < size-1; j++) {
				printf("%d ", matrix[i][j]);
			}
			printf("\n");
		}

		printf("\n");

		MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (i = 0; i < size-1; i++)
			MPI_Send(&matrix[0][i], 1, column_matrix, i+1, 0, MPI_COMM_WORLD);

		MPI_Type_free(&column_matrix);
	}
	else {
		int i, m;
		int column[N];

		MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

		MPI_Recv(column, m, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		for (i = 0; i < m; i++)
			printf("%d0:00%d:matrix[%d][%d]=%d\n", rank, i+1, rank-1, i, column[i]);

	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
