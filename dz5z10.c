/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 10. zadatak
 *******************************/

/**
 * 5. Sastaviti program koji ispisuje najveci element u svakoj vrsti matrice i najmanji element u svakoj
 * koloni matrice. Svaku pretragu treba da obavlja posebna grupa procesa. Posao podeliti tako da svaki
 * proces dobije da obradi po jednu vrstu ili kolonu. Ako je broj procesa manji od tacno potrebnog,
 * prekinuti izvrsavanje. Ako je broj procesa veci od tacno potrebnog, suvisne procese rasporediti u
 * posebnu grupu i svakom dati da ispise poruku da je bez dodeljenog posla. [2,N]
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6		// maksimalan broj procesa u sistemu
#define MAX_SIZE 1000		// maksimalan broj vrsta ili kolona matrice
#define WORKERS 10
#define NON_WORKERS 20

int matrix[MAX_SIZE][MAX_SIZE];

int main(int argc, char *argv[]) {

	int i, j, k, m, n, rank, size, msg = 0;
	MPI_Group world_group, row_group, column_group, jobless_group;
	MPI_Comm row_comm, column_comm, jobless_comm;

	int rgcount = 1, cgcount = 1, jblcount = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		++msg;
		printf("00:00%d:Unesite broj vrsta matrice (max 1000):\n", msg);
		scanf("%d", &m);
		++msg;
		printf("00:00%d:Unestite broj kolona matrice (max 1000):\n", msg);
		scanf("%d", &n);
		if (m + n > size) {
			printf("Greska! Broj procesa je manji od potrebnog!\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < m; i++) {
			for (j = 0; j < n; j++) {
				++msg;
				printf("00:00%d:Unestie element matrice matrix[%d][%d]:\n", msg, i, j);
				scanf("%d", &matrix[i][j]);
			}
		}
	}

	MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	for (i = 1; i < size; i++) {
		if (i < m) {
			rgcount++;
		}
		else if (i < m + n) {
			cgcount++;
		}
		else jblcount++;
	}

	int *row = (int*)malloc(rgcount * sizeof(int));
	int *col = (int*)malloc(cgcount * sizeof(int));
	int *jblss = (int*)malloc(jblcount * sizeof(int));

	row[0] = col[0] = 0;
	j = 1; k = 1;
	for (i = 1; i < size; i++) {
		if (i < m) {
			row[i % size] = i;
		}
		else if (i < m + n) {
			col[j % size] = i;
			j++;
		}
		else {
			jblss[k % size] = i;
			k++;
		}
	}

	// dohvatanje world grupe
	MPI_Comm_group(MPI_COMM_WORLD, &world_group);

	// kreiranje grupe i komunikatora za grupu vrsta
	MPI_Group_incl(world_group, rgcount, row, &row_group);
	MPI_Comm_create(MPI_COMM_WORLD, row_group, &row_comm);

	// kreiranje grupe i komunikatora za grupu kolona
	MPI_Group_incl(world_group, cgcount, col, &column_group);
	MPI_Comm_create(MPI_COMM_WORLD, column_group, &column_comm);

	// kreiranje grupe i komunikatora za grupu besposlenih
	MPI_Group_incl(world_group, jblcount, jblss, &jobless_group);
	MPI_Comm_create(MPI_COMM_WORLD, jobless_group, &jobless_comm);

	// izvedeni tip za vrstu matrice
	MPI_Datatype row_matrix;
	MPI_Type_contiguous(n, MPI_INT, &row_matrix);
	MPI_Type_commit(&row_matrix);

	// izvedeni tip za kolonu matrice
	MPI_Datatype column_matrix;
	MPI_Type_vector(m, 1, n, MPI_INT, &column_matrix);
	MPI_Type_commit(&column_matrix);

	if (rank == 0) {
		for (i = 1; i < m; i++)
			MPI_Send(&matrix[i], 1, row_matrix, i, 0, row_comm);
		for (i = 0; i < n; i++)
			MPI_Send(&matrix[0][i], 1, column_matrix, i + 1, 0, column_comm);
		int max = matrix[0][0];
		for (i = 1; i < m; i++) {
			if (matrix[0][i] > max)
				max = matrix[0][i];
		}
		++msg;
		printf("00:00%d:Primljena vrsta:\n", msg);
		for (i = 0; i < m; i++)
			printf("%d ", matrix[0][i]);
		printf("\n");
		++msg;
		printf("00:00%d:Najveci u vrsti je: %d\n", msg, max);
	}
	else if (rank < m) {
		int row[n];
		MPI_Recv(row, 1, row_matrix, 0, 0, row_comm, MPI_STATUS_IGNORE);
		int max = row[0];
		for (i = 1; i < n; i++) {
			if (row[i] > max)
				max = row[i];
		}
		printf("0%d:001:Primljena vrsta:\n", rank);
		for (i = 0; i < n; i++)
			printf("%d ", row[i]);
		printf("\n");
		printf("0%d:002:Najveci u vrsti je: %d\n", rank, max);
	}
	else if (rank < m + n) {
		int column[m];
		MPI_Recv(column, 1, column_matrix, 0, 0, column_comm, MPI_STATUS_IGNORE);
		int min = column[0];
		for (i = 1; i < m; i++) {
			if (column[i] < min)
				min = column[i];
		}
		printf("0%d:001:Primljena kolona:\n", rank);
		for (i = 0; i < m; i++)
			printf("%d ", column[i]);
		printf("\n");
		printf("0%d:002:Najmanji u koloni je: %d\n", rank, min);
	}
	else if (rank >= m + n) {
		printf("0%d:001:Nema posla za mene\n", rank);
	}

//	MPI_Comm_free(&row_comm);
//	MPI_Comm_free(&column_comm);
//	MPI_Comm_free(&jobless_comm);

	MPI_Group_free(&row_group);
	MPI_Group_free(&column_group);
	MPI_Group_free(&jobless_group);

	free(row);
	free(col);
	free(jblss);

	MPI_Type_free(&row_matrix);
	MPI_Type_free(&column_matrix);

	MPI_Finalize();

	return EXIT_SUCCESS;
}
