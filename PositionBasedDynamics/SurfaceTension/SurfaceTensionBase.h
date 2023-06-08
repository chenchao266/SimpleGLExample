#ifndef __SurfaceTensionBaseBase_h__
#define __SurfaceTensionBaseBase_h__

#include "Common.h"
#include "FluidModel.h"
#include "NonPressureForceBase.h"

namespace SPH
{
	/** \brief Base class for all surface tension methods.
	*/
	class SurfaceTensionBase : public NonPressureForceBase
	{
	protected:
		Real m_surfaceTension;
		Real m_surfaceTensionBoundary;

		virtual void initParameters();

	public:
		static int SURFACE_TENSION;
		static int SURFACE_TENSION_BOUNDARY;

		SurfaceTensionBase(FluidModel *model);
		virtual ~SurfaceTensionBase(void);
	};
}

#endif
