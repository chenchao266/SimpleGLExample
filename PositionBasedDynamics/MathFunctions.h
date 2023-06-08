#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include "Common.h"

// ------------------------------------------------------------------------------------
namespace PBD
{
	class MathFunctions
	{
 	public:
	 
		static Real infNorm(const Matrix3r &A);
		static Real oneNorm(const Matrix3r &A);

		static void eigenDecomposition(const Matrix3r &A,
			Matrix3r &eigenVecs,
			Vector3r &eigenVals);

		static void polarDecomposition(const Matrix3r &A,
			Matrix3r &R,
			Matrix3r &U,
			Matrix3r &D);

		static void polarDecompositionStable(const Matrix3r &M,
			const Real tolerance,
			Matrix3r &R);

		static void svdWithInversionHandling(const Matrix3r &A,
			Vector3r &sigma,
			Matrix3r &U,
			Matrix3r &VT);

		static Real cotTheta(const Vector3r &v, const Vector3r &w);

		/** Computes the cross product matrix of a vector.
		 * @param  v		input vector
		 * @param  v_hat	resulting cross product matrix
		 */	
		static void crossProductMatrix(const Vector3r &v, Matrix3r &v_hat);

		/** Implementation of the paper: \n
		 * Matthias M黮ler, Jan Bender, Nuttapong Chentanez and Miles Macklin, 
		 * "A Robust Method to Extract the Rotational Part of Deformations", 
		 * ACM SIGGRAPH Motion in Games, 2016
		 */
		static void extractRotation(const Matrix3r &A, Quaternionr &q, const unsigned int maxIter);
		static void pseudoInverse(const Matrix3r &a, Matrix3r &res);
  
		static void jacobiRotate(Matrix3r &A, Matrix3r &R, int p, int q);

		/** Returns two orthogonal vectors to vec which are also orthogonal to each other.
		*/
		static void getOrthogonalVectors(const Vector3r &vec, Vector3r &x, Vector3r &y);

		/** computes the APD of 8 deformation gradients. (Alg. 3 from the paper: Kugelstadt et al. "Fast Corotated FEM using Operator Splitting", CGF 2018)
		*/
		static void APD_Newton(const Matrix3r& F, Quaternionr& q);
	};
}

#endif