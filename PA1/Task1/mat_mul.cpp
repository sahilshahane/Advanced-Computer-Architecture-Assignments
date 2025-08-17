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
#include <immintrin.h>		// for AVX

#include "helper.h"			// for helper functions
#include <cmath>


// defines
// NOTE: you can change this value as per your requirement
#define TILE_SIZE	100		// size of the tile for blocking

/**
 * @brief 		Performs matrix multiplication of two matrices.
 * @param 		A 			pointer to the first matrix
 * @param 		B 			pointer to the second matrix
 * @param 		C 			pointer to the resultant matrix
 * @param 		size 		dimension of the matrices
 */
void naive_mat_mul(double *A, double *B, double *C, int size) {

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				C[i * size + j] += A[i * size + k] * B[k * size + j];

				// printf("%lf\n", (A[i * size + k] * B[k * size + j]));
			}

			// printf("%lf\n", C[i * size + j]);
		}
	}

	printf("\n");
}


/**
 * @brief 		Task 1A: Performs matrix multiplication of two matrices using loop optimization.
 * @param 		A 			pointer to the first matrix
 * @param 		B 			pointer to the second matrix
 * @param 		C 			pointer to the resultant matrix
 * @param 		size 		dimension of the matrices
 */
void loop_opt_mat_mul(double *A, double *B, double *C, int size){
//----------------------------------------------------- Write your code here ----------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------

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
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
    

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
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
	auto s = sizeof(double);

	// printf("double size : %d bytes\n", s);

	int size_hf = size >> 1;
	__m128d A_r;
	__m128d B_r;
	__m128d C_r;

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			for (int k = 0; k < size; k += 2) {
				A_r = _mm_loadu_pd(&(A[i * size + k]));
				B_r = _mm_set_pd(B[(k + 1) * size + j], B[k * size + j]);

				// printf("%lf %lf\n",A[i * size + k], B[i * size + k]);
				// printf("%lf %lf\n",A[i * size + (k + 1)], B[i * size + (k + 1)]);
				
				// printf("%lf %lf\n",A_r[1], B_r[1]);
				// printf("%lf %lf\n",A_r[0], B_r[0]);
				C_r = _mm_mul_pd(A_r, B_r);

				// printf("%lf\n", C_r[1]);
				// printf("%lf\n", C_r[0]);
		
				C[i * size + j] += C_r[0] + C_r[1];

				// printf("\n\n");
			}

			// printf("%lf\n", C[i * size + j]);
		}
	}

	printf("\n");

//-------------------------------------------------------------------------------------------------------------------------------------------
    
}

#ifdef OPTIMIZE_SIMD_256

void simd_mat_mul_256(double *A, double *B, double *C, int size) {
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
	auto s = sizeof(double);

	// printf("double size : %d bytes\n", s);

	int size_hf = size >> 1;
	__m256d A_r;
	__m256d B_r;
	__m256d C_r;

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			for (int k = 0; k < size; k += 4) {
				A_r = _mm256_loadu_pd(&(A[i * size + k]));
				B_r = _mm256_set_pd(
						B[(k + 3) * size + j],
						B[(k + 2) * size + j],
						B[(k + 1) * size + j],
						B[k * size + j]
					);

				// printf("%lfq %lf\n",A[i * size + k], B[i * size + k]);
				// printf("%lf %lf\n",A[i * size + (k + 1)], B[i * size + (k + 1)]);
				
				// printf("%lf %lf\n",A_r[1], B_r[1]);
				// printf("%lf %lf\n",A_r[0], B_r[0]);
				C_r = _mm256_mul_pd(A_r, B_r);

				// printf("%lf\n", C_r[1]);
				// printf("%lf\n", C_r[0]);
		
				C[i * size + j] += C_r[0] + C_r[1] + C_r[2] + C_r[3];

				// printf("\n\n");
			}

			// printf("%lf\n", C[i * size + j]);
		}
	}

	printf("\n");

//-------------------------------------------------------------------------------------------------------------------------------------------
    
}


#endif

#ifdef OPTIMIZE_SIMD_512
void simd_mat_mul_512(double *A, double *B, double *C, int size) {
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
	auto s = sizeof(double);

	// printf("double size : %d bytes\n", s);

	int size_hf = size >> 1;
	__m512d A_r;
	__m512d B_r;
	__m512d C_r;

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			for (int k = 0; k < size; k += 8) {
				A_r = _mm512_loadu_pd(&(A[i * size + k]));
				B_r = _mm512_set_pd(
						B[(k + 7) * size + j],
						B[(k + 6) * size + j],
						B[(k + 5) * size + j],
						B[(k + 4) * size + j],
						B[(k + 3) * size + j],
						B[(k + 2) * size + j],
						B[(k + 1) * size + j],
						B[k * size + j]
					);

				// printf("%lfq %lf\n",A[i * size + k], B[i * size + k]);
				// printf("%lf %lf\n",A[i * size + (k + 1)], B[i * size + (k + 1)]);
				
				// printf("%lf %lf\n",A_r[1], B_r[1]);
				// printf("%lf %lf\n",A_r[0], B_r[0]);
				C_r = _mm512_mul_pd(A_r, B_r);

				// printf("%lf\n", C_r[1]);
				// printf("%lf\n", C_r[0]);
		
				C[i * size + j] += C_r[0] + C_r[1] + C_r[2] + C_r[3] + C_r[4] + C_r[5] + C_r[6] + C_r[7];

				// printf("\n\n");
			}

			// printf("%lf\n", C[i * size + j]);
		}
	}

	printf("\n");

//-------------------------------------------------------------------------------------------------------------------------------------------
    
}

#endif


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
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
    
    
//-------------------------------------------------------------------------------------------------------------------------------------------
    
}

bool areEqual(double a, double b, double epsilon = 1e-9) {
    return std::fabs(a - b) < epsilon;
}


bool is_array_same(double *C1, double *C2, int size){
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if(!areEqual(C1[i * size + j], C2[i * size + j])) return false;
		}
	}

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

		// initialize result matrix to 0
		initialize_result_matrix(C, size, size);

		start = std::chrono::high_resolution_clock::now();
		loop_opt_mat_mul(A, B, C, size);
		end = std::chrono::high_resolution_clock::now();
		auto time_loop_mat_mul = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
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
		// Task 1c: perform matrix multiplication with SIMD instructions 
	
		double *C_simd = (double *)calloc(size * size, sizeof(double));

		// initialize result matrix to 0
		initialize_result_matrix(C_simd, size, size);

		start = std::chrono::high_resolution_clock::now();
		simd_mat_mul(A, B, C_simd, size);
		end = std::chrono::high_resolution_clock::now();
		auto time_simd_mat_mul = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		// print_array(C_simd, size, "Result arr :\n");

		// printf("Is Result Valid : %s\n", is_array_same(C, C_simd, size) ? "True" : "False");

		printf("SIMD matrix multiplication took %ld ms to execute \n", time_simd_mat_mul);
		printf("Normalized performance: %f \n\n", (double)time_naive_mat_mul / time_simd_mat_mul);
	#endif

	#ifdef OPTIMIZE_COMBINED
		// Task 1d: perform matrix multiplication with combination of tiling, SIMD and loop optimization

		// initialize result matrix to 0
		initialize_result_matrix(C, size, size);

		start = std::chrono::high_resolution_clock::now();
		combination_mat_mul(A, B, C, size, TILE_SIZE);
		end = std::chrono::high_resolution_clock::now();
		auto time_combination = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		printf("Combined optimization matrix multiplication took %ld ms to execute \n", time_combination);
		printf("Normalized performance: %f \n\n", (double)time_naive_mat_mul / time_combination);
	#endif


	// EXTRA CODE ~~~~~~~~~~~~~~~~~

	#ifdef OPTIMIZE_SIMD_256
		// Task 1c: perform matrix multiplication with SIMD instructions 
		// print_array(A, size, "A :\n");
		// print_array(B, size, "B :\n");
		// print_array(C, size, "C :\n");

		double *C_simd_256 = (double *)calloc(size * size, sizeof(double));

		// initialize result matrix to 0
		initialize_result_matrix(C_simd_256, size, size);

		start = std::chrono::high_resolution_clock::now();
		simd_mat_mul_256(A, B, C_simd_256, size);
		end = std::chrono::high_resolution_clock::now();
		auto time_simd_mat_mul = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		// print_array(C_simd_256, size, "Result arr :\n");

		// printf("Is Result Valid : %s\n", is_array_same(C, C_simd_256, size) ? "True" : "False");

		printf("SIMD matrix multiplication took %ld ms to execute \n", time_simd_mat_mul);
		printf("Normalized performance: %f \n\n", (double)time_naive_mat_mul / time_simd_mat_mul);
	#endif

	

	#ifdef OPTIMIZE_SIMD_512
		// Task 1c: perform matrix multiplication with SIMD instructions 
		// print_array(A, size, "A :\n");
		// print_array(B, size, "B :\n");
		// print_array(C, size, "C :\n");

		double *C_simd_512 = (double *)calloc(size * size, sizeof(double));

		// initialize result matrix to 0
		initialize_result_matrix(C_simd_512, size, size);

		start = std::chrono::high_resolution_clock::now();
		simd_mat_mul_512(A, B, C_simd_512, size);
		end = std::chrono::high_resolution_clock::now();
		auto time_simd_mat_mul = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		// print_array(C_simd_512, size, "Result arr :\n");

		// printf("Is Result Valid : %s\n", is_array_same(C, C_simd_512, size) ? "True" : "False");

		printf("SIMD matrix multiplication took %ld ms to execute \n", time_simd_mat_mul);
		printf("Normalized performance: %f \n\n", (double)time_naive_mat_mul / time_simd_mat_mul);
	#endif

	// ~`~~~~~~~~~~~~~~~~~~~~~~~



		// free allocated memory
		free(A);
		free(B);
		free(C);

		return 0;
	}
}


