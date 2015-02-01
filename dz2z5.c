/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 2 - 5. zadatak
 *******************************/

/*
 * 5. Napisati program koji vrsi mnozenje dve dinamicki alocirane matrice elemenata tipa double. Glavna
 * nit ucitava dimenzije i elemente matrica, stvara NUM_OF_THREADS niti i rasporedjuje posao ostalim
 * nitima na sledeci nacin: svaka stvorena nit dobija zadatak da izracuna jedan element rezultujuce
 * matrice. Kada zavrsi racunanje tekuceg elementa, stvorena nit trazi od glavne niti podatke potrebne za
 * racunanje narednog elementa, sve dok ima posla. Glavna nit treba da ispise rezultujucu matricu. Pri
 * resavanju pretpostaviti da je broj niti manji od broja elemenata rezultujuce matrice.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS 2	// broj niti u programu pored glavne niti
#define VALID 1
#define NOT_VALID -1
#define FINISHED 0

pthread_mutex_t mutex[NUM_OF_THREADS];
pthread_cond_t cond[NUM_OF_THREADS];
pthread_mutex_t mutexMain;
pthread_cond_t condMain;

double **matrixA;
double **matrixB;
double **matrixC;

int mA, nA, mB, nB;

typedef struct {
	int row;
	int col;
	int valid;
} Job;

Job jobs[NUM_OF_THREADS];

double** createMatrix(int m, int n) {
	double **matrix;
	int i, j;
	matrix = calloc(m, sizeof(double*));
	for (i = 0; i < m; i++) {
		matrix[i] = calloc(n, sizeof(double));
		printf("Unesite %d. vrstu matrice:\n", i);
		for (j = 0; j < n; j++)
			scanf("%lf", &matrix[i][j]);
	}
	return matrix;
}

void printMatrix(double **matrix, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			printf("%4.2lf ", matrix[i][j]);
		}
		printf("\n");
	}
}

void* thread(void* arg) {

	int index = (int)arg;
	int i;
	double res = 0.0;
	Job myJob;

	while (1) {
		pthread_mutex_lock(&mutex[index]);

		while(!jobs[index].valid == VALID)	// blokiramo se dok ne postavi glavna nit
			pthread_cond_wait(&cond[index], &mutex[index]);
		if (jobs[index].valid == NOT_VALID)	// ako nema posla, iskaci iz petlje
			break;
		// obavestavamo glavnu nit da smo zavrsili
		jobs[index].valid = FINISHED;
		pthread_cond_signal(&cond[index]);

		myJob = jobs[index];
		for (i = 0; i < mA; i++)
			res += matrixA[myJob.row][i]*matrixB[i][myJob.col];

		matrixC[myJob.row][myJob.col] = res;

		pthread_mutex_unlock(&mutex[index]);
	}

	pthread_exit(NULL);
	return 0;
}

int main(int argc, char* argv[]) {

	int i, j, rc, job_counter = 0;

	pthread_t threads[NUM_OF_THREADS];
	pthread_attr_t attribute;

	printf("Unesite broj vrsta i kolona prve matrice:\n");
	scanf("%d %d", &mA, &nA);

	printf("Unesite broj vrsta i kolona druge matrice:\n");
	scanf("%d %d", &mB, &nB);

	if (mA != mB || mA < 1 || mB < 1) {
		printf("Greska pri unosu velicine matrica!\n");
		exit(EXIT_FAILURE);
	}

	matrixA = createMatrix(mA, nA);
	printf("MATRICA A:\n\n");
	printMatrix(matrixA, mA, nA);
	printf("\n");

	matrixB = createMatrix(mB, nB);
	printf("\n\nMATRICA B:\n\n");
	printMatrix(matrixB, mB, nB);
	printf("\n");

	matrixC = calloc(mA, sizeof(double*));
	for (i = 0; i < mA; i++)
		matrixC[i] = calloc(nB, sizeof(double));

	for (i = 0; i < NUM_OF_THREADS; i++) {
		pthread_mutex_init(&mutex[i], NULL);
		pthread_cond_init(&cond[i], NULL);
	}

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < NUM_OF_THREADS; i++) {
		rc = pthread_create(&threads[i], &attribute, thread, (void*)i);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	job_counter = 0;
	for (i = 0; i < mA; i++) {
		for (j = 0; j < nB; j++) {
			pthread_mutex_lock(&mutex[job_counter]);
			jobs[job_counter].row = i;	// postavi indeks vrste
			jobs[job_counter].col = j;	// postavi indeks kolone
			jobs[job_counter].valid = VALID;	// oznaci da je validno i javi blokiranoj niti
			pthread_cond_signal(&cond[job_counter]);
			pthread_cond_wait(&cond[job_counter], &mutex[job_counter]); // blokiraj se dok ne zavrsi
			pthread_mutex_unlock(&mutex[job_counter]);
			job_counter = (job_counter +  1) % NUM_OF_THREADS;
		}
	}

	// obavesti sve da nema vise posla
	for (i = 0; i < NUM_OF_THREADS; i++) {
		pthread_mutex_lock(&mutex[i]);
		jobs[i].row = NOT_VALID;
		jobs[i].col = NOT_VALID;
		jobs[i].valid = NOT_VALID;
		pthread_cond_signal(&cond[i]);
		pthread_mutex_unlock(&mutex[i]);
	}

	printf("\n\nMATRICA C:\n\n");
	printMatrix(matrixC, mA, nB);

	pthread_attr_destroy(&attribute);
	for (i = 0; i < NUM_OF_THREADS; i++) {
		pthread_mutex_destroy(&mutex[i]);
		pthread_cond_destroy(&cond[i]);
	}
	pthread_mutex_destroy(&mutexMain);
	pthread_cond_destroy(&condMain);

	for (i = 0; i < mA; i++) {
		free(matrixA[i]);
		free(matrixC[i]);
	}
	free(matrixA);
	free(matrixC);

	for(i = 0; i < mB; i++)
		free(matrixB[i]);
	free(matrixB);

	return EXIT_SUCCESS;
}
