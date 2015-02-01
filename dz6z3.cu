/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 6 - 3. zadatak
 *******************************/
/**
 * 3. Sastaviti program koji racuna skalarni proizvod dva niza
 */

#include "cuda_runtime.h"
#include "device_functions.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_OF_GPU_THREADS 256

__global__ void dotProduct(int *array1, int *array2, int n, int *result) {

	int i, sum = 0;
	int idx = threadIdx.x;

	__shared__ int dotSum[NUM_OF_GPU_THREADS];

	int slice = n / NUM_OF_GPU_THREADS;
	int start = idx * slice;
	if (idx == NUM_OF_GPU_THREADS - 1)
		slice += n % NUM_OF_GPU_THREADS;
	int end = start + slice;


	for (i = start; i < end; i++) {
		array1[i] = array1[i] * array2[i];
		sum += array1[i];
	}

	dotSum[idx] = sum;
	__syncthreads();

	int half = NUM_OF_GPU_THREADS;
	do {
		__syncthreads();
		half >>= 1;
		if (idx < half)
			dotSum[idx] += dotSum[idx + half];
	} while(half != 1);

	if (idx == 0)
		*result = dotSum[0];
}

int main(int argc, char *argv[]) {
	int i, n;
	int *h_array1, *h_array2;
	int *d_array1, *d_array2;
	int h_result;
	int *d_result;

	printf("Nizovi ce biti ispisani na standardnom izlazu\n");
	printf("Unesite velicinu nizova:\n");
	scanf("%d", &n);

	h_array1 = (int*)malloc(n*sizeof(int));
	h_array2 = (int*)malloc(n*sizeof(int));

	printf("\n");
	srand(time(0));
	for (i = 0; i < n; i++) {
		h_array1[i] = -100 + rand() % 200;
		printf("%2d ", h_array1[i]);
	}
	printf("\n");
	for (i = 0; i < n; i++) {
		h_array2[i] = -100 + rand() % 200;
		printf("%2d ", h_array2[i]);
	}
	printf("\n");

	cudaMalloc((void**)&d_array1, n*sizeof(int));
	cudaMalloc((void**)&d_array2, n*sizeof(int));
	cudaMalloc((void**)&d_result, sizeof(int));

	cudaMemcpy(d_array1, h_array1, n*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_array2, h_array2, n*sizeof(int), cudaMemcpyHostToDevice);
	
	dotProduct<<<1, NUM_OF_GPU_THREADS>>>(d_array1, d_array2, n, d_result);

	cudaThreadSynchronize();

	cudaMemcpy(&h_result, d_result, sizeof(int), cudaMemcpyDeviceToHost);

	printf("\nSkalarni proizvod nizova je: %d\n", h_result);

	cudaFree(d_array1);
	cudaFree(d_array2);
	cudaFree(d_result);
	free(h_array1);
	free(h_array2);

	return EXIT_SUCCESS;
}
