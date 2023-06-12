
#pragma once

#include <array>
#include "eigenheaders.h"

namespace Discregrid
{

enum class NearestEntity
{
	VN0, VN1, VN2, EN0, EN1, EN2, FN
};

double point_triangle_sqdistance(EigenVec3d const& point, 
	std::array<EigenVec3d const*, 3> const& triangle,
	EigenVec3d* nearest_point = nullptr,
	NearestEntity* ne = nullptr);

}

