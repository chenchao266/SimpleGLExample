/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BT_IDEBUG_DRAW__H
#define BT_IDEBUG_DRAW__H

#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/Color.h"
#include "Bullet3Common/b3Transform.h"

///The btIDebugDraw interface class allows hooking up a debug renderer to visually debug simulations.
///Typical use case: create a debug drawer object, and assign it to a btCollisionWorld or btDynamicsWorld using setDebugDrawer and call debugDrawWorld.
///A class that implements the btIDebugDraw interface has to implement the drawLine method at a minimum.
///For color arguments the X,Y,Z components refer to Red, Green and Blue each in the range [0..1]
class btIDebugDraw
{
public:
    B3_ATTRIBUTE_ALIGNED16(struct)
	DefaultColors
	{
		b3Vector3 m_activeObject;
		b3Vector3 m_deactivatedObject;
		b3Vector3 m_wantsDeactivationObject;
		b3Vector3 m_disabledDeactivationObject;
		b3Vector3 m_disabledSimulationObject;
		b3Vector3 m_aabb;
		b3Vector3 m_contactPoint;

		DefaultColors()
			: m_activeObject(1, 1, 1),
			  m_deactivatedObject(0, 1, 0),
			  m_wantsDeactivationObject(0, 1, 1),
			  m_disabledDeactivationObject(1, 0, 0),
			  m_disabledSimulationObject(1, 1, 0),
			  m_aabb(1, 0, 0),
			  m_contactPoint(1, 1, 0)
		{
		}
	};

	enum DebugDrawModes
	{
		DBG_NoDebug = 0,
		DBG_DrawWireframe = 1,
		DBG_DrawAabb = 2,
		DBG_DrawFeaturesText = 4,
		DBG_DrawContactPoints = 8,
		DBG_NoDeactivation = 16,
		DBG_NoHelpText = 32,
		DBG_DrawText = 64,
		DBG_ProfileTimings = 128,
		DBG_EnableSatComparison = 256,
		DBG_DisableBulletLCP = 512,
		DBG_EnableCCD = 1024,
		DBG_DrawConstraints = (1 << 11),
		DBG_DrawConstraintLimits = (1 << 12),
		DBG_FastWireframe = (1 << 13),
		DBG_DrawNormals = (1 << 14),
		DBG_DrawFrames = (1 << 15),
		DBG_MAX_DEBUG_DRAW_MODE
	};

	virtual ~btIDebugDraw(){};

	virtual DefaultColors getDefaultColors() const
	{
		DefaultColors colors;
		return colors;
	}
	///the default implementation for setDefaultColors has no effect. A derived class can implement it and store the colors.
	virtual void setDefaultColors(const DefaultColors& /*colors*/) {}

	virtual void drawLine(const b3Vector3& from, const b3Vector3& to, const Colorf& color) = 0;

	virtual void drawLine(const b3Vector3& from, const b3Vector3& to, const Colorf& fromColor, const Colorf& toColor)
	{
		(void)toColor;
		drawLine(from, to, fromColor);
	}

	virtual void drawSphere(b3Scalar radius, const b3Transform& transform, const Colorf& color)
	{
		b3Vector3 center = transform.getOrigin();
		b3Vector3 up = transform.getBasis().getColumn(1);
		b3Vector3 axis = transform.getBasis().getColumn(0);
		b3Scalar minTh = -SIMD_HALF_PI;
		b3Scalar maxTh = SIMD_HALF_PI;
		b3Scalar minPs = -SIMD_HALF_PI;
		b3Scalar maxPs = SIMD_HALF_PI;
		b3Scalar stepDegrees = 30.f;
		drawSpherePatch(center, up, axis, radius, minTh, maxTh, minPs, maxPs, color, stepDegrees, false);
		drawSpherePatch(center, up, -axis, radius, minTh, maxTh, minPs, maxPs, color, stepDegrees, false);
	}

	virtual void drawSphere(const b3Vector3& p, b3Scalar radius, const Colorf& color)
	{
		b3Transform tr;
		tr.setIdentity();
		tr.setOrigin(p);
		drawSphere(radius, tr, color);
	}

	virtual void drawTriangle(const b3Vector3& v0, const b3Vector3& v1, const b3Vector3& v2, const b3Vector3& /*n0*/, const b3Vector3& /*n1*/, const b3Vector3& /*n2*/, const Colorf& color, b3Scalar alpha)
	{
		drawTriangle(v0, v1, v2, color, alpha);
	}
	virtual void drawTriangle(const b3Vector3& v0, const b3Vector3& v1, const b3Vector3& v2, const Colorf& color, b3Scalar /*alpha*/)
	{
		drawLine(v0, v1, color);
		drawLine(v1, v2, color);
		drawLine(v2, v0, color);
	}

	virtual void drawContactPoint(const b3Vector3& PointOnB, const b3Vector3& normalOnB, b3Scalar distance, int lifeTime, const Colorf& color) = 0;

	virtual void reportErrorWarning(const char* warningString) = 0;

	virtual void draw3dText(const b3Vector3& location, const char* textString) = 0;

	virtual void setDebugMode(int debugMode) = 0;

	virtual int getDebugMode() const = 0;

	virtual void drawAabb(const b3Vector3& from, const b3Vector3& to, const Colorf& color)
	{
		b3Vector3 halfExtents = (to - from) * 0.5f;
		b3Vector3 center = (to + from) * 0.5f;
		int i, j;

		b3Vector3 edgecoord(1.f, 1.f, 1.f), pa, pb;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 3; j++)
			{
				pa = b3Vector3(edgecoord[0] * halfExtents[0], edgecoord[1] * halfExtents[1],
							   edgecoord[2] * halfExtents[2]);
				pa += center;

				int othercoord = j % 3;
				edgecoord[othercoord] *= -1.f;
				pb = b3Vector3(edgecoord[0] * halfExtents[0], edgecoord[1] * halfExtents[1],
							   edgecoord[2] * halfExtents[2]);
				pb += center;

				drawLine(pa, pb, color);
			}
			edgecoord = b3Vector3(-1.f, -1.f, -1.f);
			if (i < 3)
				edgecoord[i] *= -1.f;
		}
	}
	virtual void drawTransform(const b3Transform& transform, b3Scalar orthoLen)
	{
		b3Vector3 start = transform.getOrigin();
		drawLine(start, start + transform.getBasis() * b3Vector3(orthoLen, 0, 0), Colorf(b3Scalar(1.), b3Scalar(0.3), b3Scalar(0.3)));
		drawLine(start, start + transform.getBasis() * b3Vector3(0, orthoLen, 0), Colorf(b3Scalar(0.3), b3Scalar(1.), b3Scalar(0.3)));
		drawLine(start, start + transform.getBasis() * b3Vector3(0, 0, orthoLen), Colorf(b3Scalar(0.3), b3Scalar(0.3), b3Scalar(1.)));
	}

	virtual void drawArc(const b3Vector3& center, const b3Vector3& normal, const b3Vector3& axis, b3Scalar radiusA, b3Scalar radiusB, b3Scalar minAngle, b3Scalar maxAngle,
						 const Colorf& color, bool drawSect, b3Scalar stepDegrees = b3Scalar(10.f))
	{
		const b3Vector3& vx = axis;
		b3Vector3 vy = normal.cross(axis);
		b3Scalar step = stepDegrees * SIMD_RADS_PER_DEG;
		int nSteps = (int)b3Fabs((maxAngle - minAngle) / step);
		if (!nSteps) nSteps = 1;
		b3Vector3 prev = center + radiusA * vx * b3Cos(minAngle) + radiusB * vy * b3Sin(minAngle);
		if (drawSect)
		{
			drawLine(center, prev, color);
		}
		for (int i = 1; i <= nSteps; i++)
		{
			b3Scalar angle = minAngle + (maxAngle - minAngle) * b3Scalar(i) / b3Scalar(nSteps);
			b3Vector3 next = center + radiusA * vx * b3Cos(angle) + radiusB * vy * b3Sin(angle);
			drawLine(prev, next, color);
			prev = next;
		}
		if (drawSect)
		{
			drawLine(center, prev, color);
		}
	}
	virtual void drawSpherePatch(const b3Vector3& center, const b3Vector3& up, const b3Vector3& axis, b3Scalar radius,
								 b3Scalar minTh, b3Scalar maxTh, b3Scalar minPs, b3Scalar maxPs, const Colorf& color, b3Scalar stepDegrees = b3Scalar(10.f), bool drawCenter = true)
	{
		b3Vector3 vA[74];
		b3Vector3 vB[74];
		b3Vector3 *pvA = vA, *pvB = vB, *pT;
		b3Vector3 npole = center + up * radius;
		b3Vector3 spole = center - up * radius;
		b3Vector3 arcStart;
		b3Scalar step = stepDegrees * SIMD_RADS_PER_DEG;
		const b3Vector3& kv = up;
		const b3Vector3& iv = axis;
		b3Vector3 jv = kv.cross(iv);
		bool drawN = false;
		bool drawS = false;
		if (minTh <= -SIMD_HALF_PI)
		{
			minTh = -SIMD_HALF_PI + step;
			drawN = true;
		}
		if (maxTh >= SIMD_HALF_PI)
		{
			maxTh = SIMD_HALF_PI - step;
			drawS = true;
		}
		if (minTh > maxTh)
		{
			minTh = -SIMD_HALF_PI + step;
			maxTh = SIMD_HALF_PI - step;
			drawN = drawS = true;
		}
		int n_hor = (int)((maxTh - minTh) / step) + 1;
		if (n_hor < 2) n_hor = 2;
		b3Scalar step_h = (maxTh - minTh) / b3Scalar(n_hor - 1);
		bool isClosed = false;
		if (minPs > maxPs)
		{
			minPs = -SIMD_PI + step;
			maxPs = SIMD_PI;
			isClosed = true;
		}
		else if ((maxPs - minPs) >= SIMD_PI * b3Scalar(2.f))
		{
			isClosed = true;
		}
		else
		{
			isClosed = false;
		}
		int n_vert = (int)((maxPs - minPs) / step) + 1;
		if (n_vert < 2) n_vert = 2;
		b3Scalar step_v = (maxPs - minPs) / b3Scalar(n_vert - 1);
		for (int i = 0; i < n_hor; i++)
		{
			b3Scalar th = minTh + b3Scalar(i) * step_h;
			b3Scalar sth = radius * b3Sin(th);
			b3Scalar cth = radius * b3Cos(th);
			for (int j = 0; j < n_vert; j++)
			{
				b3Scalar psi = minPs + b3Scalar(j) * step_v;
				b3Scalar sps = b3Sin(psi);
				b3Scalar cps = b3Cos(psi);
				pvB[j] = center + cth * cps * iv + cth * sps * jv + sth * kv;
				if (i)
				{
					drawLine(pvA[j], pvB[j], color);
				}
				else if (drawS)
				{
					drawLine(spole, pvB[j], color);
				}
				if (j)
				{
					drawLine(pvB[j - 1], pvB[j], color);
				}
				else
				{
					arcStart = pvB[j];
				}
				if ((i == (n_hor - 1)) && drawN)
				{
					drawLine(npole, pvB[j], color);
				}

				if (drawCenter)
				{
					if (isClosed)
					{
						if (j == (n_vert - 1))
						{
							drawLine(arcStart, pvB[j], color);
						}
					}
					else
					{
						if (((!i) || (i == (n_hor - 1))) && ((!j) || (j == (n_vert - 1))))
						{
							drawLine(center, pvB[j], color);
						}
					}
				}
			}
			pT = pvA;
			pvA = pvB;
			pvB = pT;
		}
	}

	virtual void drawBox(const b3Vector3& bbMin, const b3Vector3& bbMax, const Colorf& color)
	{
		drawLine(b3Vector3(bbMin[0], bbMin[1], bbMin[2]), b3Vector3(bbMax[0], bbMin[1], bbMin[2]), color);
		drawLine(b3Vector3(bbMax[0], bbMin[1], bbMin[2]), b3Vector3(bbMax[0], bbMax[1], bbMin[2]), color);
		drawLine(b3Vector3(bbMax[0], bbMax[1], bbMin[2]), b3Vector3(bbMin[0], bbMax[1], bbMin[2]), color);
		drawLine(b3Vector3(bbMin[0], bbMax[1], bbMin[2]), b3Vector3(bbMin[0], bbMin[1], bbMin[2]), color);
		drawLine(b3Vector3(bbMin[0], bbMin[1], bbMin[2]), b3Vector3(bbMin[0], bbMin[1], bbMax[2]), color);
		drawLine(b3Vector3(bbMax[0], bbMin[1], bbMin[2]), b3Vector3(bbMax[0], bbMin[1], bbMax[2]), color);
		drawLine(b3Vector3(bbMax[0], bbMax[1], bbMin[2]), b3Vector3(bbMax[0], bbMax[1], bbMax[2]), color);
		drawLine(b3Vector3(bbMin[0], bbMax[1], bbMin[2]), b3Vector3(bbMin[0], bbMax[1], bbMax[2]), color);
		drawLine(b3Vector3(bbMin[0], bbMin[1], bbMax[2]), b3Vector3(bbMax[0], bbMin[1], bbMax[2]), color);
		drawLine(b3Vector3(bbMax[0], bbMin[1], bbMax[2]), b3Vector3(bbMax[0], bbMax[1], bbMax[2]), color);
		drawLine(b3Vector3(bbMax[0], bbMax[1], bbMax[2]), b3Vector3(bbMin[0], bbMax[1], bbMax[2]), color);
		drawLine(b3Vector3(bbMin[0], bbMax[1], bbMax[2]), b3Vector3(bbMin[0], bbMin[1], bbMax[2]), color);
	}
	virtual void drawBox(const b3Vector3& bbMin, const b3Vector3& bbMax, const b3Transform& trans, const Colorf& color)
	{
		drawLine(trans * b3Vector3(bbMin[0], bbMin[1], bbMin[2]), trans * b3Vector3(bbMax[0], bbMin[1], bbMin[2]), color);
		drawLine(trans * b3Vector3(bbMax[0], bbMin[1], bbMin[2]), trans * b3Vector3(bbMax[0], bbMax[1], bbMin[2]), color);
		drawLine(trans * b3Vector3(bbMax[0], bbMax[1], bbMin[2]), trans * b3Vector3(bbMin[0], bbMax[1], bbMin[2]), color);
		drawLine(trans * b3Vector3(bbMin[0], bbMax[1], bbMin[2]), trans * b3Vector3(bbMin[0], bbMin[1], bbMin[2]), color);
		drawLine(trans * b3Vector3(bbMin[0], bbMin[1], bbMin[2]), trans * b3Vector3(bbMin[0], bbMin[1], bbMax[2]), color);
		drawLine(trans * b3Vector3(bbMax[0], bbMin[1], bbMin[2]), trans * b3Vector3(bbMax[0], bbMin[1], bbMax[2]), color);
		drawLine(trans * b3Vector3(bbMax[0], bbMax[1], bbMin[2]), trans * b3Vector3(bbMax[0], bbMax[1], bbMax[2]), color);
		drawLine(trans * b3Vector3(bbMin[0], bbMax[1], bbMin[2]), trans * b3Vector3(bbMin[0], bbMax[1], bbMax[2]), color);
		drawLine(trans * b3Vector3(bbMin[0], bbMin[1], bbMax[2]), trans * b3Vector3(bbMax[0], bbMin[1], bbMax[2]), color);
		drawLine(trans * b3Vector3(bbMax[0], bbMin[1], bbMax[2]), trans * b3Vector3(bbMax[0], bbMax[1], bbMax[2]), color);
		drawLine(trans * b3Vector3(bbMax[0], bbMax[1], bbMax[2]), trans * b3Vector3(bbMin[0], bbMax[1], bbMax[2]), color);
		drawLine(trans * b3Vector3(bbMin[0], bbMax[1], bbMax[2]), trans * b3Vector3(bbMin[0], bbMin[1], bbMax[2]), color);
	}

	virtual void drawCapsule(b3Scalar radius, b3Scalar halfHeight, int upAxis, const b3Transform& transform, const Colorf& color)
	{
		int stepDegrees = 30;

		b3Vector3 capStart(0.f, 0.f, 0.f);
		capStart[upAxis] = -halfHeight;

		b3Vector3 capEnd(0.f, 0.f, 0.f);
		capEnd[upAxis] = halfHeight;

		// Draw the ends
		{
			b3Transform childTransform = transform;
			childTransform.getOrigin() = transform * capStart;
			{
				b3Vector3 center = childTransform.getOrigin();
				b3Vector3 up = childTransform.getBasis().getColumn((upAxis + 1) % 3);
				b3Vector3 axis = -childTransform.getBasis().getColumn(upAxis);
				b3Scalar minTh = -SIMD_HALF_PI;
				b3Scalar maxTh = SIMD_HALF_PI;
				b3Scalar minPs = -SIMD_HALF_PI;
				b3Scalar maxPs = SIMD_HALF_PI;

				drawSpherePatch(center, up, axis, radius, minTh, maxTh, minPs, maxPs, color, b3Scalar(stepDegrees), false);
			}
		}

		{
			b3Transform childTransform = transform;
			childTransform.getOrigin() = transform * capEnd;
			{
				b3Vector3 center = childTransform.getOrigin();
				b3Vector3 up = childTransform.getBasis().getColumn((upAxis + 1) % 3);
				b3Vector3 axis = childTransform.getBasis().getColumn(upAxis);
				b3Scalar minTh = -SIMD_HALF_PI;
				b3Scalar maxTh = SIMD_HALF_PI;
				b3Scalar minPs = -SIMD_HALF_PI;
				b3Scalar maxPs = SIMD_HALF_PI;
				drawSpherePatch(center, up, axis, radius, minTh, maxTh, minPs, maxPs, color, b3Scalar(stepDegrees), false);
			}
		}

		// Draw some additional lines
		b3Vector3 start = transform.getOrigin();

		for (int i = 0; i < 360; i += stepDegrees)
		{
			capEnd[(upAxis + 1) % 3] = capStart[(upAxis + 1) % 3] = b3Sin(b3Scalar(i) * SIMD_RADS_PER_DEG) * radius;
			capEnd[(upAxis + 2) % 3] = capStart[(upAxis + 2) % 3] = b3Cos(b3Scalar(i) * SIMD_RADS_PER_DEG) * radius;
			drawLine(start + transform.getBasis() * capStart, start + transform.getBasis() * capEnd, color);
		}
	}

	virtual void drawCylinder(b3Scalar radius, b3Scalar halfHeight, int upAxis, const b3Transform& transform, const Colorf& color)
	{
		b3Vector3 start = transform.getOrigin();
		b3Vector3 offsetHeight(0, 0, 0);
		offsetHeight[upAxis] = halfHeight;
		int stepDegrees = 30;
		b3Vector3 capStart(0.f, 0.f, 0.f);
		capStart[upAxis] = -halfHeight;
		b3Vector3 capEnd(0.f, 0.f, 0.f);
		capEnd[upAxis] = halfHeight;

		for (int i = 0; i < 360; i += stepDegrees)
		{
			capEnd[(upAxis + 1) % 3] = capStart[(upAxis + 1) % 3] = b3Sin(b3Scalar(i) * SIMD_RADS_PER_DEG) * radius;
			capEnd[(upAxis + 2) % 3] = capStart[(upAxis + 2) % 3] = b3Cos(b3Scalar(i) * SIMD_RADS_PER_DEG) * radius;
			drawLine(start + transform.getBasis() * capStart, start + transform.getBasis() * capEnd, color);
		}
		// Drawing top and bottom caps of the cylinder
		b3Vector3 yaxis(0, 0, 0);
		yaxis[upAxis] = b3Scalar(1.0);
		b3Vector3 xaxis(0, 0, 0);
		xaxis[(upAxis + 1) % 3] = b3Scalar(1.0);
		drawArc(start - transform.getBasis() * (offsetHeight), transform.getBasis() * yaxis, transform.getBasis() * xaxis, radius, radius, 0, SIMD_2_PI, color, false, b3Scalar(10.0));
		drawArc(start + transform.getBasis() * (offsetHeight), transform.getBasis() * yaxis, transform.getBasis() * xaxis, radius, radius, 0, SIMD_2_PI, color, false, b3Scalar(10.0));
	}

	virtual void drawCone(b3Scalar radius, b3Scalar height, int upAxis, const b3Transform& transform, const Colorf& color)
	{
		int stepDegrees = 30;
		b3Vector3 start = transform.getOrigin();

		b3Vector3 offsetHeight(0, 0, 0);
		b3Scalar halfHeight = height * b3Scalar(0.5);
		offsetHeight[upAxis] = halfHeight;
		b3Vector3 offsetRadius(0, 0, 0);
		offsetRadius[(upAxis + 1) % 3] = radius;
		b3Vector3 offset2Radius(0, 0, 0);
		offset2Radius[(upAxis + 2) % 3] = radius;

		b3Vector3 capEnd(0.f, 0.f, 0.f);
		capEnd[upAxis] = -halfHeight;

		for (int i = 0; i < 360; i += stepDegrees)
		{
			capEnd[(upAxis + 1) % 3] = b3Sin(b3Scalar(i) * SIMD_RADS_PER_DEG) * radius;
			capEnd[(upAxis + 2) % 3] = b3Cos(b3Scalar(i) * SIMD_RADS_PER_DEG) * radius;
			drawLine(start + transform.getBasis() * (offsetHeight), start + transform.getBasis() * capEnd, color);
		}

		drawLine(start + transform.getBasis() * (offsetHeight), start + transform.getBasis() * (-offsetHeight + offsetRadius), color);
		drawLine(start + transform.getBasis() * (offsetHeight), start + transform.getBasis() * (-offsetHeight - offsetRadius), color);
		drawLine(start + transform.getBasis() * (offsetHeight), start + transform.getBasis() * (-offsetHeight + offset2Radius), color);
		drawLine(start + transform.getBasis() * (offsetHeight), start + transform.getBasis() * (-offsetHeight - offset2Radius), color);

		// Drawing the base of the cone
		b3Vector3 yaxis(0, 0, 0);
		yaxis[upAxis] = b3Scalar(1.0);
		b3Vector3 xaxis(0, 0, 0);
		xaxis[(upAxis + 1) % 3] = b3Scalar(1.0);
		drawArc(start - transform.getBasis() * (offsetHeight), transform.getBasis() * yaxis, transform.getBasis() * xaxis, radius, radius, 0, SIMD_2_PI, color, false, 10.0);
	}

	virtual void drawPlane(const b3Vector3& planeNormal, b3Scalar planeConst, const b3Transform& transform, const Colorf& color)
	{
		b3Vector3 planeOrigin = planeNormal * planeConst;
		b3Vector3 vec0, vec1;
		b3PlaneSpace1(planeNormal, vec0, vec1);
		b3Scalar vecLen = 100.f;
		b3Vector3 pt0 = planeOrigin + vec0 * vecLen;
		b3Vector3 pt1 = planeOrigin - vec0 * vecLen;
		b3Vector3 pt2 = planeOrigin + vec1 * vecLen;
		b3Vector3 pt3 = planeOrigin - vec1 * vecLen;
		drawLine(transform * pt0, transform * pt1, color);
		drawLine(transform * pt2, transform * pt3, color);
	}

	virtual void clearLines()
	{
	}

	virtual void flushLines()
	{
	}
};

#endif  //BT_IDEBUG_DRAW__H
