/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 4. zadatak
 *******************************/

/**
 * 4. Sastaviti program koji za diplomirane studente na istom smeru odredjuje prosecne ocene. Podaci o
 * ocenama studenata su za svakog studenta smesteni u jednodimenzionalnom nizu. Posto student u toku
 * jedne skolske godine, u zavisnosti od smera, moze imati maksimalno 20 ocena, niz ima fiksno 80
 * elemenata. Ocene iz prve godine ce se nalaziti u prvih 20 elemenata niza, ocene iz druge u drugih 20,
 * itd. Ukoliko je student u toku skolske godine dobio manje od 20 ocena po godini, ostatak elemenata
 * ima vrednost 0. Za svakog od studenata na istom smeru je broj ocena po godini isti. Proces sa rangom
 * 0 treba da ucita potrebne podatke o ocenama studenata i podatke o broju ocena na svakoj od godina
 * studija, raspodeli posao ostalim procesima i ispise prosecne ocene. Proces sa rangom 0 treba da salje
 * ostalim procesima samo one elemente niza ocena koji predstavljaju dobijene ocene. Za prenos ocena
 * jednog studenta koristiti samo jedan MPI poziv Send i njemu odgovarajuci Receive.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define N 6					// maksimalan broj procesa u sistemu
#define SIZE 80				// velicina niza za ocene studenta
#define YEARS 4				// broj godina na fakultetu
#define MAX_MARKS 20		// maksimalan broj ocena po godini
#define MAX_STUDENTS 100	// maksimalan broj studenata
#define FINISH 1000			// tag koji signalizira procesu da zavrsi

MPI_Datatype indexed;		// indeksirani tip podataka za slanje ocena

int main(int argc, char *argv[]) {

	int i, j, k, rank, size, work, numOfStudents, numOfMarks, offset, dest, msg = 0;
	int totalMarks = 0;
	int counter = 0;
	float average;
	int diary[MAX_STUDENTS][SIZE];	// ocene za 4 godine za jednog studenta
	int blocklenghts[YEARS];
	int displacements[YEARS];

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	if (rank == 0) {
		++msg;
		printf("00:00%d:Unesite broj studenata:\n", msg);
		scanf("%d", &numOfStudents);
		if (numOfStudents < 0 || numOfStudents > MAX_STUDENTS) {
			printf("Unesite pozitivan broj manji od 100\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}

		++msg;
		printf("00:00%d:Unesite broj ocena po godinama:\n", msg);
		for (i = 0; i < YEARS; i++) {
			++msg;
			printf("00:00%d:Broj ocena za %d. godinu:\n", msg, i+1);
			scanf("%d", &numOfMarks);
			totalMarks += numOfMarks;
			blocklenghts[i] = numOfMarks;
			displacements[i] = i * MAX_MARKS;
		}

		for (i = 0; i < numOfStudents; i++) {
			for (j = 0; j < YEARS; j++) {
				++msg;
				printf("00:00%d:Unesite ocene za %d. studenta, za %d. godinu:\n", msg, i+1, j+1);
				offset = j * MAX_MARKS;
				for (k = 0; k < blocklenghts[j]; k++) {
					++msg;
					printf("00:00%d:Unesite %d. ocenu:\n", msg, k+1);
					scanf("%d", &diary[i][offset++]);
				}
			}
		}

		for (i = 0; i < numOfStudents; i++) {
			printf("\nOcene %d. studenta:\n", i+1);
			for (j = 0; j < YEARS; j++) {
				offset = displacements[j];
				printf("\n%d. godina:\n", j+1);
				for (k = 0; k < MAX_MARKS; k++)
					printf("%d ", diary[i][offset++]);
				printf("\n");
			}
			printf("\n");
		}

		MPI_Type_indexed(YEARS, blocklenghts, displacements, MPI_INT, &indexed);

		MPI_Type_commit(&indexed);

		MPI_Bcast(&numOfStudents, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (numOfStudents <= size-1) {			// ako je manje studenta nego procesa radnika
			for (i = 0; i < numOfStudents; i++)	// salji samo onoliko koliko ima studenata
				MPI_Send(&totalMarks, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);

			for (i = numOfStudents; i < size; i++)	// ostalima kazi da zavrse
				MPI_Send(&totalMarks, 1, MPI_INT, i, FINISH, MPI_COMM_WORLD);

			// salji podatke za obradu
			for (i = 0; i < numOfStudents; i++) {
				dest = (i + 1) % size;
				counter++;
				if (dest == 0)
					dest = 1;
				MPI_Send(diary[i], 1, indexed, dest, 0, MPI_COMM_WORLD);
			}

			for (i = 0; i < numOfStudents; i++) {
				MPI_Recv(&average, 1, MPI_FLOAT, i+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				++msg;
				printf("00:00%d:Prosek %d. studenta je: %5.2lf\n", msg, i, average);
			}
		}
		else {	// ako ima vise studenata nego procesa radnika
			counter = 0;
			work = numOfStudents;
			while (work > 0) {
//				MPI_Send(&numOfMarks, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				dest = (counter + 1) % size;
				if (dest == 0)
					dest = 1;
				MPI_Send(diary[counter], 1, indexed, dest, 0, MPI_COMM_WORLD);
				work--;
				counter++;
			}
			// primi rezultate
			for (i = 0; i < numOfStudents; i++) {
				MPI_Recv(&average, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				++msg;
				printf("00:00%d:Prosek %d. studenta je: %5.2lf\n", msg, i, average);
			}
			for (i = 0; i < size-1; i++)
				MPI_Send(&numOfMarks, 1, MPI_INT, i+1, FINISH, MPI_COMM_WORLD);
		}

	}
	else {
		int i, numOfMarks, numOfStudents, size;
		float average = 0.0;
		int marks[SIZE];

		MPI_Status status;

		MPI_Comm_size(MPI_COMM_WORLD, &size);

		MPI_Bcast(&numOfStudents, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (numOfStudents <= size-1) {
			MPI_Recv(&numOfMarks, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			if (status.MPI_TAG != FINISH) {
				MPI_Recv(marks, numOfMarks, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				for (i = 0; i < numOfMarks; i++)
					average += marks[i];
				average /= numOfMarks;

				MPI_Send(&average, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
			}
		}
		else {
//			printf("OVo radim %d\n", numOfStudents);
			while (1) {
				MPI_Recv(&numOfMarks, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				if (status.MPI_TAG == FINISH)
					break;
				MPI_Recv(marks, numOfMarks, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				if (status.MPI_TAG == FINISH)
					break;
				for (i = 0; i < numOfMarks; i++)
					average += marks[i];
				average /= numOfMarks;
				MPI_Send(&average, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
			}
		}
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
