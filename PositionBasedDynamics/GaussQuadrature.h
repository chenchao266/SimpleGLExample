#ifndef __GaussQuadrature_h__
#define __GaussQuadrature_h__

#include "eigenheaders.h"

namespace SPH
{
	class GaussQuadrature
	{
	public:

		using Integrand = std::function<double(EigenVec3d const&)>;
		using Domain = Eigen::AlignedBox3d;

		static double integrate(Integrand integrand, Domain const& domain, unsigned int p);
		static void exportSamples(unsigned int p);
	};
}

#endif

