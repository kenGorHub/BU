
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <math.h>
#include <windows.h>

typedef struct {
  int width;
  int height;
  int stride;
  float* elements;
} Matrix;

#define WIDTH_A 1024	
#define HEIGHT_A 1024

#define WIDTH_B 1024
#define HEIGHT_B 1024

#define BLOCK_SIZE 16
#define EPSILON 0.000001

void randMatInit(Matrix A)
{
	int row, column;
	
	for( row = 0; row < A.width; ++row)
	{
		for( column = 0; column < A.height; ++column)
		{
			A.elements[row * A.width + column] = ((float)(rand() % 1000)/10.0);
		}
	}
}

__device__ float GetElement(const Matrix A, int row, int col)
{
	return A.elements[A.stride * row + col];
}

__device__ void SetElement(Matrix A, int row, int col, float value)
{
	A.elements[A.stride * row + col] = value;
}

__device__ Matrix GetSubMatrix(Matrix A, int row, int col)
{
	Matrix Asub;
	Asub.width = BLOCK_SIZE;
	Asub.height = BLOCK_SIZE;
	Asub.stride = A.stride;
	Asub.elements = &A.elements[A.stride * BLOCK_SIZE * row + BLOCK_SIZE *col];
	
	return Asub;
}

__global__ void MatMulKernel_Share(const Matrix A, const Matrix B, Matrix C)
{
	int blockRow = blockIdx.y;
	int blockCol = blockIdx.x; 
	
	Matrix Csub = GetSubMatrix(C, blockRow, blockCol);
	
	float pValue = 0.0;
	
	int row = threadIdx.y;
	int col = threadIdx.x;
	
	for(int index = 0; index < (A.width / BLOCK_SIZE); ++index)
	{	
		Matrix Asub = GetSubMatrix(A, blockRow, index);
		Matrix Bsub = GetSubMatrix(B, index, blockCol);
		
		__shared__ float As[BLOCK_SIZE][BLOCK_SIZE];
		__shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];
		
		As[row][col] = GetElement(Asub, row, col);
		Bs[row][col] = GetElement(Bsub, row, col);
		
		__syncthreads();// Synchronize to make sure the sub-matrices As and Bs are loaded over before starting the computation
		
		for(int subIndex = 0 ; subIndex < BLOCK_SIZE; ++subIndex)
		{
			pValue += As[row][subIndex] * Bs[subIndex][col];
		}
		
		__syncthreads(); // Synchronize to make sure the computation completed for all threads before next load and computation round
	}
	
	SetElement(Csub, row, col, pValue);
}

int MatCompare(const Matrix A, const Matrix B)
{
	int i, j, index;
	if(A.height != B.height || A.width != B.width)
		return 0;
	for(i = 0; i < A.height; i++)
		for(j = 0; j < A.width; j++) {
			index = i * A.width + j;
			if(fabsf(A.elements[index] - B.elements[index]) > EPSILON)
				return 0;
		}
	return 1;
}

void MatMul_CPU(const Matrix A, const Matrix B, Matrix C)
{
	int i, j, k;
	for(i = 0; i < C.width; i++)
		for(j = 0; j < C.height; j++) {
			float sum = 0;
			for(k = 0; k < A.width; k++)
				sum += A.elements[i * A.width + k] * B.elements[k * B.width + j];
			C.elements[i * C.width + j] = sum;
		}
}


void MatMul_GPU(const Matrix A, const Matrix B, Matrix C)
{
	Matrix d_A, d_B, d_C;
	
	float elapsedTime;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	
	d_A.width = A.width;
	d_A.height = A.height;
	d_A.stride = d_A.width;

	d_B.width = B.width;
	d_B.height = B.height;
	d_B.stride = d_B.width;
	
	d_C.width = B.width;
	d_C.height = A.height;
	d_C.stride = d_B.width;
	
	cudaMalloc(&(d_A.elements), d_A.width * d_A.height * sizeof(float));
	cudaMemcpy(d_A.elements, A.elements, d_A.width * d_A.height * sizeof(float), cudaMemcpyHostToDevice);
	
	cudaMalloc(&(d_B.elements), d_B.width *d_B.height * sizeof(float));
	cudaMemcpy(d_B.elements, B.elements, d_B.width *d_B.height * sizeof(float), cudaMemcpyHostToDevice);
	
	cudaMalloc(&(d_C.elements), d_C.width * d_C.height * sizeof(float));

	dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE); // each block has BLOCK_SIZE * BLOCK_SIZE threads with two dimensions.
	
	dim3 dimGrid(ceil(1.0 * d_C.height / dimBlock.x), ceil(1.0 * d_C.width / dimBlock.y));
	
	cudaEventRecord(start);
	MatMulKernel_Share<<<dimGrid, dimBlock>>>(d_A, d_B, d_C);
	cudaEventRecord(stop);
	
	cudaEventSynchronize(stop);
	
	cudaEventElapsedTime(&elapsedTime, start, stop);
		
	cudaMemcpy(C.elements, d_C.elements, d_C.width * d_C.height * sizeof(float), cudaMemcpyDeviceToHost);
	
	cudaFree(d_A.elements);
	cudaFree(d_B.elements);
	cudaFree(d_C.elements);
	
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
	
	printf("Matrix Multiplication time on GPU is: %fms\n", elapsedTime);
}

int main()
{
	cudaError_t cudaStatus;
	Matrix A, B, C, D;
	DWORD start, end;

	A.width = WIDTH_A;
	A.height = HEIGHT_A;
	
	B.width = WIDTH_B;
	B.height = HEIGHT_B;
	
	C.width = B.width;
	C.height = A.height;

	D.width = B.width;
	D.height = A.height;

	A.elements = (float *)malloc(sizeof(float) * A.width * A.height);
	B.elements = (float *)malloc(sizeof(float) * B.width * B.height);
	C.elements = (float *)malloc(sizeof(float) * C.width * C.height);
	D.elements = (float *)malloc(sizeof(float) * D.width * D.height);
	
	randMatInit(A);
	randMatInit(B);

	/* Run matrix multiplication on CPU: C = A x B */
	start = GetTickCount();
	MatMul_CPU(A, B, C);
	end = GetTickCount();
	printf("Matrix Multiplication time on GPU is: %dms\n", end-start);

	/* Run matrix multiplication on GPU: D = A x B */
	MatMul_GPU(A, B, D);

	/* verify the results */
	if(!MatCompare(C, D))
		printf("Your GPU results are different from CPU results.\n");
	else
		printf("Your GPU results are the same as CPU results.\n");

	free(A.elements);
	free(B.elements);
	free(C.elements);
	free(D.elements);

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }

    return 0;
}
