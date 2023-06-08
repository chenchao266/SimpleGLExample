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
#include "btCollisionShape.h"
#include "Bullet3Common/btSerializer.h"

/*
  Make sure this dummy function never changes so that it
  can be used by probes that are checking whether the
  library is actually installed.
*/
extern "C"
{
	void btBulletCollisionProbe();

	void btBulletCollisionProbe() {}
}

void btCollisionShape::getBoundingSphere(b3Vector3& center, b3Scalar& radius) const
{
	b3Transform tr;
	tr.setIdentity();
	b3Vector3 aabbMin, aabbMax;

	getAabb(tr, aabbMin, aabbMax);

	radius = (aabbMax - aabbMin).length() * b3Scalar(0.5);
	center = (aabbMin + aabbMax) * b3Scalar(0.5);
}

b3Scalar btCollisionShape::getContactBreakingThreshold(b3Scalar defaultContactThreshold) const
{
	return getAngularMotionDisc() * defaultContactThreshold;
}

b3Scalar btCollisionShape::getAngularMotionDisc() const
{
	///@todo cache this value, to improve performance
	b3Vector3 center;
	b3Scalar disc;
	getBoundingSphere(center, disc);
	disc += (center).length();
	return disc;
}

void btCollisionShape::calculateTemporalAabb(const b3Transform& curTrans, const b3Vector3& linvel, const b3Vector3& angvel, b3Scalar timeStep, b3Vector3& temporalAabbMin, b3Vector3& temporalAabbMax) const
{
	//start with static aabb
	getAabb(curTrans, temporalAabbMin, temporalAabbMax);

	b3Scalar temporalAabbMaxx = temporalAabbMax.getX();
	b3Scalar temporalAabbMaxy = temporalAabbMax.getY();
	b3Scalar temporalAabbMaxz = temporalAabbMax.getZ();
	b3Scalar temporalAabbMinx = temporalAabbMin.getX();
	b3Scalar temporalAabbMiny = temporalAabbMin.getY();
	b3Scalar temporalAabbMinz = temporalAabbMin.getZ();

	// add linear motion
	b3Vector3 linMotion = linvel * timeStep;
	///@todo: simd would have a vector max/min operation, instead of per-element access
	if (linMotion.x > b3Scalar(0.))
		temporalAabbMaxx += linMotion.x;
	else
		temporalAabbMinx += linMotion.x;
	if (linMotion.y > b3Scalar(0.))
		temporalAabbMaxy += linMotion.y;
	else
		temporalAabbMiny += linMotion.y;
	if (linMotion.z > b3Scalar(0.))
		temporalAabbMaxz += linMotion.z;
	else
		temporalAabbMinz += linMotion.z;

	//add conservative angular motion
	b3Scalar angularMotion = angvel.length() * getAngularMotionDisc() * timeStep;
	b3Vector3 angularMotion3d(angularMotion, angularMotion, angularMotion);
	temporalAabbMin = b3Vector3(temporalAabbMinx, temporalAabbMiny, temporalAabbMinz);
	temporalAabbMax = b3Vector3(temporalAabbMaxx, temporalAabbMaxy, temporalAabbMaxz);

	temporalAabbMin -= angularMotion3d;
	temporalAabbMax += angularMotion3d;
}


///fills the dataBuffer and returns the struct name (and 0 on failure)
const char* btCollisionShape::serialize(void* dataBuffer, btSerializer* serializer) const
{
	btCollisionShapeData* shapeData = (btCollisionShapeData*)dataBuffer;
	char* name = (char*)serializer->findNameForPointer(this);
	shapeData->m_name = (char*)serializer->getUniquePointer(name);
	if (shapeData->m_name)
	{
		serializer->serializeName(name);
	}
	shapeData->m_shapeType = m_shapeType;

	// Fill padding with zeros to appease msan.
	memset(shapeData->m_padding, 0, sizeof(shapeData->m_padding));

	return "btCollisionShapeData";
}

void btCollisionShape::serializeSingleShape(btSerializer* serializer) const
{
	int len = calculateSerializeBufferSize();
	btChunk* chunk = serializer->allocate(len, 1);
	const char* structType = serialize(chunk->m_oldPtr, serializer);
	serializer->finalizeChunk(chunk, structType, BT_SHAPE_CODE, (void*)this);
}
