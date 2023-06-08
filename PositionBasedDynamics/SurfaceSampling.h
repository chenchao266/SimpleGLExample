#ifndef SURFACESAMPLING_H
#define SURFACESAMPLING_H

#include "PoissonDiskSampling.h"
#include "RegularTriangleSampling.h"
#include "RegularSampling2D.h"

namespace SPH
{
	enum SurfaceSamplingMode { PoissonDisk, RegularTriangle, Regular2D };
}

#endif // SURFACESAMPLING_H
