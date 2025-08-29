#include "matrix_operation.h"
#include <immintrin.h>

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
    int a;

	for(int l = 0; l < k; l++) {
		for(int i = 0; i < n ; i++) {
			a = A(i,l);
			for (int j = 0 ; j < m ; j++) {
					C(i,j) +=  a * B(l,j);
			}
		}
	}
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
	int a, l;
	const int k_unroll_aligned = k - (k % UNROLL);

	for(int i = 0; i < n ; i++) {
		for (int j = 0 ; j < m; j++) {

			for(l = 0; l < k_unroll_aligned; l+= UNROLL) {
				C(i,j) +=  
				A(i,l) * B(l,j) 
				+ A(i,l + 1) * B(l + 1,j) 
				+ A(i,l + 2) * B(l + 2,j) 
				+ A(i,l + 3) * B(l + 3,j);
			}

			// leftover
			for (; l < k; l++) {
				C(i,j) +=  A(i,l) * B(l,j);
			}
		}
	}

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
    

//-------------------------------------------------------------------------------------------------------------------------------------------

    return C;
}

// Optimized matrix transpose
Matrix MatrixOperation::Transpose(const Matrix& A) {
	size_t rows = A.getRows();
	size_t cols = A.getCols();
	Matrix result(cols, rows);

	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
			result(j, i) = A(i, j);
		}
	}

	// Optimized transpose using blocking for better cache performance
	// This is a simple implementation, more advanced techniques can be applied
	// Write your code here and commnent the above code
//----------------------------------------------------- Write your code here ----------------------------------------------------------------
    

//-------------------------------------------------------------------------------------------------------------------------------------------

	
	return result;
}
