/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BT_DYNAMICS_WORLD_H
#define BT_DYNAMICS_WORLD_H
#include "Bullet3Common/b3Scalar.h"
#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/btCollisionObject.h"
class btDynamicsWorld;
class btIDebugDraw;
/// Type for the callback for each tick
typedef void (*btInternalTickCallback)(btDynamicsWorld* world, b3Scalar timeStep);

enum btDynamicsWorldType
{
	BT_SIMPLE_DYNAMICS_WORLD = 1,
	BT_DISCRETE_DYNAMICS_WORLD = 2,
	BT_CONTINUOUS_DYNAMICS_WORLD = 3,
	BT_SOFT_RIGID_DYNAMICS_WORLD = 4,
	BT_GPU_DYNAMICS_WORLD = 5,
	BT_SOFT_MULTIBODY_DYNAMICS_WORLD = 6,
    BT_DEFORMABLE_MULTIBODY_DYNAMICS_WORLD = 7
};

///The btDynamicsWorld is the interface class for several dynamics implementation, basic, discrete, parallel, and continuous etc.
class btDynamicsWorld
{
protected:
	btInternalTickCallback m_internalTickCallback;
	btInternalTickCallback m_internalPreTickCallback;
	void* m_worldUserInfo;
    b3AlignedObjectArray<btCollisionObject*> m_collisionObjects;
     
    btIDebugDraw* m_debugDrawer;
    ///m_forceUpdateAllAabbs can be set to false as an optimization to only update active object AABBs
    ///it is true by default, because it is error-prone (setting the position of static objects wouldn't update their AABB)
    bool m_forceUpdateAllAabbs;
public:
	btDynamicsWorld()
		: m_internalTickCallback(0), m_internalPreTickCallback(0), m_worldUserInfo(0)
	{
	}

	virtual ~btDynamicsWorld()
	{
	}

	///stepSimulation proceeds the simulation over 'timeStep', units in preferably in seconds.
	///By default, Bullet will subdivide the timestep in constant substeps of each 'fixedTimeStep'.
	///in order to keep the simulation real-time, the maximum number of substeps can be clamped to 'maxSubSteps'.
	///You can disable subdividing the timestep/substepping by passing maxSubSteps=0 as second argument to stepSimulation, but in that case you have to keep the timeStep constant.
    virtual int stepSimulation(b3Scalar timeStep, int maxSubSteps = 1, b3Scalar fixedTimeStep = b3Scalar(1.) / b3Scalar(60.)) {};

    virtual void debugDrawWorld() {};

    virtual void debugDrawObject(const b3Transform& worldTransform, const btCollisionShape* shape, const b3Vector3& color) {};

	//once a rigidbody is added to the dynamics world, it will get this gravity assigned
	//existing rigidbodies in the world get gravity assigned too, during this method
    virtual void setGravity(const b3Vector3& gravity) {};
	virtual b3Vector3 getGravity() const { };
       
    virtual btDynamicsWorldType getWorldType() const {};

    virtual void updateAabbs() {};

    virtual void setDebugDrawer(btIDebugDraw* debugDrawer)
    {
        m_debugDrawer = debugDrawer;
    }

    virtual btIDebugDraw* getDebugDrawer()
    {
        return m_debugDrawer;
    }
    btCollisionObjectArray& getCollisionObjectArray()
    {
        return m_collisionObjects;
    }

    const btCollisionObjectArray& getCollisionObjectArray() const
    {
        return m_collisionObjects;
    }
    int getNumCollisionObjects() const
    {
        return int(m_collisionObjects.size());
    }
    virtual void removeCollisionObject(btCollisionObject* collisionObject) {};
    bool getForceUpdateAllAabbs() const
    {
        return m_forceUpdateAllAabbs;
    }
    void setForceUpdateAllAabbs(bool forceUpdateAllAabbs)
    {
        m_forceUpdateAllAabbs = forceUpdateAllAabbs;
    }
	/// Set the callback for when an internal tick (simulation substep) happens, optional user info
	void setInternalTickCallback(btInternalTickCallback cb, void* worldUserInfo = 0, bool isPreTick = false)
	{
		if (isPreTick)
		{
			m_internalPreTickCallback = cb;
		}
		else
		{
			m_internalTickCallback = cb;
		}
		m_worldUserInfo = worldUserInfo;
	}

	void setWorldUserInfo(void* worldUserInfo)
	{
		m_worldUserInfo = worldUserInfo;
	}

	void* getWorldUserInfo() const
	{
		return m_worldUserInfo;
	}
      
};

///do not change those serialization structures, it requires an updated sBulletDNAstr/sBulletDNAstr64
struct btDynamicsWorldDoubleData
{
 
	b3Vector3DoubleData m_gravity;
};

///do not change those serialization structures, it requires an updated sBulletDNAstr/sBulletDNAstr64
struct btDynamicsWorldFloatData
{
 
	b3Vector3FloatData m_gravity;
};

#endif  //BT_DYNAMICS_WORLD_H
