/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 1 - 1. zadatak
 *******************************/

/*
 * Sastaviti program koji odredjuje vrednost broja PI. Glavna nit treba da stvori ostale niti, obavesti ih o
 * broju tacaka koje treba da obrade (unosi se sa glavnog ulaza) i prikupi podatke od njih po zavrsenom
 * racunanju. Glavna nit ne ucestvuje u obradi tacaka. Niti koje racunaju ne dele zajednicke podatke i ne
 * smeju medusobno da se sinhronizuju. Za sinhronizaciju koristiti samo pthread_join(). [1]
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS 4	// broj niti u programu

void* calculate_PI(void* arg) {
	int* points_to_generate = (int*) arg;
	int i;
	int *in_circle = malloc(sizeof(int));
	double x, y, point;

	srand((unsigned) time(NULL));
	for (i = 0; i < *points_to_generate; i++) {
		x = (double) rand() / RAND_MAX;
		y = (double) rand() / RAND_MAX;
		point = x * x + y * y;
		if (point <= 1.0)
			(*in_circle)++;
	}

	pthread_exit((void *)in_circle);
}

int main(int argc, char* argv[]) {

	pthread_t threads[NUM_OF_THREADS];
	pthread_attr_t attribute;

	int i, rc, num_of_points, circle_count = 0;
	int *status = malloc(sizeof(int));
	double pi = 0.0;

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	printf("Unesite broj tacaka za svaku nit:\n");
	scanf("%d", &num_of_points);
	if(num_of_points < 1) {
		printf("Morate uneti broj tacaka veci od nule!\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < NUM_OF_THREADS; i++) {
		rc = pthread_create(&threads[i], &attribute, calculate_PI, (void*) &num_of_points);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
		}
	}

	pthread_attr_destroy(&attribute);

	for (i = 0; i < NUM_OF_THREADS; i++) {
		rc = pthread_join(threads[i], (void**) &status);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_join() je: %d", rc);
			exit(-1);
		}
		circle_count += (int) (*status);
	}

	pi = (4.0 * circle_count) / (double) (NUM_OF_THREADS * num_of_points);
	printf("Vrednost broja PI je: %lf\n", pi);

	return EXIT_SUCCESS;
}
