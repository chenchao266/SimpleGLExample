#ifndef __SimpleQuadrature_h__
#define __SimpleQuadrature_h__

#include <Eigen/Dense>
#include <vector>
#include "Common.h"

namespace SPH
{
	class SimpleQuadrature
	{
	public:
		using Integrand = std::function<double(EigenVec3d const&)>;
		using Domain = Eigen::AlignedBox3d;

		static std::vector<EigenVec3d> m_samplePoints;
		static double m_volume;

		static void determineSamplePointsInSphere(const double radius, unsigned int p);
		static void determineSamplePointsInCircle(const double radius, unsigned int p);
		static double integrate(Integrand integrand);
	};
}

#endif

