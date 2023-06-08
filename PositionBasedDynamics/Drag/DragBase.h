#ifndef __DragBase_h__
#define __DragBase_h__

#include "Common.h"
#include "FluidModel.h"
#include "NonPressureForceBase.h"

namespace SPH
{
	/** \brief Base class for all drag force methods.
	*/
	class DragBase : public NonPressureForceBase
	{
	protected:
		Real m_dragCoefficient;

		virtual void initParameters();

	public:
		static int DRAG_COEFFICIENT;

		DragBase(FluidModel *model);
		virtual ~DragBase(void);
	};
}

#endif
