/******************************
 * Tisma Miroslav 2006/0395
 * Multiprocesorski sistemi
 * domaci zadatak 6 - 1. zadatak
 *******************************/

/**
 * 1. Sastaviti program koji kvadrira elemente dvodimenzionalne matrice.
 */

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_GPU_THREADS 256

__global__ void matrixSquare(int *matrix) {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	matrix[idx] = matrix[idx] * matrix[idx];
}

int main(int argc, char *argv[]) {
	int i, j, m, n;
	int *h_matrix, *d_matrix;
	FILE *out;

	printf("Matrice ce biti upisane u fajl dz6_1_izl1.in\n");
	printf("Unesite broj vrsta matrice:\n");
	scanf("%d", &m);
	printf("Unesite broj kolona matrice:\n");
	scanf("%d", &n);

	out = fopen("dz6_1_izl1.in", "w");
	if (out == NULL) {
		printf("Greska pri otvaranju fajla!");
		exit(EXIT_FAILURE);
	}

	fprintf(out, "ORIGINALNA MATRICA:\n");

	h_matrix = (int*) malloc(m*n * sizeof(int));
	for (i = 0; i < m*n; i++) {
		if (i % m == 0)
			fprintf(out, "\n");
		h_matrix[i] = -100 + rand() % 200;
		fprintf(out, "%4d ", h_matrix[i]);
	}

	int dimGrid(m*n / NUM_OF_GPU_THREADS + 1);
	int dimBlock(NUM_OF_GPU_THREADS);
	int size = (m*n * sizeof(int) / (dimGrid * dimBlock) + 1) * (dimGrid * dimBlock);

	cudaMalloc((void**)&d_matrix, size);
	cudaMemcpy(d_matrix, h_matrix, m*n * sizeof(int), cudaMemcpyHostToDevice);
	matrixSquare<<<dimGrid, dimBlock>>>(d_matrix);
	
	cudaThreadSynchronize();

	cudaMemcpy(h_matrix, d_matrix, m*n * sizeof(int), cudaMemcpyDeviceToHost);

	fprintf(out, "\n\nKVADRIRANA MATRICA\n\n");
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++)
			fprintf(out, "%5d ", h_matrix[i * m + j]);
		fprintf(out, "\n");
	}

	fclose(out);
	cudaFree(d_matrix);

	return EXIT_SUCCESS;
}