/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 6 - 4. zadatak
 *******************************/

/**
 * 4. Sastaviti program koji menja znak svim elementima niza celih brojeva. Po zavrsenoj obradi niza, treba 
 * ispisati izmenjeni niz, ukupan broj pozitivnih i ukupan broj negativnih elemenata rezultujuceg niza. 
 */

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "device_functions.h"
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_OF_GPU_THREADS 256

__global__ void changeSignsInArray(int *array1, int *positive, int *negative, int n) {
	int i, counter = 0;
	int idx = threadIdx.x;

	__shared__ int numbers[NUM_OF_GPU_THREADS];

	int slice = n / NUM_OF_GPU_THREADS;
	int start = idx * slice;
	if (idx == NUM_OF_GPU_THREADS - 1)
		slice += n % NUM_OF_GPU_THREADS;
	int end = start + slice;

	for (i = start; i < end; i++) {
		array1[i] = -array1[i];
		if (array1[i] > 0)
			counter++;
	}

	numbers[idx] = counter;
	__syncthreads();

	int half = NUM_OF_GPU_THREADS;
	do {
		__syncthreads();
		half >>= 1;
		if (idx < half)
			numbers[idx] += numbers[idx + half];
	} while(half != 1);

	if (idx == 0) {
		*positive = numbers[0];
		*negative = n - *positive;
	}
}

int main(int argc, char *argv[]) {
	
	int i, n, h_positives, h_negatives;
	int *h_array;
	int *d_array, *d_positives, *d_negatives;
	FILE *out;

	printf("Nizovi ce biti upisani u fajl dz6_4_izl1.in\n");
	printf("Unesite velicinu niza:\n");
	scanf("%d", &n);

	h_array = (int*)malloc(n*sizeof(int));

	out = fopen("dz6_4_izl1.in", "w");
	if (out == NULL) {
		printf("Greska pri otvaranju fajla!");
		exit(EXIT_FAILURE);
	}
	fprintf(out, "ORIGINALNI NIZ\n");

	srand(time(0));
	for (i = 0; i < n; i++) {
		h_array[i] = -100 + rand() % 200;
		fprintf(out, "%3d ", h_array[i]);
	}
	fprintf(out, "\n");

	cudaMalloc((void**)&d_array, n*sizeof(int));
	cudaMalloc((void**)&d_positives, sizeof(int));
	cudaMalloc((void**)&d_negatives, sizeof(int));

	cudaMemcpy(d_array, h_array, n*sizeof(int), cudaMemcpyHostToDevice);

	changeSignsInArray<<<1, NUM_OF_GPU_THREADS>>>(d_array, d_positives, d_negatives, n);

	cudaThreadSynchronize();

	cudaMemcpy(h_array, d_array, n*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(&h_positives, d_positives, sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(&h_negatives, d_negatives, sizeof(int), cudaMemcpyDeviceToHost);

	fprintf(out, "IZMENJENI NIZ:\n");
	for (i = 0; i < n; i++)
		fprintf(out, "%3.2d ", h_array[i]);

	fprintf(out, "\n");
	fprintf(out, "Broj pozitivnih elemenata u nizu je: %d\n", h_positives);
	fprintf(out, "Broj negativnih elemenata u nizu je: %d\n", h_negatives);

	cudaFree(d_array);
	cudaFree(d_positives);
	cudaFree(d_negatives);
	free(h_array);
	fclose(out);

	return EXIT_SUCCESS;
}