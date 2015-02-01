/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 6 - 5. zadatak
 *******************************/

/**
 * 5. Sastaviti program koji odredjuje vrednost broja PI na neki od dva ispod opisana nacina.
 */

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "device_functions.h"
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_OF_GPU_THREADS 256

__global__ void calculatePi(int *array1, int n, float *pi) {
	int i;
	float sum = 0.0;
	int idx = threadIdx.x;

	__shared__ float numbers[NUM_OF_GPU_THREADS];

	int slice = n / NUM_OF_GPU_THREADS;
	int start = idx * slice;
	if (idx == NUM_OF_GPU_THREADS - 1)
		slice += n % NUM_OF_GPU_THREADS;
	int end = start + slice;

	for (i = start; i < end; i++) {
		if (i == 0) continue;	
		sum += (float)1.0 / (float)(i*i);
	}

	numbers[idx] = sum;
	__syncthreads();

	int half = NUM_OF_GPU_THREADS;
	do {
		__syncthreads();
		half >>= 1;
		if (idx < half)
			numbers[idx] += numbers[idx + half];
	} while(half != 1);

	if (idx == 0) {
		*pi = sqrt(6.0 * numbers[0]);
	}

}

int main(int argc, char *argv[]) {
	int i, n;
	int *h_array;
	int *d_array;
	float h_pi;
	float *d_pi;

	printf("Unesite broj tacaka za priblizno racunanje broja PI:\n");
	scanf("%d", &n);

	h_array = (int*)malloc(n*sizeof(int));
	
	srand(time(0));
	for (i = 0; i < n; i++)
		h_array[i] = rand();
		
	cudaMalloc((void**)&d_array, n*sizeof(int));
	cudaMalloc((void**)&d_pi, sizeof(float));

	cudaMemcpy(d_array, h_array, n*sizeof(int), cudaMemcpyHostToDevice);

	calculatePi<<<1, NUM_OF_GPU_THREADS>>>(d_array, n, d_pi);

	cudaThreadSynchronize();

	cudaMemcpy(&h_pi, d_pi, sizeof(float), cudaMemcpyDeviceToHost);

	printf("Priblizna vrednost broja PI je: %lf\n", h_pi);

	cudaFree(d_array);
	cudaFree(d_pi);
	free(h_array);

	return EXIT_SUCCESS;
}

