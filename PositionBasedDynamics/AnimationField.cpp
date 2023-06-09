﻿#include "AnimationField.h"
#include "SPHKernelsSPH.h"
#include <iostream>
#include "TimeManagerSPH.h"
#include "TimeStepSPH.h"
#include "FluidModel.h"
#include "SimulationSPH.h"
#include "extern/tinyexpr/tinyexpr.h"
#include "Logger.h"

using namespace SPH;

AnimationField::AnimationField(
	const std::string &particleFieldName,
	const Vector3r &pos, const Matrix3r & rotation, const Vector3r &scale,
	const std::string expression[3], const unsigned int type)
	: m_particleFieldName(particleFieldName)
	, m_x(pos)
	, m_rotation(rotation)
	, m_scale(scale)
	, m_type(type)
	, m_startTime(0)
	, m_endTime(REAL_MAX)
{
	for (int i = 0; i < 3; i++)
		m_expression[i] = expression[i];
}

AnimationField::~AnimationField(void)
{
}

void AnimationField::reset()
{
}

double getTime()
{
	return TimeManager::getCurrent()->getTime();
}

void AnimationField::step()
{
	Simulation *sim = Simulation::getCurrent();
	TimeManager *tm = TimeManager::getCurrent();
	const Real t = tm->getTime();
	const Real dt = tm->getTimeStepSize();

	if (t >= m_startTime && t <= m_endTime)
	{
		// animate particles		
		const unsigned int nModels = sim->numberOfFluidModels();
		for (unsigned int m = 0; m < nModels; m++)
		{
			FluidModel *fm = sim->getFluidModel(m);
			const unsigned int numParticles = fm->numActiveParticles();

			// find animated field
			const FieldDescription *particleField = nullptr;
			for (unsigned int j = 0; j < fm->numberOfFields(); j++)
			{
				const FieldDescription &field = fm->getField(j);
				if (field.name == m_particleFieldName)
				{
					particleField = &field;
					break;
				}
			}

			if (particleField == nullptr)
				continue;

			#pragma omp parallel for schedule(static) default(shared)
			for (int i = 0; i < (int)numParticles; i++)
			{
				const Vector3r &xi = fm->getPosition(i);
				const Vector3r &vi = fm->getVelocity(i);

				const EigenVec3d xi_double = xi.cast<double>();
				const EigenVec3d vi_double = vi.cast<double>();

				if (inShape(m_type, xi, m_x, m_rotation, m_scale))
				{
					Eigen::Map<Vector3r> value((Real*) particleField->getFct(i));
					const EigenVec3d value_double = Vector3r(value).cast<double>();

					const double t_double = static_cast<double>(t);
					const double dt_double = static_cast<double>(dt);

 					te_variable vars[] = { {"t", &t_double}, {"dt", &dt_double}, 
										   {"x", &xi_double[0]}, {"y", &xi_double[1]}, {"z", &xi_double[2]},
										   {"vx", &vi_double[0]}, {"vy", &vi_double[1]}, {"vz", &vi_double[2]},
										   {"valuex", &value_double[0]}, {"valuey", &value_double[1]}, {"valuez", &value_double[2]},
										 };
 					const int numVars = 11;
 					int err;

					//////////////////////////////////////////////////////////////////////////
					// v_x
					//////////////////////////////////////////////////////////////////////////
					if (m_expression[0] != "")
					{
						te_expr *expr_vx = te_compile(m_expression[0].c_str(), vars, numVars, &err);
						if (expr_vx)
							value[0] = static_cast<Real>(te_eval(expr_vx));
						te_free(expr_vx);

						if (err != 0)
							LOG_ERR << "Animation field: expression for x is wrong.";
					}

					//////////////////////////////////////////////////////////////////////////
					// v_y
					//////////////////////////////////////////////////////////////////////////
					if (m_expression[1] != "")
					{
						te_expr *expr_vy = te_compile(m_expression[1].c_str(), vars, numVars, &err);
						if (expr_vy)
							value[1] = static_cast<Real>(te_eval(expr_vy));
						te_free(expr_vy);

						if (err != 0)
							LOG_ERR << "Animation field: expression for y is wrong.";
					}

					//////////////////////////////////////////////////////////////////////////
					// v_z
					//////////////////////////////////////////////////////////////////////////
					if (m_expression[2] != "")
					{
						te_expr *expr_vz = te_compile(m_expression[2].c_str(), vars, numVars, &err);
						if (expr_vz)
							value[2] = static_cast<Real>(te_eval(expr_vz));
						te_free(expr_vz);

						if (err != 0)
							LOG_ERR << "Animation field: expression for z is wrong.";
					}
				}
			}
		}
	}
}

