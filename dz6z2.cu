/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 6 - 2. zadatak
 *******************************/

/**
 * 2. Sastaviti program koji pronalazi najmanji i najveci element dvodimenzionalne matrice.
 */

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "device_functions.h"
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_OF_GPU_THREADS 256

__global__ void findMinMaxInMatrix(int *matrix, int *min, int *max, int size) {

	int i;
	int idx = threadIdx.x;

	__shared__ int maxs[NUM_OF_GPU_THREADS];
	__shared__ int mins[NUM_OF_GPU_THREADS];

	int slice = size / NUM_OF_GPU_THREADS;
	int start = idx * slice;
	if (idx == NUM_OF_GPU_THREADS - 1)
		slice += size % NUM_OF_GPU_THREADS;
	int end = start + slice;

	int local_min = matrix[start];
	int local_max = matrix[start];

	for (i = start; i < end; i++) {
		if (matrix[i] < local_min)
			local_min = matrix[i];
		else if (matrix[i] > local_max)
			local_max = matrix[i];
	}

	mins[idx] = local_min;
	maxs[idx] = local_max;
	__syncthreads();
	
	int half = NUM_OF_GPU_THREADS;
	do {
		__syncthreads();
		half >>= 1;
		if (idx < half) {
			if (mins[idx] < mins[idx + half])
				mins[idx] = mins[idx];
			else
				mins[idx] = mins[idx + half];
			if (maxs[idx] > maxs[idx + half])
				maxs[idx] = maxs[idx];
			else
				maxs[idx] = maxs[idx + half];
		}
	} while(half != 1);

	if (idx == 0) {
		*min = mins[0];
		*max = maxs[0];
	}
}

int main(int argc, char *argv[]) {

	int i, m, n;
	int min, max;
	int *h_matrix, *d_matrix, *d_min, *d_max;

	printf("Matrica ce biti ispisana na standardnom izlazu\n");
	printf("Unesite broj vrsta matrice:\n");
	scanf("%d", &m);
	printf("Unesite broj kolona matrice:\n");
	scanf("%d", &n);

	h_matrix = (int*)malloc(m*n*sizeof(int));
	srand(time(0));
	for (i = 0; i < m*n; i++) {
		if (i % m == 0)
			printf("\n");
		h_matrix[i] = -10000 + rand() % 20000;
		printf("%5d ", h_matrix[i]);
	}
	printf("\n");

	cudaMalloc((void**)&d_matrix, m*n*sizeof(int));
	cudaMalloc((void**)&d_min, sizeof(int));
	cudaMalloc((void**)&d_max, sizeof(int));

	cudaMemcpy(d_matrix, h_matrix, m*n*sizeof(int), cudaMemcpyHostToDevice);

	findMinMaxInMatrix<<< 1, NUM_OF_GPU_THREADS >>>(d_matrix, d_min, d_max, m*n);

	cudaThreadSynchronize();

	cudaMemcpy(&min, d_min, sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(&max, d_max, sizeof(int), cudaMemcpyDeviceToHost);

	printf("\nNajmanji element u matrici je: %d\n", min);
	printf("Najveci element u matrici je: %d\n", max);

	cudaFree(d_min);
	cudaFree(d_max);
	cudaFree(d_matrix);
	free(h_matrix);

	return EXIT_SUCCESS;
}


