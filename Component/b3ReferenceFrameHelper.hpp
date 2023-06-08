/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2015 Google Inc. http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B3_REFERENCEFRAMEHELPER_H
#define B3_REFERENCEFRAMEHELPER_H

#include "LinearMath/b3Transform.h"
#include "LinearMath/b3Vector3.h"

class b3ReferenceFrameHelper
{
public:
	static b3Vector3 getPointWorldToLocal(const b3Transform& localObjectCenterOfMassTransform, const b3Vector3& point)
	{
		return localObjectCenterOfMassTransform.inverse() * point;  // transforms the point from the world frame into the local frame
	}

	static b3Vector3 getPointLocalToWorld(const b3Transform& localObjectCenterOfMassTransform, const b3Vector3& point)
	{
		return localObjectCenterOfMassTransform * point;  // transforms the point from the world frame into the local frame
	}

	static b3Vector3 getAxisWorldToLocal(const b3Transform& localObjectCenterOfMassTransform, const b3Vector3& axis)
	{
		b3Transform local1 = localObjectCenterOfMassTransform.inverse();  // transforms the axis from the local frame into the world frame
		b3Vector3 zero(0, 0, 0);
		local1.setOrigin(zero);
		return local1 * axis;
	}

	static b3Vector3 getAxisLocalToWorld(const b3Transform& localObjectCenterOfMassTransform, const b3Vector3& axis)
	{
		b3Transform local1 = localObjectCenterOfMassTransform;  // transforms the axis from the local frame into the world frame
		b3Vector3 zero(0, 0, 0);
		local1.setOrigin(zero);
		return local1 * axis;
	}

	static b3Transform getTransformWorldToLocal(const b3Transform& localObjectCenterOfMassTransform, const b3Transform& transform)
	{
		return localObjectCenterOfMassTransform.inverse() * transform;  // transforms the axis from the local frame into the world frame
	}

	static b3Transform getTransformLocalToWorld(const b3Transform& localObjectCenterOfMassTransform, const b3Transform& transform)
	{
		return localObjectCenterOfMassTransform * transform;  // transforms the axis from the local frame into the world frame
	}
};

#endif /* B3_REFERENCEFRAMEHELPER_H */
