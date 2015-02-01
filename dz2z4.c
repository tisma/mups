/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 2 - 4. zadatak
 *******************************/

/*
 * 4. Resiti prethodni problem primenom brava (pthread_mutex), bez koriscenja uslovnih promenljivih.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 4		// broj niti u programu, pored glavne niti

pthread_mutex_t mutex[N];	// niz mutex-a za sinhronizaciju

int sum = 0;	// zbir svih brojeva koje smo uneli

void* thread(void* arg) {

	int thread_id = (int)arg;
	int local_num;

	pthread_mutex_lock(&mutex[thread_id]);

	printf("Zdravo, ja sam nit broj: %d\nUnesite jedan ceo broj:\n", thread_id);
	scanf("%d", &local_num);
	sum += local_num;

	if (thread_id == N-1) {
		pthread_mutex_unlock(&mutex[0]);
	} else {
		pthread_mutex_unlock(&mutex[thread_id+1]);
	}

	pthread_exit(NULL);
	return 0;
}

int main(int argc, char* argv[]) {

	int i, rc, num;
	pthread_t threads[N];
	pthread_attr_t attribute;

	for (i = 0; i < N; i++) {
		pthread_mutex_init(&mutex[i], NULL);
		pthread_mutex_lock(&mutex[i]);
	}

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	for (i = 1; i < N; i++) {
		rc = pthread_create(&threads[i-1], &attribute, thread, (void*)i);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	printf("Zdravo, ja sam glavna nit.\nUnesite jedan ceo broj:\n");
	scanf("%d", &num);
	sum += num;

	pthread_mutex_unlock(&mutex[1]);
	pthread_mutex_lock(&mutex[0]);

	printf("Zbir svih brojeva je: %d\n", sum);

	pthread_mutex_unlock(&mutex[0]);

	pthread_attr_destroy(&attribute);
	for (i = 0; i < N; i++)
		pthread_mutex_destroy(&mutex[i]);
	return EXIT_SUCCESS;
}
