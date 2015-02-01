/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 2 - 2. zadatak
 *******************************/

/*
 * 2. Napisati program koji ce kreirati NUM_OF_PRODUCERS niti proizvodjaca i NUM_OF_CONSUMERS niti
 * potrosaca. Proizvodjaci proizvode pseudoslucajne brojeve i stavljaju ih u bafer kapaciteta CAPACITY,
 * odakle ih potrosaci uzimaju i ispisuju na konzoli. Konstante NUM_OF_PRODUCERS i NUM_OF_CONSUMERS
 * su jednocifreni pozitivni celi brojevi. Treba proizvesti i ispisati ukupno 50 brojeva.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_OF_PRODUCERS 2	// broj niti proizvodjaca
#define NUM_OF_CONSUMERS 6	// broj niti potrosaca

#define NUMBERS 50	// koliko brojeva treba obraditi

#define CAPACITY 10	// kapacitet bafera

pthread_cond_t notFull, notEmpty;
pthread_mutex_t mutex;

int buffer[CAPACITY];
int head = 0, tail = 0, size = 0;
int counter_producers = 0, counter_consumers = 0;

void* producer(void *arg) {
	int product;
	srand(time(NULL));
	while (counter_producers < NUMBERS) {
		pthread_mutex_lock(&mutex);
		while (size == CAPACITY && counter_producers < NUMBERS)
			pthread_cond_wait(&notFull, &mutex);

		if (counter_producers < NUMBERS) {
			product = -100 + rand() % 200;
			buffer[head] = product;
			head = (head + 1) % CAPACITY;
			size++;
			counter_producers++;
			printf("Produced %d. number = %d\n", counter_producers, product);
			pthread_cond_signal(&notEmpty);
		}
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

void* consumer(void *arg) {
	int product;
	while (counter_consumers < NUMBERS) {
		pthread_mutex_lock(&mutex);
		while (size == 0 && counter_consumers < NUMBERS)
			pthread_cond_wait(&notEmpty, &mutex);

		if (counter_consumers < NUMBERS) {
			product = buffer[tail];
			tail = (tail + 1) % CAPACITY;
			size--;
			counter_consumers++;
			printf("Consumed %d. number = %d\n", counter_consumers, product);
			pthread_cond_signal(&notFull);
		}
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

int main(int argc, char* argv[]) {

	int i, rc;

	pthread_t producers[NUM_OF_PRODUCERS];
	pthread_t consumers[NUM_OF_CONSUMERS];
	pthread_attr_t attribute;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&notFull, NULL);
	pthread_cond_init(&notEmpty, NULL);

	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	// PTHREAD_CREATE
	for (i = 0; i < NUM_OF_PRODUCERS; i++) {
		rc = pthread_create(&producers[i], &attribute, producer, (void *)i);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < NUM_OF_CONSUMERS; i++) {
		rc = pthread_create(&consumers[i], &attribute, consumer, (void *)NULL);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_create() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	// PTHREAD_JOIN
	for (i = 0; i < NUM_OF_PRODUCERS; i++) {
		rc = pthread_join(producers[i], NULL);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_join() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < NUM_OF_CONSUMERS; i++) {
		rc = pthread_join(consumers[i], NULL);
		if (rc) {
			printf("GRESKA! Povratni kod iz pthread_join() je: %d", rc);
			exit(EXIT_FAILURE);
		}
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&notFull);
	pthread_cond_destroy(&notEmpty);
	pthread_attr_destroy(&attribute);

	return EXIT_SUCCESS;
}
