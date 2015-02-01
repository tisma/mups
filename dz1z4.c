/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak - 4. zadatak
 *******************************/

/* 4. Sastaviti program koji menja znak svim elementima niza celih brojeva. Glavna nit treba da ucita
 * velicinu i elemente niza, da svakoj od stvorenih niti prosledi pocetni i krajnji indeks dela niza koji data
 * nit treba da obradi, te da i sama ucestvuje u obradi. Po zavrsenoj obradi niza, glavna nit ispisuje
 * rezultujuci niz. Raspodela posla izmedu niti treba da bude što ravnomernija. [1]
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS 5

int *array;

typedef struct {
	int begin;
	int end;
	int *a;
} thread_data;

void* change_sign_of_array(void* arg) {
	int begin = ((thread_data *)arg)->begin;
	int end = ((thread_data *)arg)->end;
	int *a = ((thread_data *)arg)->a;
	int i;

	for(i = begin; i < end; i++) {
		a[i] *= -1;
	}

	pthread_exit(NULL);
	return 0;
}

int main(int argc, char* argv[]) {

	int i, rc, status = 0, n;
	pthread_t threads[NUM_OF_THREADS];
	pthread_attr_t attribute;
	thread_data td[NUM_OF_THREADS];

	printf("Unesite broj elemenata niza:\n");
	scanf("%d", &n);
	array = calloc(n, sizeof(int));

	for(i = 0; i < n; i++) {
		printf("array[%d]=", i);
		scanf("%d", array+i);
	}

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	for(i = 0; i < NUM_OF_THREADS; i++) {
		td[i].begin = i*n/(NUM_OF_THREADS+1);
		td[i].end = (i+1)*n/(NUM_OF_THREADS+1);
		td[i].a = array;
		rc = pthread_create(&threads[i], &attribute, change_sign_of_array, (void *)&td[i]);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	// glavna nit obradjuje do kraja niza sta je ostalo
	for(i = NUM_OF_THREADS*n/(NUM_OF_THREADS + 1); i < n; i++) {
		array[i] *= -1;
	}

	for(i = 0; i < NUM_OF_THREADS; i++) {
		rc = pthread_join(threads[i], (void **)status);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_join() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < n; i++) {
		printf("array[%d] = %d\n", i, array[i]);
	}

	pthread_attr_destroy(&attribute);
	free(array);
	return EXIT_SUCCESS;
}
