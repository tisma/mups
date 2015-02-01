/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak - 5. zadatak
 *******************************/

/* 5. Sastaviti program koji pronalazi maksimalni i minimalni element u 2D matrici realnih brojeva. Glavna
 * nit treba da ucita dimenzije i elemente matrice realnih brojeva, stvori ostale niti, rasporedi im posao i,
 * na kraju, ispise rezultat. Glavna nit ne ucestvuje u pretrazi. Krajnji rezultat treba da bude smesten u
 * dve globalne realne promenljive min i max. Stvorene niti ne smeju da razmenjuju informacije, izuzev
 * preko pomenute dve promenljive. Glavna nit ce svakoj stvorenoj niti dodeliti odgovarajuci broj vrsta
 * matrice za pretragu, tako da posao bude što ravnomernije rasporeden. Zadatak resiti tako da se
 * stvorene niti nikada ne blokiraju (koristiti pthread_mutex_trylock()). Predlog: koristiti razlicite
 * brave za promenljive min i max. [1]
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS 3

double **matrix;
double min, max;
pthread_mutex_t mutex_min, mutex_max;

typedef struct {
	int startRow;
	int endRow;
	int rowSize;
} thread_data;

void* find_min_max_in_matrix(void* arg) {

	int i, j;
	int startRow = ((thread_data*)arg)->startRow;
	int endRow = ((thread_data*)arg)->endRow;
	int rowSize = ((thread_data*)arg)->rowSize;
	double local_min, local_max;

	local_min = local_max = matrix[startRow][0];

	for (i = startRow; i < endRow; i++) {
		for (j = 0; j < rowSize; j++) {
			if (matrix[i][j] < local_min) local_min = matrix[i][j];
			if (matrix[i][j] > local_max) local_max = matrix[i][j];
		}
	}

	while(pthread_mutex_trylock(&mutex_min) != 0);
	if (local_min < min)
		min = local_min;
	pthread_mutex_unlock(&mutex_min);

	while(pthread_mutex_trylock(&mutex_max) != 0);
	if (local_max > max)
		max = local_max;
	pthread_mutex_unlock(&mutex_max);

	pthread_exit(NULL);
	return 0;
}

int main(int argc, char* argv[]) {

	int i, j, rc, status = 0, m, n;
	int startRow = 0, slice, rest;
	pthread_t threads[NUM_OF_THREADS];
	pthread_attr_t attribute;
	thread_data t[NUM_OF_THREADS];

	printf("Unesite broj vrsta matrice:\n");
	scanf("%d", &m);
	printf("Unesite broj kolona matrice:\n");
	scanf("%d", &n);

	matrix = calloc(m, sizeof(double*));
	for (i = 0; i < m; i++) {
		matrix[i] = calloc(n, sizeof(double));
	}
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			printf("matrix[%d][%d]=", i, j);
			scanf("%lf", &matrix[i][j]);
		}
	}

	// inicijalizujemo min i max na pocetku
	min = max = matrix[0][0];

	pthread_mutex_init(&mutex_min, NULL);
	pthread_mutex_init(&mutex_max, NULL);

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	rest = m % NUM_OF_THREADS;
	for (i = 0; i < NUM_OF_THREADS; i++) {
		slice = m / NUM_OF_THREADS;

		if (rest > 0) {
			slice++; rest--;
		}

		t[i].startRow = startRow;
		t[i].endRow = startRow + slice;
		t[i].rowSize = n;

		printf("Nit %d dobija startRow = %d , endRow = %d , rowSize = %d \n", i, t[i].startRow, t[i].endRow, t[i].rowSize);
		rc = pthread_create(&threads[i], &attribute, find_min_max_in_matrix, (void*)&t[i]);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(EXIT_FAILURE);
		}
		startRow += slice;
	}

	for (i = 0; i < NUM_OF_THREADS; i++) {
		rc = pthread_join(threads[i], (void **)status);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_join() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	printf("Minimalni element matrice je: %lf\n", min);
	printf("Maksimalni element matrice je: %lf\n", max);

	pthread_mutex_destroy(&mutex_min);
	pthread_mutex_destroy(&mutex_max);
	pthread_attr_destroy(&attribute);
	for (i = 0; i < m; i++) free(matrix[i]);
	free(matrix);
	return EXIT_SUCCESS;
}
