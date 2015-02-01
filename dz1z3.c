/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 1 - 3. zadatak
 *******************************/

/* 3. Sastaviti program koji ce korisnicke niti programa podeliti u dve grupe, prema parnosti identifikatora.
 * Svaka nit pri kreiranju dobija od glavne niti jedinstveni identifikator. Glavna nit sa glavnog ulaza cita
 * ukupan broj N niti koje treba stvoriti. Vrednosti identifikatora su u celobrojnom opsegu [1,N].
 * Stvorene niti treba da izracunaju geometrijsku sredinu svojih identifikatora u grupi. Glavna nit ne
 * ucestvuje u racunanju. Glavna nit treba da ispise rezultat rada obe grupe niti. Niti iz razlicitih grupa
 * medusobno ne komuniciraju. Niti unutar iste grupe dele najviše jednu zajednicku promenljivu. [2]
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define NUM_OF_THREADS 4	// broj niti u programu

pthread_mutex_t mutex_g1, mutex_g2;
double g1 = 1, g2 = 1;

void* geometric_mean_calc(void* arg) {
	int id = (int) arg;
	if (id % 2) {
		pthread_mutex_lock(&mutex_g1);
		g1 *= id;
		pthread_mutex_unlock(&mutex_g1);
	} else {
		pthread_mutex_lock(&mutex_g2);
		g2 *= id;
		pthread_mutex_unlock(&mutex_g2);

	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

	int i, n, rc, status = 0;
	pthread_attr_t attribute;
	pthread_t *threads;

	printf("Unesite broj niti koje treba kreirati:\n");
	scanf("%d", &n);
	if(n < 0) {
		printf("GRESKA! Niste uneli pozitivan broj!");
		exit(-1);
	}

	threads = calloc(n, sizeof(pthread_t));

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	pthread_mutex_init(&mutex_g1, NULL);
	pthread_mutex_init(&mutex_g2, NULL);

	for(i = 0; i < n; i++) {
		rc = pthread_create(&threads[i], &attribute, geometric_mean_calc, (void *)i+1);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	pthread_attr_destroy(&attribute);

	for(i = 0; i < n; i++) {
		rc = pthread_join(threads[i], (void **)status);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_join() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	g2 = pow(g2, 1/(n/2 + 0.));
	if (n % 2) {
		g1 = pow(g1, 1/((n/2) + 1.0));
	} else {
		g1 = pow(g1, 1/(n/2 + 0.));
	}

	printf("Geometrijska sredina prve (neparne) grupe niti je: %lf\n", g1);
	printf("Geometrijska sredina druge (parne) grupe niti je: %lf", g2);

	pthread_mutex_destroy(&mutex_g1);
	pthread_mutex_destroy(&mutex_g2);
	free(threads);

	return EXIT_SUCCESS;
}

