/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 5 - 5. zadatak
 *******************************/

/**
 * 5. Sastaviti program koji u nizu kompleksnih brojeva pronalazi broj sa najvecom apsolutnom vrednoscu.
 * Kompleksni broj je predstavljen sa dva realna broja i jednim celim brojem. Ako je celi broj jednak 0,
 * realni brojevi predstavljaju realni i imaginarni deo kompleksnog broja, inace predstavljaju moduo i
 * argument. Proces sa rangom 0 treba da ucita broj elemenata niza i elemente niza, a zatim da posalje
 * ostalim procesima odgovarajuci deo niza na obradu. Za prenos podniza koristiti samo jedan MPI
 * poziv Send i njemu odgovarajuci Receive. [2,N]
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 6		// maksimalan broj procesa u sistemu
#define MAX 100	// maksimalna duzina niza

typedef struct {
	double Re, Im;
	int p;
} Complex;

Complex complexArray[MAX];

double absoluteValue(Complex c) {
	if (c.p == 0) {
		return sqrt(pow(c.Re, 2) + pow(c.Im, 2));
	} else {
		return c.Re;
	}
}

MPI_Datatype complex_type;

int main(int argc, char *argv[]) {

	int i, j = 1, n, rank, size, slice, msg = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2 || size > N) {
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}

	// kreiranje strukturiranog tipa
	int blocklens[2];			// duzine podataka starih tipova
	MPI_Aint offsets[2];		// pomeraji starih tipova
	MPI_Datatype oldtypes[2];	// stari tipovi
	MPI_Aint doubleExtent;		// pomeraj za MPI_DOUBLE
	MPI_Type_extent(MPI_DOUBLE, &doubleExtent);

	blocklens[0] = 2;
	offsets[0] = 0;
	oldtypes[0] = MPI_DOUBLE;

	blocklens[1] = 1;
	offsets[1] = 2 * doubleExtent;
	oldtypes[1] = MPI_INT;

	MPI_Type_struct(2, blocklens, offsets, oldtypes, &complex_type);
	MPI_Type_commit(&complex_type);

	if (rank == 0) {
		++msg;
		printf("00:00%d:Unesite broj elemenata niza kompleksnih brojeva:\n", msg);
		scanf("%d", &n);
		if (n % (size-1) != 0) {
			printf("Morate uneti broj deljiv sa brojem procesa radnika");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < n; i++) {
			++msg;
			printf("00:00%d:Unestite 2 realna broja i 1 ceo (0 ReIm ili 1 ModArg):\n", msg);
			++msg;
			printf("00:00%d:Unesite %d. element niza:\n", msg, i);
			scanf("%lf %lf %d", &complexArray[i].Re, &complexArray[i].Im, &complexArray[i].p);
		}

		// parce posla za svakog radnika
		slice = n / (size - 1);

		MPI_Bcast(&slice, 1, MPI_INT, 0, MPI_COMM_WORLD);

		for (i = 0; i < n; i += slice) {
			MPI_Send(&complexArray[i], slice, complex_type, j, 0, MPI_COMM_WORLD);
			j++;
		}

		Complex locMaxs[slice];
		for (i = 0; i < slice; i++) {
			MPI_Recv(&locMaxs[i], 1, complex_type, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		Complex max = locMaxs[0];
		for (i = 1; i < slice; i++)
			if (absoluteValue(max) < absoluteValue(locMaxs[i]))
				max = locMaxs[i];

		++msg;
		printf("00:00%d:Kompleksni broj sa najvecom apsolutnom vrednoscu je:\n", msg);
		++msg;
		printf("00:00%d: %.2lf %.2lf abs = %.2lf\n", msg, max.Re, max.Im, absoluteValue(max));
	}
	else {
		int slice;

		MPI_Bcast(&slice, 1, MPI_INT, 0, MPI_COMM_WORLD);
		Complex array[slice];
		Complex locMax;

		MPI_Recv(&array, slice, complex_type, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		locMax = array[0];
		for (i = 1; i < slice; i++) {
			if (absoluteValue(locMax) < absoluteValue(array[i]))
				locMax = array[i];
		}

		MPI_Send(&locMax, 1, complex_type, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Type_free(&complex_type);

	MPI_Finalize();

	return EXIT_SUCCESS;
}
