#ifndef __VorticityBaseBase_h__
#define __VorticityBaseBase_h__

#include "Common.h"
#include "FluidModel.h"
#include "NonPressureForceBase.h"

namespace SPH
{
	/** \brief Base class for all vorticity methods.
	*/
	class VorticityBase : public NonPressureForceBase
	{
	protected:
		Real m_vorticityCoeff;

		virtual void initParameters();

	public:
		static int VORTICITY_COEFFICIENT;

		VorticityBase(FluidModel *model);
		virtual ~VorticityBase(void);
	};
}

#endif
