#ifndef MATLAPACK_H
#define MATLAPACK_H

#include "lapack_wrapper.h"
#include "mat.h"

template<size_t M, size_t N, class T>
void invert(mat<M,N,T>& matrix) {
	assert(M==N);
    static int ipiv[M];
	static T work[M];
	int lwork = M;
	int info;
    
	LAPACK::invert_general_matrix(M, matrix.a, M, ipiv, info);
}


template<size_t M, size_t N, class T>
void least_squares(mat<M,N,T>&matrix, vec<M,T>&rhs) {
    int info = 0;
    LAPACK::solve_least_squares('N', M, N, 1, matrix.a, M, rhs.v, max(M,N), info);
}
#endif
