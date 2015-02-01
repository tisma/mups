/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 9. zadatak
 *******************************/

/**
 * 9. Sastaviti program koji racuna vrednost broja PI po grupama od po priblizno K clanova, gde je K manje
 * od broja procesa. Procesi treba da budu sto ravnomernije rasporedjeni po novim grupama i
 * komunikatorima. Svaka grupa treba da izracuna vrednost broja PI. Proces sa rangom 0 u svakoj novoj
 * grupi treba da ispise dobijenu vrednost i posalje je procesu sa rangom 0 u MPI svetu, koji treba da
 * usrednji dobijene vrednosti i ispise konacni rezultat. [2,N]
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 6		// maksimalan broj procesa u sistemu
#define K 3		// broj grupa procesa
#define NUM_OF_POINTS 100000	// broj tacaka za generisanje

double calculatePi() {
	int i, in_circle = 0;
	double x, y, pi;
	srand(time(0));
	for (i = 0; i < NUM_OF_POINTS; i++) {
		x = (double) rand() / RAND_MAX;
		y = (double) rand() / RAND_MAX;
		if ((x*x + y*y) <= 1.0)
			in_circle++;
	}
	pi = 4.0 * (double)in_circle / NUM_OF_POINTS;
	return pi;
}

int main(int argc, char *argv[]) {

	int i, rank, size, rankColor, groupRank, msg = 0;
	double pi, sumPi, localPi, localPi2;
	MPI_Comm groupComm;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	rankColor = rank % K;

	// delimo u grupe prema
	MPI_Comm_split(MPI_COMM_WORLD, rankColor, rank, &groupComm);

	// grupni rank
	MPI_Comm_rank(groupComm, &groupRank);

	srand(time(0));
	localPi = calculatePi();

	if (rank == 0) {
		for (i = 1; i < K; i++) {
			MPI_Recv(&sumPi, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			pi += sumPi;
		}
		++msg;
		printf("00:00%d:Priblizna vrednost broja Pi je: %lf\n", msg, pi/(K-1));
	}

	// sumiramo sve lokalne vrednosti iz grupe
	MPI_Reduce(&localPi, &localPi2, 1, MPI_DOUBLE, MPI_SUM, 0, groupComm);

	if (groupRank == 0) {
		double localPi;
		int groupSize;
		MPI_Comm_size(groupComm, &groupSize);
		localPi = localPi2 / groupSize;
		MPI_Send(&localPi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		++msg;
		printf("0%d:00%d:Lokalna vrednos broja PI je: %lf\n", rank, msg, localPi);
	}

	MPI_Finalize();
	return EXIT_SUCCESS;
}
