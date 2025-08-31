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
#define COMB_UNROLL_FACTOR 5    

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
	#ifdef OPTIMIZE_SIMD_512
	__m512d A_r;
	__m512d B_r;
	__m512d C_r;

	double sum;

	int size8 = size - (size % 8);
	int k;

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			C_r = _mm512_setzero_pd();

			for(k = 0; k < size8; k += 8){
				A_r  = _mm512_loadu_pd(&(A[i * size + k]));
				
				B_r = _mm512_set_pd(	B[(k + 7) * size + j], 
										B[(k + 6) * size + j], 
										B[(k + 5) * size + j], 
										B[(k + 4) * size + j], 
										B[(k + 3) * size + j], 
										B[(k + 2) * size + j], 
										B[(k + 1) * size + j], 
										B[(k) * size + j]);

				C_r = _mm512_fmadd_pd(A_r, B_r, C_r);
			}	

			sum = _mm512_reduce_add_pd(C_r);

			for(; k < size; k++){
				sum += A[i * size + k] + B[k * size + j];
			}	

			C[i * size + j] = sum;
		}
	}

	#endif

	#ifdef OPTIMIZE_SIMD_256
	__m256d A_r;
	__m256d B_r;
	__m256d C_r;

	double sum;

	int size_aligned = size - (size % 4);
	int k;

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			C_r = _mm256_setzero_pd();

			for(k = 0; k < size_aligned; k += 4){
				A_r  = _mm256_loadu_pd(&(A[i * size + k]));
				
				B_r = _mm256_set_pd(	B[(k + 3) * size + j], 
										B[(k + 2) * size + j], 
										B[(k + 1) * size + j], 
										B[(k) * size + j]);

				C_r = _mm256_fmadd_pd(A_r, B_r, C_r);
			}	

			sum = C_r[0] + C_r[1] + C_r[2] + C_r[3];

			for(; k < size; k++){
				sum += A[i * size + k] + B[k * size + j];
			}	

			C[i * size + j] = sum;
		}
	}

	#endif

	#ifdef OPTIMIZE_SIMD_128
	__m128d A_r;
	__m128d B_r;
	__m128d C_r;

	double sum;

	int size_aligned = size - (size % 2);
	int k;

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			C_r = _mm_setzero_pd();

			for(k = 0; k < size_aligned; k += 2){
				A_r  = _mm_loadu_pd(&(A[i * size + k]));
				
				B_r = _mm_set_pd(	B[(k + 1) * size + j], 
										B[(k) * size + j]);

				C_r = _mm_fmadd_pd(A_r, B_r, C_r);
			}	

			sum = C_r[0] + C_r[1];

			for(; k < size; k++){
				sum += A[i * size + k] + B[k * size + j];
			}	

			C[i * size + j] = sum;
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

	#ifdef simd_tiling
	__m512d A_r8;
	__m512d B_r8;
	__m512d C_r8;
	for (int i = 0; i < size; i += tile_size)
	{
		for (int j = 0; j < size; j += tile_size)
		{
			for (int k = 0; k < size; k += tile_size)
			{
				// now we will peform multiplication of tiles using simd
				for (int tilei = i; tilei < i + tile_size && tilei < size; tilei++)
				{
					int tilej = j;
					for (; tilej + 7 < j + tile_size && tilej + 7 < size; tilej += 8)
					{
						C_r8 = _mm512_loadu_pd(&(C[tilei * size + tilej]));
						for (int tilek = k; tilek < k + tile_size && tilek < size; tilek++)
						{
							A_r8 = _mm512_set1_pd(A[tilei * size + tilek]);
							B_r8 = _mm512_loadu_pd(&(B[tilek * size + tilej]));
							C_r8 = _mm512_fmadd_pd(A_r8, B_r8, C_r8);
						}
						_mm512_storeu_pd(&(C[tilei * size + tilej]), C_r8);
					}
					// leftover columns
					for (; tilej < j + tile_size && tilej < size; tilej++)
					{
						double sum = C[tilei * size + tilej];
						for (int tilek = k; tilek < k + tile_size && tilek < size; tilek++)
							sum += A[tilei * size + tilek] * B[tilek * size + tilej];
						C[tilei * size + tilej] = sum;
					}
				}
			}
		}
	}
	#endif

	#ifdef tiling_loop_optmisation
	for (int i = 0; i < size; i += tile_size)
	{
		for (int j = 0; j < size; j += tile_size)
		{
			for (int k = 0; k < size; k += tile_size)
			{
				// now we will peform multiplication of tiles using loop_reordering and unrolling
				for (int tilei = i; tilei < i + tile_size && tilei < size; tilei++)
				{
					for (int tilek = k; tilek < k + tile_size && tilek < size; tilek++)
					{
						double a_temp = A[tilei * size + tilek];

						int tilej_end = (j + tile_size < size) ? j + tile_size : size;
						int size_unroll_aligned = tilej_end - ((tilej_end - j) % UNROLL_FACTOR);
						int tilej = j;
						// unrolling
						for (; tilej < size_unroll_aligned; tilej += UNROLL_FACTOR)
						{
							C[tilei * size + (tilej)] += a_temp * B[tilek * size + (tilej)];
							C[tilei * size + (tilej + 1)] += a_temp * B[tilek * size + (tilej + 1)];
							C[tilei * size + (tilej + 2)] += a_temp * B[tilek * size + (tilej + 2)];
							C[tilei * size + (tilej + 3)] += a_temp * B[tilek * size + (tilej + 3)];
						}
						for (; tilej < tilej_end; tilej++)
						{
							C[tilei * size + tilej] += a_temp * B[tilek * size + tilej];
						}
					}
				}
			}
		}
	}
	#endif

	#ifdef simd_loop_optimisation
	int size8 = size - (size % 32);
	int j;

	for (int k = 0; k < size; k++)
	{
		for (int i = 0; i < size; i++)
		{
			__m512d A_r8 = _mm512_set1_pd(A[i * size + k]);
			j = 0;
			int size_unroll_aligned = size8 - (size8 % UNROLL_FACTOR);

			// unrolled SIMD loop
			for (; j < size_unroll_aligned; j += UNROLL_FACTOR)
			{
				int simd_count = UNROLL_FACTOR / 8; // 8 doubles per __m512d
				for (int s = 0; s < simd_count; s++)
				{
					int idx = j + s * 8;
					__m512d B_r8 = _mm512_loadu_pd(&B[k * size + idx]);
					__m512d C_r8 = _mm512_loadu_pd(&C[i * size + idx]);
					C_r8 = _mm512_fmadd_pd(A_r8, B_r8, C_r8);
					_mm512_storeu_pd(&C[i * size + idx], C_r8);
				}
			}

			// leftover elements
			for (; j < size; j++)
				C[i * size + j] += A[i * size + k] * B[k * size + j];
		}
	}
	#endif

	#ifdef simd_loop_optimisation_tiling
	__m512d A_r8;
	__m512d B_r8;
	__m512d C_r8;
	for (int i = 0; i < size; i += tile_size)
	{
		for (int j = 0; j < size; j += tile_size)
		{
			for (int k = 0; k < size; k += tile_size)
			{
				// now we will perform unrolling and reordering and innermost loop we will do simd
				for (int tilei = i; tilei < i + tile_size && tilei < size; tilei++)
				{
					for (int tilek = k; tilek < k + tile_size && tilek < size; tilek++)
					{
						// unrolled SIMD loop
						double a_temp = A[tilei * size + tilek];
	                    A_r8 = _mm512_set1_pd(a_temp);

	                    int tilej = j;
						int tilej_end = (j + tile_size < size) ? j + tile_size : size;
	                    int size_unroll_aligned = tilej_end - ((tilej_end - j) % UNROLL_FACTOR);

	                    for (; tilej < size_unroll_aligned; tilej += UNROLL_FACTOR) {
	                        int simd_count = UNROLL_FACTOR / 8;
	                        for (int s = 0; s < simd_count; s++) {
	                            int idx = tilej + s * 8;
	                            B_r8 = _mm512_loadu_pd(&B[tilek * size + idx]);
	                            C_r8 = _mm512_loadu_pd(&C[tilei * size + idx]);
	                            C_r8 = _mm512_fmadd_pd(A_r8, B_r8, C_r8);
	                            _mm512_storeu_pd(&C[tilei * size + idx], C_r8);
	                        }
	                    }

						// leftover elements
						for (; tilej < tilej_end; tilej++)
							C[tilei * size + tilej] += A[tilei * size + tilek] * B[tilek * size + tilej];
					}
				}
			}
		}
	}
	#endif
}

bool areEqual(double a, double b, double epsilon = 1e-9) {
    return std::fabs(a - b) < epsilon;
}


bool is_array_same(double *C1, double *C2, int size){

	#ifdef DEBUG
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if(!areEqual(C1[i * size + j], C2[i * size + j])) {
				printf("NO, ARRAY NOT SAME\n");
				return false;}
		}
	}
	

	printf("YES, THE ARRAY ARE SAME!\n");
	#endif

	return true;
}

void print_array(double *arr, int size, char* msg = ""){

	#ifdef DEBUG

	printf("%s", msg);

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%lf ", arr[i * size + j]);
		}
			printf("\n");
	}

	printf("\n");


	#endif
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
		is_array_same(C, C2, size);

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
