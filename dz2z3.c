/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 2 - 3. zadatak
 *******************************/

/*
 * 3. Od svih niti u programu potrebno je stvoriti prsten (ciklicki graf) na sledeci nacin: nitima dodeliti
 * jedinstveni celobrojni identifikator iz opsega [1,N] (identifikator glavne niti je 0), koji ce omoguciti
 * rastuce uredjenje niti. Glavna nit ucitava sa standardnog ulaza broj i salje narednoj niti. Svaka od
 * ostalih niti treba da primi broj od prethodne, potom sa glavnog ulaza procita ceo broj, sabere procitani
 * sa primljenim brojem i zbir posalje narednoj niti. Poslednja nit salje svoj rezultat glavnoj niti koja
 * ispisuje taj broj (sumu svih unetih brojeva) na glavnom izlazu i zavrsava program. Za sinhronizaciju je
 * obavezno koristiti uslovne promenljive. Nije dozvoljeno koristiti funkciju pthread_join().
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5

pthread_mutex_t mutex, mutex2;
pthread_cond_t conditions[N];

int sum = 0;	// deljena promenljiva u koju svi upisiju

void* thread(void *arg) {

	int thread_id = (int)arg;
	int local_num;

	// uzmi bravu i blokiraj se na svoj thread_id
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&conditions[thread_id], &mutex);
	pthread_mutex_unlock(&mutex);

	// stigao je singal od prethodne niti
	printf("Zdravo, ja sam nit broj: %d\nUnesite jedan ceo broj:\n", thread_id);
	scanf("%d", &local_num);

	sum += local_num;

	// mora N+1, zbog < !!!
	if(thread_id == N-1) {
		// ja sam poslednja nit, signaliziram main niti
		pthread_cond_signal(&conditions[0]);
	} else {
		// nisam poslednja, signaliziram sledecoj niti
		pthread_cond_signal(&conditions[thread_id+1]);
	}

	pthread_exit(NULL);
	return 0;
}

int main(int argc, char *argv[]) {

	int i, rc, num;
	pthread_t threads[N];
	pthread_attr_t attribute;

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex2, NULL);
	for (i = 0; i < N; i++)
		pthread_cond_init(&conditions[i], NULL);

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	for (i = 1; i < N; i++) {
		rc = pthread_create(&threads[i-1], &attribute, thread, (void *)i);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	printf("Zdravo, ja sam glavna nit.\nUnesite jedan ceo broj:\n");
	scanf("%d", &num);
	sum += num;

	// signaliziramo prvoj niti da krene
	pthread_cond_signal(&conditions[1]);

	// blokiram se na uslovnoj promenljivoj i cekam poslednju nit
	pthread_mutex_lock(&mutex2);
	pthread_cond_wait(&conditions[0], &mutex2);
	pthread_mutex_unlock(&mutex2);

	printf("Zbir svih brojeva je: %d\n", sum);

	pthread_mutex_destroy(&mutex);
	for (i = 0; i < N; i++)
		pthread_cond_destroy(&conditions[i]);
	pthread_attr_destroy(&attribute);
	return EXIT_SUCCESS;
}
