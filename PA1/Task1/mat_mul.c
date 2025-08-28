/*******************************************************************
 * Author: <Name1>, <Name2>
 * Date: <Date>
 * File: mat_mul.c
 * Description: This file contains implementations of matrix multiplication
 *			    algorithms using various optimization techniques.
 *******************************************************************/

// PA 1: Matrix Multiplication

// includes
#include <stdio.h>
#include <stdlib.h>         // for malloc, free, atoi
#include <time.h>           // for time()
#include <chrono>	        // for timing
#include <xmmintrin.h> 		// for SSE
#include <immintrin.h>		// for AV
#include "helper.h"			// for helper functions
#include <cmath>

// defines
// NOTE: you can change this value as per your requirement
// #define TILE_SIZE	100		// size of the tile for blocking
#define UNROLL_FACTOR 4                  
#define COMB_UNROLL_FACTOR 4    
#define TILE_SIZE 64 

/**
 * @brief 		Performs matrix multiplication of two matrices.
 * @param 		A 			pointer to the first matrix
 * @param 		B 			pointer to the second matrix
 * @param 		C 			pointer to the resultant matrix
 * @param 		size 		dimension of the matrices
 */

void print_matrix(double *C,int size){
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%f ",C[i*size + j]);	
		}
		printf("\n");	
	}
}

void naive_mat_mul(double *A, double *B, double *C, int size) {
	#ifndef NOT_NAIVE

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				C[i * size + j] += A[i * size + k] * B[k * size + j];
			}
		}
	}

	#endif
}

/**
 * @brief 		Task 1A: Performs matrix multiplication of two matrices using loop optimization.
 * @param 		A 			pointer to the first matrix
 * @param 		B 			pointer to the second matrix
 * @param 		C 			pointer to the resultant matrix
 * @param 		size 		dimension of the matrices
 */
void loop_opt_mat_mul(double *A, double *B, double *C, int size){

	#ifdef OPTIMIZE_LOOP_OPT

	int size_unroll_aligned = size - (size % UNROLL_FACTOR);
		
	double sum;

	for (int i = 0; i < size; i++) {
	for (int k = 0; k < size; k++) {

			double a_temp = A[i * size + k];

			for (int j = 0; j < size_unroll_aligned; j += UNROLL_FACTOR) {
				C[i * size + (j)] += a_temp * B[ k * size + (j)];
				C[i * size + (j + 1)] += a_temp * B[ k * size + (j + 1)];
				C[i * size + (j + 2)] += a_temp * B[ k* size + (j + 2)];
				C[i * size + (j + 3)] += a_temp * B[ k* size + (j + 3)];
				// C[i * size + (j + 4)] += a_temp * B[ k* size + (j + 4)];
				// C[i * size + (j + 5)] += a_temp * B[ k* size + (j + 5)];
				// C[i * size + (j + 6)] += a_temp * B[ k* size + (j + 6)];
				// C[i * size + (j + 7)] += a_temp * B[ k* size + (j + 7)];
			}

			for (int j = size_unroll_aligned; j < size; j++) {
				C[i * size + (j)] += a_temp * B[ k * size + (j)];
			}
		}
	}

	#endif
}

/**
 * @brief 		Task 1B: Performs matrix multiplication of two matrices using tiling.
 * @param 		A 			pointer to the first matrix
 * @param 		B 			pointer to the second matrix
 * @param 		C 			pointer to the resultant matrix
 * @param 		size 		dimension of the matrices
 * @param 		tile_size 	size of the tile
 * @note 		The tile size should be a multiple of the dimension of the matrices.
 * 				For example, if the dimension is 1024, then the tile size can be 32, 64, 128, etc.
 * 				You can assume that the matrices are square matrices.
*/
void tile_mat_mul(double *A, double *B, double *C, int size, int tile_size) {

	#ifdef OPTIMIZE_TILING
	double sum;
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
    for (int i = 0; i < size; i+=tile_size) {
        for (int j = 0; j < size; j+=tile_size) {
            for (int k = 0; k < size; k+=tile_size) {
                // now we will peform multiplication of tiles
                for(int tilei=i;tilei<i+tile_size && tilei<size;tilei++){
                    for(int tilej=j;tilej<j+tile_size && tilej<size;tilej++){
                        double sum=C[tilei * size + tilej];
                        for(int tilek=k;tilek<k+tile_size && tilek<size;tilek++){
                            sum += A[tilei * size + tilek] * B[tilek * size + tilej];
                        }       
                        C[tilei * size + tilej]=sum;
                    }
                }
            }
        }
    }

	#endif

//-------------------------------------------------------------------------------------------------------------------------------------------
    
}

/**
 * @brief 		Task 1C: Performs matrix multiplication of two matrices using SIMD instructions.
 * @param 		A 			pointer to the first matrix
 * @param 		B 			pointer to the second matrix
 * @param 		C 			pointer to the resultant matrix
 * @param 		size 		dimension of the matrices
 * @note 		You can assume that the matrices are square matrices.
*/
void simd_mat_mul(double *A, double *B, double *C, int size) {
	#ifdef OPTIMIZE_SIMD

	__m512d A_r8;
	__m512d B_r8;
	__m512d C_r8;


	int size8 = size - (size % 8);
	int j;


	for(int k = 0; k < size; k++){

		for(int i = 0; i < size; i++){

			A_r8  = _mm512_set1_pd(A[i * size + k]);
			j = 0;

			for(; j < size8; j += 8){
				B_r8 = _mm512_loadu_pd(&(B[k * size + j]));
				C_r8 = _mm512_loadu_pd(&(C[i * size + j]));

				C_r8 = _mm512_fmadd_pd(A_r8, B_r8, C_r8);

				_mm512_storeu_pd(&(C[i * size + j]), C_r8);
			}

			// leftover elements
			for(; j < size; j++) C[i * size + j] += A_r8[0] * B[k * size + j];
		}

	}

	#endif
}



/**
 * @brief 		Task 1D: Performs matrix multiplication of two matrices using combination of tiling/SIMD/loop optimization.
 * @param 		A 			pointer to the first matrix
 * @param 		B 			pointer to the second matrix
 * @param 		C 			pointer to the resultant matrix
 * @param 		size 		dimension of the matrices
 * @param 		tile_size 	size of the tile
 * @note 		The tile size should be a multiple of the dimension of the matrices.
 * @note 		You can assume that the matrices are square matrices.
*/
void combination_mat_mul(double *A, double *B, double *C, int size, int tile_size) {

	// #ifdef OPTIMIZE_COMBINED

	__m512d C_r8;

	__m512d A1_r8;
	__m512d B1_r8;

	__m512d A2_r8;
	__m512d B2_r8;

	__m512d A3_r8;
	__m512d B3_r8;

	__m512d A4_r8;
	__m512d B4_r8;



	int size_unroll_aligned = size - (size % COMB_UNROLL_FACTOR);

	int size8 = size_unroll_aligned - (size_unroll_aligned % 8);
	int i,j,t,k;


	for(k = 0; k < size_unroll_aligned; k += COMB_UNROLL_FACTOR){

		i = 0;

		for(; i < size; i ++){

			A1_r8  = _mm512_set1_pd(A[i * size + k]);
			A2_r8  = _mm512_set1_pd(A[i * size + (k + 1)]);
			A3_r8  = _mm512_set1_pd(A[i * size + (k + 2)]);
			A4_r8  = _mm512_set1_pd(A[i * size + (k + 3)]);

			j = 0;

			for(; j < size8; j += 8){
				B1_r8 = _mm512_loadu_pd(&(B[k * size + j]));
				C_r8 = _mm512_loadu_pd(&(C[i * size + j]));

				C_r8 = _mm512_fmadd_pd(A1_r8, B1_r8, C_r8);

				B2_r8 = _mm512_loadu_pd(&(B[(k + 1) * size + j]));
				C_r8 = _mm512_fmadd_pd(A2_r8, B2_r8, C_r8);

				B3_r8 = _mm512_loadu_pd(&(B[(k + 2) * size + j]));
				C_r8 = _mm512_fmadd_pd(A3_r8, B3_r8, C_r8);

				B4_r8 = _mm512_loadu_pd(&(B[(k + 3) * size + j]));
				C_r8 = _mm512_fmadd_pd(A4_r8, B4_r8, C_r8);

				_mm512_storeu_pd(&(C[i * size + j]), C_r8);
			}

			t = j;

			// leftover elements
			for(; j < size; j ++){
				C[i * size + j] += A1_r8[0] * B[k * size + j];
			}

			j = t;

			// leftover elements
			for(; j < size; j ++){
				C[i * size + j] += A2_r8[0] * B[(k + 1) * size + j];
			}
		}
	}


	for(; k < size; k++){

		for(int i = 0; i < size; i++){

			A1_r8  = _mm512_set1_pd(A[i * size + k]);
			j = 0;

			for(; j < size8; j += 8){
				B1_r8 = _mm512_loadu_pd(&(B[k * size + j]));
				C_r8 = _mm512_loadu_pd(&(C[i * size + j]));

				C_r8 = _mm512_fmadd_pd(A1_r8, B1_r8, C_r8);

				_mm512_storeu_pd(&(C[i * size + j]), C_r8);
			}

			// leftover elements
			for(; j < size; j++) C[i * size + j] += A1_r8[0] * B[k * size + j];
		}

	}


	// #endif
}

bool areEqual(double a, double b, double epsilon = 1e-9) {
    return std::fabs(a - b) < epsilon;
}


bool is_array_same(double *C1, double *C2, int size){
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if(!areEqual(C1[i * size + j], C2[i * size + j])) {
				printf("NO, ARRAY NOT SAME\n");
				return false;}
		}
	}
	

	printf("YES, THE ARRAY ARE SAME!\n");

	return true;
}

void print_array(double *arr, int size, char* msg = ""){
	printf("%s", msg);

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%lf ", arr[i * size + j]);
		}
			printf("\n");
	}

	printf("\n");
}

// NOTE: DO NOT CHANGE ANYTHING BELOW THIS LINE
/**
 * @brief 		Main function
 * @param 		argc 		number of command line arguments
 * @param 		argv 		array of command line arguments
 * @return 		0 on success
 * @note 		DO NOT CHANGE THIS FUNCTION
 * 				DO NOT ADD OR REMOVE ANY COMMAND LINE ARGUMENTS
*/
int main(int argc, char **argv) {

	// argc = 2;

	// argv[1] = "2048";

	if ( argc <= 1 ) {
		printf("Usage: %s <matrix_dimension>\n", argv[0]);
		return 0;
	}

	else {
		int size = atoi(argv[1]);

		double *A = (double *)malloc(size * size * sizeof(double));
		double *B = (double *)malloc(size * size * sizeof(double));
		double *C = (double *)calloc(size * size, sizeof(double));

		// initialize random seed
		srand(time(NULL));

		// initialize matrices A and B with random values
		initialize_matrix(A, size, size);
		initialize_matrix(B, size, size);

		// perform normal matrix multiplication
		auto start = std::chrono::high_resolution_clock::now();
		naive_mat_mul(A, B, C, size);
		auto end = std::chrono::high_resolution_clock::now();
		auto time_naive_mat_mul = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		printf("Normal matrix multiplication took %ld ms to execute \n\n", time_naive_mat_mul);

		// print_array(A, size, "A :\n");
		// print_array(B, size, "B :\n");
		// print_array(C, size, "C :\n");

	#ifdef OPTIMIZE_LOOP_OPT
		// Task 1a: perform matrix multiplication with loop optimization
		double *C2 = (double *)calloc(size * size, sizeof(double));

		// initialize result matrix to 0
		initialize_result_matrix(C2, size, size);

		start = std::chrono::high_resolution_clock::now();
		loop_opt_mat_mul(A, B, C2, size);
		end = std::chrono::high_resolution_clock::now();
		auto time_loop_mat_mul = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		printf("Array same : %d\n", is_array_same(C, C2, size));

		printf("Loop optimized matrix multiplication took %ld ms to execute \n", time_loop_mat_mul);
		printf("Normalized performance: %f \n\n", (double)time_naive_mat_mul / time_loop_mat_mul);
	#endif

	#ifdef OPTIMIZE_TILING
		// Task 1b: perform matrix multiplication with tiling

		// initialize result matrix to 0
		initialize_result_matrix(C, size, size);
		start = std::chrono::high_resolution_clock::now();
		tile_mat_mul(A, B, C, size, TILE_SIZE);
		end = std::chrono::high_resolution_clock::now();
		auto time_tiling_mat_mul = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		printf("Tiling matrix multiplication took %ld ms to execute \n", time_tiling_mat_mul);
		printf("Normalized performance: %f \n\n", (double)time_naive_mat_mul / time_tiling_mat_mul);
	#endif

	#ifdef OPTIMIZE_SIMD

		double *C2 = (double *)calloc(size * size, sizeof(double));

		// Task 1c: perform matrix multiplication with SIMD instructions 
		// initialize result matrix to 0
		initialize_result_matrix(C2, size, size);

		start = std::chrono::high_resolution_clock::now();
		simd_mat_mul(A, B, C2, size);
		end = std::chrono::high_resolution_clock::now();
		auto time_simd_mat_mul = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		printf("Array same : %d\n", is_array_same(C, C2, size));

		printf("SIMD matrix multiplication took %ld ms to execute \n", time_simd_mat_mul);
		printf("Normalized performance: %f \n\n", (double)time_naive_mat_mul / time_simd_mat_mul);
	#endif

	#ifdef OPTIMIZE_COMBINED
		// Task 1d: perform matrix multiplication with combination of tiling, SIMD and loop optimization

		// Task 1a: perform matrix multiplication with loop optimization
		double *C2 = (double *)calloc(size * size, sizeof(double));

		// initialize result matrix to 0
		initialize_result_matrix(C2, size, size);

		start = std::chrono::high_resolution_clock::now();
		combination_mat_mul(A, B, C2, size, TILE_SIZE);
		end = std::chrono::high_resolution_clock::now();
		auto time_combination = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		is_array_same(C, C2, size);

		printf("Combined optimization matrix multiplication took %ld ms to execute \n", time_combination);
		printf("Normalized performance: %f \n\n", (double)time_naive_mat_mul / time_combination);
	#endif

		// free allocated memory
		free(A);
		free(B);
		free(C);

		return 0;
	}
}
