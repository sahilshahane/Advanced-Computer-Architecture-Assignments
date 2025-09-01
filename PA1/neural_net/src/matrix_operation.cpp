#include "matrix_operation.h"
#include <immintrin.h>


bool is_matrix_same(Matrix A,Matrix B, string msg = ""){
	size_t n = A.getRows();
	size_t k = A.getCols();
	size_t m = B.getCols();

	if (A.getRows() != B.getRows() || A.getCols() != B.getCols()) {
        std::cout << "Error ( Matrix not same ) ( " << msg << " ): Dimention Mismatch" << std::endl;
		return false;
	}


    for(int i = 0; i < n; i++){
		for(int j = 0; j < m; j++){
			if(abs(A(i,j) - B(i,j)) > 1e-6){
				std::cout << "Error ( Matrix not same ) ( " << msg << " ): Mismatch at (" << i << ", " << j << "): " << A(i,j) << " != " << B(i,j) << std::endl;
                return false;
			}
		}
	}

    return true;
}

Matrix MatrixOperation::NaiveMatMul(const Matrix &A, const Matrix &B) {
	size_t n = A.getRows();
	size_t k = A.getCols();
	size_t m = B.getCols();

	if (k != B.getRows()) {
		throw std::invalid_argument("Matrix dimensions don't match for multiplication");
	}
	
	
	Matrix C(n,m);
	
	for(int i = 0; i < n ; i++) {
		for (int j = 0 ; j< m ; j++) {
			for(int l = 0; l < k; l++) {
				C(i,j) += A(i,l) * B(l,j);
			}
		}
	}
	
	return C;
}

// Loop reordered matrix multiplication (ikj order for better cache locality)
Matrix MatrixOperation::ReorderedMatMul(const Matrix& A, const Matrix& B) {
	size_t n = A.getRows();
	size_t k = A.getCols();
	size_t m = B.getCols();

	if (k != B.getRows()) {
		throw std::invalid_argument("Matrix dimensions don't match for multiplication");
	}
	
	
	Matrix C(n,m);
	
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
    element_t a;

	for(int i = 0; i < n ; i++) {
		for(int l = 0; l < k; l++) {
			a = A(i,l);

			for (int j = 0 ; j< m ; j++) {
				C(i,j) +=  a * B(l,j);
			}
		}
	}

	// is_matrix_same(C, NaiveMatMul(A, B), "ReorderedMatMul");
//-------------------------------------------------------------------------------------------------------------------------------------------


	return C;
}

// Loop unrolled matrix multiplication
Matrix MatrixOperation::UnrolledMatMul(const Matrix& A, const Matrix& B) {
	size_t n = A.getRows();
    size_t k = A.getCols();
    size_t m = B.getCols();

    if (k != B.getRows()) {
        throw std::invalid_argument("Matrix dimensions don't match for multiplication");
    }

    Matrix C(n, m);

    const int UNROLL = 4;
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
	int a, l, j, i;
	const int m_unroll_aligned = m - (m % UNROLL);

	for(i = 0; i < n ; i++) {
		for (j = 0 ; j < m_unroll_aligned; j+= UNROLL) {
			for(l = 0; l < k; l++) {
				C(i,j) += A(i,l) * B(l,j);
				C(i,j + 1) += A(i,l) * B(l,j + 1);
				C(i,j + 2) += A(i,l) * B(l,j + 2);
				C(i,j + 3) += A(i,l) * B(l,j + 3);
			}
		}

		for (; j < m; j++) {
			for(l = 0; l < k; l++) {
				C(i,j) += A(i,l) * B(l,j);
			}
		}
	}

	// is_matrix_same(C, NaiveMatMul(A, B), "UnrolledMatMul");
//-------------------------------------------------------------------------------------------------------------------------------------------

    return C;
}

// Tiled (blocked) matrix multiplication for cache efficiency
Matrix MatrixOperation::TiledMatMul(const Matrix& A, const Matrix& B) {
	size_t n = A.getRows();
    size_t k = A.getCols();
    size_t m = B.getCols();

    if (k != B.getRows()) {
        throw std::invalid_argument("Matrix dimensions don't match for multiplication");
    }

    Matrix C(n, m);
    const int T = 128;   // tile size
	int i_max = 0;
	int k_max = 0;
	int j_max = 0;
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
	int n_i = n, k_i = k, m_i = m;

  	for (int i = 0; i < n; i += T) {
		i_max = min(i + T, n_i);

        for (int j = 0; j < m; j+=T) {
			j_max = min(j + T, m_i);

            for (int l = 0; l < k; l+=T) {
				k_max = min(l + T, k_i);

                // now we will peform multiplication of tiles
                for(int tilei=i;tilei < i_max ;tilei++){
                    for(int tilej=j;tilej < j_max ;tilej++){
                        double sum = C(tilei, tilej);

                        for(int tileL=l;tileL< k_max;tileL++){
                            sum += A(tilei ,tileL) * B(tileL ,tilej);
                        }       
						
                        C(tilei, tilej) = sum;
                    }
                }
				
            }
        }
    }

	// is_matrix_same(C, NaiveMatMul(A, B), "TiledMatMul");

//-------------------------------------------------------------------------------------------------------------------------------------------

    return C;
}

// SIMD vectorized matrix multiplication (using AVX2)
Matrix MatrixOperation::VectorizedMatMul(const Matrix& A, const Matrix& B) {
	size_t n = A.getRows();
    size_t k = A.getCols();
    size_t m = B.getCols();

    if (k != B.getRows()) {
        throw std::invalid_argument("Matrix dimensions don't match for multiplication");
    }

    Matrix C(n, m);
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
	const int m_r_aligned =  m - (m % 8);

	__m512d A_r;
	__m512d B_r;
	__m512d C_r;

	double sum;

	int l, j;

	for(l = 0; l < k; l ++){

		for(int i = 0; i < n; i++){
			A_r  = _mm512_set1_pd(A(i,l));

			for(j = 0; j < m_r_aligned; j+=8){
				B_r = _mm512_loadu_pd(&(B(l, j)));
				C_r = _mm512_loadu_pd(&(C(i,j)));

				C_r = _mm512_fmadd_pd(A_r, B_r, C_r);

				_mm512_storeu_pd(&(C(i,j)), C_r);
			}	

			// leftover elements
			for(; j < m; j++){
				C(i, j) += A_r[0] * B(l,j);
			} 
		}
	}

	// is_matrix_same(C, NaiveMatMul(A, B), "VectorizedMatMul");
//-------------------------------------------------------------------------------------------------------------------------------------------

    return C;
}

// Optimized matrix transpose
Matrix MatrixOperation::Transpose(const Matrix& A) {
	size_t rows = A.getRows();
	size_t cols = A.getCols();
	Matrix result(cols, rows);

	// for (size_t i = 0; i < rows; ++i) {
	// 	for (size_t j = 0; j < cols; ++j) {
	// 		result(j, i) = A(i, j);
	// 	}
	// }

	// Optimized transpose using blocking for better cache performance
	// This is a simple implementation, more advanced techniques can be applied
	// Write your code here and commnent the above code
//----------------------------------------------------- Write your code here ----------------------------------------------------------------

	const int BLOCK_SIZE = 56; // You can adjust the block size for better performance

	for (size_t i = 0; i < rows; i += BLOCK_SIZE) {
		for (size_t j = 0; j < cols; j += BLOCK_SIZE) {
			for (size_t bi = i; bi < std::min(i + BLOCK_SIZE, rows); ++bi) {
				for (size_t bj = j; bj < std::min(j + BLOCK_SIZE, cols); ++bj) {
					result(bj, bi) = A(bi, bj);
				}
			}
		}
	}

//-------------------------------------------------------------------------------------------------------------------------------------------

	
	return result;
}
