#ifndef FILE_LAPACK_INTERFACE_H
#define FILE_LAPACK_INTERFACE_H

#include <iostream>
#include <string>
#include <vector>
#include <complex>

#include "vector.h"
#include "matrix.h"

typedef int integer;
typedef integer logical;
typedef float real;
typedef double doublereal;
typedef std::complex<float> singlecomplex;
typedef std::complex<double> doublecomplex;

// Windows SDK defines VOID in the file WinNT.h
#ifndef VOID
typedef void VOID;
#endif
typedef int ftnlen;
typedef int L_fp;  // ?

extern "C" {
#include <clapack.h>
}

namespace pep::bla {

	class T_Lapack { };
	static constexpr T_Lapack Lapack;

	// BLAS-1 functions:

	/*
		int daxpy_(integer *n, doublereal *da, doublereal *dx,
		integer *incx, doublereal *dy, integer *incy);
	*/
	// y += alpha x
	template <typename SX, typename SY>
	void AddVectorLapack (double alpha, VectorView<double,SX> x, VectorView<double,SY> y) {
		integer n = x.Size();
		integer incx = x.Dist();
		integer incy = y.Dist();
		int err = daxpy_ (&n, &alpha, &x(0),  &incx, &y(0), &incy);
		if (err != 0) {
			throw std::runtime_error(std::string("daxpy returned errcode "+std::to_string(err)));
		}
	}


	// BLAS-2 functions:

	// BLAS-3 functions:

	// int dgemm_ (char *transa, char *transb, integer *m, integer * n,
	// integer *k, doublereal *alpha, doublereal *a, integer *lda,
	// doublereal *b, integer *ldb, doublereal *beta, doublereal *c__,
	// integer *ldc);


	// c = a*b
	template <ORDERING OA, ORDERING OB>
	void MultMatMatLapack (MatrixView<double, OA> a, MatrixView<double, OB> b, MatrixView<double, ORDERING::ColMajor> c) {
		char transa_ = (OA == ORDERING::ColMajor) ? 'N' : 'T';
		char transb_ = (OB == ORDERING::ColMajor) ? 'N' : 'T';

		integer n = c.Rows();
		integer m = c.Cols();
		integer k = a.Cols();

		double alpha = 1.0;
		double beta = 0;
		integer lda = std::max(a.Dist(), 1ul);
		integer ldb = std::max(b.Dist(), 1ul);
		integer ldc = std::max(c.Dist(), 1ul);

		int err = dgemm_ (&transa_, &transb_, &n, &m, &k, &alpha, a.Data(), &lda, b.Data(), &ldb, &beta, c.Data(), &ldc);

		// if (err != 0)
		//   throw std::runtime_error(std::string("MultMatMat got error "+std::to_string(err)));
	}

	template <ORDERING OA, ORDERING OB>
	void MultMatMatLapack (MatrixView<double, OA> a, MatrixView<double, OB> b, MatrixView<double, ORDERING::RowMajor> c) {
		MultMatMatLapack(b.Transpose(), a.Transpose(), c.Transpose());
	}

	template <ORDERING ORD>
	class LapackLU {
		private:
		Matrix<double, ORD> a;
		std::vector<integer> ipiv;

		public:
		LapackLU (Matrix<double,ORD> _a) : a(std::move(_a)), ipiv(a.Rows()) {
			integer m = a.Rows();
			if (m == 0) {
				return;
			}
			integer n = a.Cols();
			integer lda = a.Dist();
			integer info;

			// int dgetrf_(integer *m, integer *n, doublereal *a,
			//             integer * lda, integer *ipiv, integer *info);

			dgetrf_(&n, &m, &a(0,0), &lda, &ipiv[0], &info);
		}

		// b overwritten with A^{-1} b
		void Solve (VectorView<double> b) {
			char transa = (ORD == ORDERING::ColMajor) ? 'N' : 'T';
			integer n = a.Rows();
			integer nrhs = 1;
			integer lda = a.Dist();
			integer ldb = b.Size();
			integer info;

			// int dgetrs_(char *trans, integer *n, integer *nrhs,
			//             doublereal *a, integer *lda, integer *ipiv,
			//             doublereal *b, integer *ldb, integer *info);

			dgetrs_(&transa, &n, &nrhs, a.Data(), &lda, (integer*)ipiv.data(), b.Data(), &ldb, &info);
		}

		Matrix<double,ORD> Inverse() && {
			double hwork;
			integer lwork = -1;
			integer n = a.Rows();
			integer lda = a.Dist();
			integer info;

			// int dgetri_(integer *n, doublereal *a, integer *lda,
			//             integer *ipiv, doublereal *work, integer *lwork,
			//             integer *info);

			// query work-size
			dgetri_(&n, &a(0,0), &lda, ipiv.data(), &hwork, &lwork, &info);
			lwork = integer(hwork);
			std::vector<double> work(lwork);
			dgetri_(&n, &a(0,0), &lda, ipiv.data(), &work[0], &lwork, &info);
			return std::move(a);
		}

		// Matrix<double,ORD> LFactor() const { ... }
		// Matrix<double,ORD> UFactor() const { ... }
		// Matrix<double,ORD> PFactor() const { ... }
	};
}

#endif
