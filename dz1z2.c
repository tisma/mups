/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 1 - 2. zadatak
 *******************************/

/*
 * 2. Sastaviti program koji odreduje vrednost broja PI. Glavna nit treba da stvori ostale niti, obavesti ih o
 * broju tacaka koje treba da obrade (unosi se sa glavnog ulaza) i prikupi podatke od njih po zavrsenom
 * racunanju. Glavna nit ucestvuje u obradi tacaka. Sve niti dele samo jednu celobrojnu promenljivu i
 * komuniciraju preko nje. Obezbediti logicku ispravnost programa medusobnim iskljucivanjem pomocu
 * brava (pthread_mutex_t).
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS 4

pthread_mutex_t mutex_in_circle;
int in_circle = 0;		// deljena promenljiva za broj tacaka unutar kruga

void* calculate_PI(void* arg) {
	int i, in_circle_loc = 0;
	double x, y, point;
	int *p = (int*)arg;

//	srand((unsigned)time(NULL));
	for(i = 0; i < *p; i++) {
		x = (double)rand() / RAND_MAX;
		y = (double)rand() / RAND_MAX;
		point = x*x + y*y;
		if (point <= 1.0)
			in_circle_loc++;
	}

	pthread_mutex_lock(&mutex_in_circle);
	in_circle += in_circle_loc;
	pthread_mutex_unlock(&mutex_in_circle);

	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	pthread_t threads[NUM_OF_THREADS];
	pthread_attr_t attribute;
	int i, rc, num_of_points;
	double x, y, point, pi;
	int in_circle_main = 0;

	printf("Unesite broj tacaka za generisanje:\n");
	scanf("%d", &num_of_points);

	pthread_mutex_init(&mutex_in_circle, NULL);
	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	for(i = 0; i < NUM_OF_THREADS; i++) {
		rc = pthread_create(&threads[i], &attribute, calculate_PI, (void*)&num_of_points);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(-1);
		}
	}

	//srand((unsigned)time(NULL));
	for(i = 0; i < num_of_points; i++) {
		x = (double)rand() / RAND_MAX;
		y = (double)rand() / RAND_MAX;
		point = x*x + y*y;
		if (point <= 1.0)
			in_circle_main++;
	}

	for (i = 0; i < NUM_OF_THREADS; i++) {
		rc = pthread_join(threads[i], NULL);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_join() je: %d", rc);
			exit(-2);
		}
	}

	pi = (4.0 * (in_circle+in_circle_main)) / (double)((NUM_OF_THREADS + 1)*num_of_points);
	printf("Vrednost broja PI je: %lf\n", pi);

	pthread_attr_destroy(&attribute);
	pthread_mutex_destroy(&mutex_in_circle);
	return EXIT_SUCCESS;
}
