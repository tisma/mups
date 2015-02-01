/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 2 - 1. zadatak
 *******************************/

/*
 * 1. Napisati program koji ce kreirati dve korisnicke niti, koje ce ciklicno raditi sledeci posao: prva nit
 * treba da od korisnika zatrazi unos celog broja, a druga nit da ispise uneti broj na standardnom izlazu.
 * Niti treba da rade sve dok korisnik ne unese negativan broj, koji takodje treba ispisati. Unos narednog
 * broja moze zapoceti tek po ispisu prethodno unetog broja. Koristiti uslovne promenljive.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS 2

pthread_cond_t condition;
pthread_mutex_t mutex;
int number, turn = 1, outExit = 0;

void* input(void *arg) {
	do {
		if(turn) {
			pthread_mutex_lock(&mutex);
			printf("Unesite jedan ceo broj:\n");
			scanf("%d", &number);
			turn = 0;
			if(number < 0) {
				outExit = 1;
				pthread_mutex_unlock(&mutex);
				pthread_cond_signal(&condition);
				break;
			}
			pthread_mutex_unlock(&mutex);
			pthread_cond_signal(&condition);
		}
	} while(1);
	pthread_exit(NULL);
}

void* output(void* arg) {
	do {
		if(turn == 0) {
			pthread_mutex_lock(&mutex);
			while(turn != 0) {
				pthread_cond_wait(&condition, &mutex);
			}
			printf("Broj koji ste uneli je:%d\n", number);
			turn = 1;
			if(outExit) {
				break;
			}
			pthread_mutex_unlock(&mutex);
		}
	} while(1);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

	int i, rc;
	pthread_t threads[NUM_OF_THREADS];
	pthread_attr_t attribute;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	rc = pthread_create(&threads[0], &attribute, input, NULL);
	if (rc) {
		printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
		exit(EXIT_FAILURE);
	}

	rc = pthread_create(&threads[1], &attribute, output, NULL);
	if (rc) {
		printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < NUM_OF_THREADS; i++) {
		rc = pthread_join(threads[i], NULL);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_join() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condition);
	pthread_attr_destroy(&attribute);
	return EXIT_SUCCESS;
}
