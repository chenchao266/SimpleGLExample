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

#ifndef BT_COLLISION_OBJECT_H
#define BT_COLLISION_OBJECT_H

#include "Bullet3Common/b3Transform.h"

//island management, m_activationState1
#define ACTIVE_TAG 1
#define ISLAND_SLEEPING 2
#define WANTS_DEACTIVATION 3
#define DISABLE_DEACTIVATION 4
#define DISABLE_SIMULATION 5

//struct btBroadphaseProxy;
class btCollisionShape;
struct btCollisionShapeData;
//#include "Bullet3Common/btMotionState.h"
#include "Bullet3Common/b3AlignedAllocator.h"
#include "Bullet3Common/b3AlignedObjectArray.h"

typedef b3AlignedObjectArray<class btCollisionObject*> btCollisionObjectArray;

#ifdef B3_USE_DOUBLE_PRECISION
#define btCollisionObjectData btCollisionObjectDoubleData
#define btCollisionObjectDataName "btCollisionObjectDoubleData"
#else
#define btCollisionObjectData btCollisionObjectFloatData
#define btCollisionObjectDataName "btCollisionObjectFloatData"
#endif

/// btCollisionObject can be used to manage collision detection objects.
/// btCollisionObject maintains all information that is needed for a collision detection: Shape, Transform and AABB proxy.
/// They can be added to the btCollisionWorld.
B3_ATTRIBUTE_ALIGNED16(class)
btCollisionObject
{
protected:
	b3Transform m_worldTransform;

	///m_interpolationWorldTransform is used for CCD and interpolation
	///it can be either previous or future (predicted) transform
	b3Transform m_interpolationWorldTransform;
	//those two are experimental: just added for bullet time effect, so you can still apply impulses (directly modifying velocities)
	//without destroying the continuous interpolated motion (which uses this interpolation velocities)
	b3Vector3 m_interpolationLinearVelocity;
	b3Vector3 m_interpolationAngularVelocity;

	b3Vector3 m_anisotropicFriction;
	int m_hasAnisotropicFriction;
	b3Scalar m_contactProcessingThreshold;

	//btBroadphaseProxy* m_broadphaseHandle;
	btCollisionShape* m_collisionShape;
	///m_extensionPointer is used by some internal low-level Bullet extensions.
	void* m_extensionPointer;

	///m_rootCollisionShape is temporarily used to store the original collision shape
	///The m_collisionShape might be temporarily replaced by a child collision shape during collision detection purposes
	///If it is NULL, the m_collisionShape is not temporarily replaced.
	btCollisionShape* m_rootCollisionShape;

	int m_collisionFlags;

	int m_islandTag1;
	int m_companionId;
	int m_worldArrayIndex;  // index of object in world's collisionObjects array

	mutable int m_activationState1;
	mutable b3Scalar m_deactivationTime;

	b3Scalar m_friction;
	b3Scalar m_restitution;
	b3Scalar m_rollingFriction;   //torsional friction orthogonal to contact normal (useful to stop spheres rolling forever)
	b3Scalar m_spinningFriction;  // torsional friction around the contact normal (useful for grasping)
	b3Scalar m_contactDamping;
	b3Scalar m_contactStiffness;

	///m_internalType is reserved to distinguish Bullet's btCollisionObject, btRigidBody, btSoftBody, btGhostObject etc.
	///do not assign your own m_internalType unless you write a new dynamics object class.
	int m_internalType;

	///users can point to their objects, m_userPointer is not used by Bullet, see setUserPointer/getUserPointer

	void* m_userObjectPointer;

	int m_userIndex2;

	int m_userIndex;

	int m_userIndex3;

	///time of impact calculation
	b3Scalar m_hitFraction;

	///Swept sphere radius (0.0 by default), see btConvexConvexAlgorithm::
	b3Scalar m_ccdSweptSphereRadius;

	/// Don't do continuous collision detection if the motion (in one step) is less then m_ccdMotionThreshold
	b3Scalar m_ccdMotionThreshold;

	/// If some object should have elaborate collision filtering by sub-classes
	int m_checkCollideWith;

	b3AlignedObjectArray<const btCollisionObject*> m_objectsWithoutCollisionCheck;

	///internal update revision number. It will be increased when the object changes. This allows some subsystems to perform lazy evaluation.
	int m_updateRevision;

	b3Vector3 m_customDebugColorRGB;

public:
	B3_DECLARE_ALIGNED_ALLOCATOR();

	enum CollisionFlags
	{
		CF_STATIC_OBJECT = 1,
		CF_KINEMATIC_OBJECT = 2,
		CF_NO_CONTACT_RESPONSE = 4,
		CF_CUSTOM_MATERIAL_CALLBACK = 8,  //this allows per-triangle material (friction/restitution)
		CF_CHARACTER_OBJECT = 16,
		CF_DISABLE_VISUALIZE_OBJECT = 32,          //disable debug drawing
		CF_DISABLE_SPU_COLLISION_PROCESSING = 64,  //disable parallel/SPU processing
		CF_HAS_CONTACT_STIFFNESS_DAMPING = 128,
		CF_HAS_CUSTOM_DEBUG_RENDERING_COLOR = 256,
		CF_HAS_FRICTION_ANCHOR = 512,
		CF_HAS_COLLISION_SOUND_TRIGGER = 1024
	};

	enum CollisionObjectTypes
	{
		CO_COLLISION_OBJECT = 1,
		CO_RIGID_BODY = 2,
		///CO_GHOST_OBJECT keeps track of all objects overlapping its AABB and that pass its collision filter
		///It is useful for collision sensors, explosion objects, character controller etc.
		CO_GHOST_OBJECT = 4,
		CO_SOFT_BODY = 8,
		CO_HF_FLUID = 16,
		CO_USER_TYPE = 32,
		CO_FEATHERSTONE_LINK = 64
	};

	enum AnisotropicFrictionFlags
	{
		CF_ANISOTROPIC_FRICTION_DISABLED = 0,
		CF_ANISOTROPIC_FRICTION = 1,
		CF_ANISOTROPIC_ROLLING_FRICTION = 2
	};

	B3_FORCE_INLINE bool mergesSimulationIslands() const
	{
		///static objects, kinematic and object without contact response don't merge islands
		return ((m_collisionFlags & (CF_STATIC_OBJECT | CF_KINEMATIC_OBJECT | CF_NO_CONTACT_RESPONSE)) == 0);
	}

	const b3Vector3& getAnisotropicFriction() const
	{
		return m_anisotropicFriction;
	}
	void setAnisotropicFriction(const b3Vector3& anisotropicFriction, int frictionMode = CF_ANISOTROPIC_FRICTION)
	{
		m_anisotropicFriction = anisotropicFriction;
		bool isUnity = (anisotropicFriction[0] != 1.f) || (anisotropicFriction[1] != 1.f) || (anisotropicFriction[2] != 1.f);
		m_hasAnisotropicFriction = isUnity ? frictionMode : 0;
	}
	bool hasAnisotropicFriction(int frictionMode = CF_ANISOTROPIC_FRICTION) const
	{
		return (m_hasAnisotropicFriction & frictionMode) != 0;
	}

	///the constraint solver can discard solving contacts, if the distance is above this threshold. 0 by default.
	///Note that using contacts with positive distance can improve stability. It increases, however, the chance of colliding with degerate contacts, such as 'interior' triangle edges
	void setContactProcessingThreshold(b3Scalar contactProcessingThreshold)
	{
		m_contactProcessingThreshold = contactProcessingThreshold;
	}
	b3Scalar getContactProcessingThreshold() const
	{
		return m_contactProcessingThreshold;
	}

	B3_FORCE_INLINE bool isStaticObject() const
	{
		return (m_collisionFlags & CF_STATIC_OBJECT) != 0;
	}

	B3_FORCE_INLINE bool isKinematicObject() const
	{
		return (m_collisionFlags & CF_KINEMATIC_OBJECT) != 0;
	}

	B3_FORCE_INLINE bool isStaticOrKinematicObject() const
	{
		return (m_collisionFlags & (CF_KINEMATIC_OBJECT | CF_STATIC_OBJECT)) != 0;
	}

	B3_FORCE_INLINE bool hasContactResponse() const
	{
		return (m_collisionFlags & CF_NO_CONTACT_RESPONSE) == 0;
	}

	btCollisionObject();

	virtual ~btCollisionObject();

	virtual void setCollisionShape(btCollisionShape * collisionShape)
	{
		m_updateRevision++;
		m_collisionShape = collisionShape;
		m_rootCollisionShape = collisionShape;
	}

	B3_FORCE_INLINE const btCollisionShape* getCollisionShape() const
	{
		return m_collisionShape;
	}

	B3_FORCE_INLINE btCollisionShape* getCollisionShape()
	{
		return m_collisionShape;
	}

	void setIgnoreCollisionCheck(const btCollisionObject* co, bool ignoreCollisionCheck)
	{
		if (ignoreCollisionCheck)
		{
			//We don't check for duplicates. Is it ok to leave that up to the user of this API?
			//int index = m_objectsWithoutCollisionCheck.findLinearSearch(co);
			//if (index == m_objectsWithoutCollisionCheck.size())
			//{
			m_objectsWithoutCollisionCheck.push_back(co);
			//}
		}
		else
		{
			m_objectsWithoutCollisionCheck.remove(co);
		}
		m_checkCollideWith = m_objectsWithoutCollisionCheck.size() > 0;
	}

	virtual bool checkCollideWithOverride(const btCollisionObject* co) const
	{
		int index = m_objectsWithoutCollisionCheck.findLinearSearch(co);
		if (index < m_objectsWithoutCollisionCheck.size())
		{
			return false;
		}
		return true;
	}

	///Avoid using this internal API call, the extension pointer is used by some Bullet extensions.
	///If you need to store your own user pointer, use 'setUserPointer/getUserPointer' instead.
	void* internalGetExtensionPointer() const
	{
		return m_extensionPointer;
	}
	///Avoid using this internal API call, the extension pointer is used by some Bullet extensions
	///If you need to store your own user pointer, use 'setUserPointer/getUserPointer' instead.
	void internalSetExtensionPointer(void* pointer)
	{
		m_extensionPointer = pointer;
	}

	B3_FORCE_INLINE int getActivationState() const { return m_activationState1; }

	void setActivationState(int newState) const;

	void setDeactivationTime(b3Scalar time)
	{
		m_deactivationTime = time;
	}
	b3Scalar getDeactivationTime() const
	{
		return m_deactivationTime;
	}

	void forceActivationState(int newState) const;

	void activate(bool forceActivation = false) const;

	B3_FORCE_INLINE bool isActive() const
	{
		return ((getActivationState() != ISLAND_SLEEPING) && (getActivationState() != DISABLE_SIMULATION));
	}

	void setRestitution(b3Scalar rest)
	{
		m_updateRevision++;
		m_restitution = rest;
	}
	b3Scalar getRestitution() const
	{
		return m_restitution;
	}
	void setFriction(b3Scalar frict)
	{
		m_updateRevision++;
		m_friction = frict;
	}
	b3Scalar getFriction() const
	{
		return m_friction;
	}

	void setRollingFriction(b3Scalar frict)
	{
		m_updateRevision++;
		m_rollingFriction = frict;
	}
	b3Scalar getRollingFriction() const
	{
		return m_rollingFriction;
	}
	void setSpinningFriction(b3Scalar frict)
	{
		m_updateRevision++;
		m_spinningFriction = frict;
	}
	b3Scalar getSpinningFriction() const
	{
		return m_spinningFriction;
	}
	void setContactStiffnessAndDamping(b3Scalar stiffness, b3Scalar damping)
	{
		m_updateRevision++;
		m_contactStiffness = stiffness;
		m_contactDamping = damping;

		m_collisionFlags |= CF_HAS_CONTACT_STIFFNESS_DAMPING;

		//avoid divisions by zero...
		if (m_contactStiffness < SIMD_EPSILON)
		{
			m_contactStiffness = SIMD_EPSILON;
		}
	}

	b3Scalar getContactStiffness() const
	{
		return m_contactStiffness;
	}

	b3Scalar getContactDamping() const
	{
		return m_contactDamping;
	}

	///reserved for Bullet internal usage
	int getInternalType() const
	{
		return m_internalType;
	}

	b3Transform& getWorldTransform()
	{
		return m_worldTransform;
	}

	const b3Transform& getWorldTransform() const
	{
		return m_worldTransform;
	}

	void setWorldTransform(const b3Transform& worldTrans)
	{
		m_updateRevision++;
		m_worldTransform = worldTrans;
	}
 
	const b3Transform& getInterpolationWorldTransform() const
	{
		return m_interpolationWorldTransform;
	}

	b3Transform& getInterpolationWorldTransform()
	{
		return m_interpolationWorldTransform;
	}

	void setInterpolationWorldTransform(const b3Transform& trans)
	{
		m_updateRevision++;
		m_interpolationWorldTransform = trans;
	}

	void setInterpolationLinearVelocity(const b3Vector3& linvel)
	{
		m_updateRevision++;
		m_interpolationLinearVelocity = linvel;
	}

	void setInterpolationAngularVelocity(const b3Vector3& angvel)
	{
		m_updateRevision++;
		m_interpolationAngularVelocity = angvel;
	}

	const b3Vector3& getInterpolationLinearVelocity() const
	{
		return m_interpolationLinearVelocity;
	}

	const b3Vector3& getInterpolationAngularVelocity() const
	{
		return m_interpolationAngularVelocity;
	}

	B3_FORCE_INLINE int getIslandTag() const
	{
		return m_islandTag1;
	}

	void setIslandTag(int tag)
	{
		m_islandTag1 = tag;
	}

	B3_FORCE_INLINE int getCompanionId() const
	{
		return m_companionId;
	}

	void setCompanionId(int id)
	{
		m_companionId = id;
	}

	B3_FORCE_INLINE int getWorldArrayIndex() const
	{
		return m_worldArrayIndex;
	}

	// only should be called by CollisionWorld
	void setWorldArrayIndex(int ix)
	{
		m_worldArrayIndex = ix;
	}

	B3_FORCE_INLINE b3Scalar getHitFraction() const
	{
		return m_hitFraction;
	}

	void setHitFraction(b3Scalar hitFraction)
	{
		m_hitFraction = hitFraction;
	}

	B3_FORCE_INLINE int getCollisionFlags() const
	{
		return m_collisionFlags;
	}

	void setCollisionFlags(int flags)
	{
		m_collisionFlags = flags;
	}

	///Swept sphere radius (0.0 by default), see btConvexConvexAlgorithm::
	b3Scalar getCcdSweptSphereRadius() const
	{
		return m_ccdSweptSphereRadius;
	}

	///Swept sphere radius (0.0 by default), see btConvexConvexAlgorithm::
	void setCcdSweptSphereRadius(b3Scalar radius)
	{
		m_ccdSweptSphereRadius = radius;
	}

	b3Scalar getCcdMotionThreshold() const
	{
		return m_ccdMotionThreshold;
	}

	b3Scalar getCcdSquareMotionThreshold() const
	{
		return m_ccdMotionThreshold * m_ccdMotionThreshold;
	}

	/// Don't do continuous collision detection if the motion (in one step) is less then m_ccdMotionThreshold
	void setCcdMotionThreshold(b3Scalar ccdMotionThreshold)
	{
		m_ccdMotionThreshold = ccdMotionThreshold;
	}

	///users can point to their objects, userPointer is not used by Bullet
	void* getUserPointer() const
	{
		return m_userObjectPointer;
	}

	int getUserIndex() const
	{
		return m_userIndex;
	}

	int getUserIndex2() const
	{
		return m_userIndex2;
	}

	int getUserIndex3() const
	{
		return m_userIndex3;
	}

	///users can point to their objects, userPointer is not used by Bullet
	void setUserPointer(void* userPointer)
	{
		m_userObjectPointer = userPointer;
	}

	///users can point to their objects, userPointer is not used by Bullet
	void setUserIndex(int index)
	{
		m_userIndex = index;
	}

	void setUserIndex2(int index)
	{
		m_userIndex2 = index;
	}

	void setUserIndex3(int index)
	{
		m_userIndex3 = index;
	}

	int getUpdateRevisionInternal() const
	{
		return m_updateRevision;
	}

	void setCustomDebugColor(const b3Vector3& colorRGB)
	{
		m_customDebugColorRGB = colorRGB;
		m_collisionFlags |= CF_HAS_CUSTOM_DEBUG_RENDERING_COLOR;
	}

	void removeCustomDebugColor()
	{
		m_collisionFlags &= ~CF_HAS_CUSTOM_DEBUG_RENDERING_COLOR;
	}

	bool getCustomDebugColor(b3Vector3 & colorRGB) const
	{
		bool hasCustomColor = (0 != (m_collisionFlags & CF_HAS_CUSTOM_DEBUG_RENDERING_COLOR));
		if (hasCustomColor)
		{
			colorRGB = m_customDebugColorRGB;
		}
		return hasCustomColor;
	}

	inline bool checkCollideWith(const btCollisionObject* co) const
	{
		if (m_checkCollideWith)
			return checkCollideWithOverride(co);

		return true;
	}

	virtual int calculateSerializeBufferSize() const;

	///fills the dataBuffer and returns the struct name (and 0 on failure)
	virtual const char* serialize(void* dataBuffer, class btSerializer* serializer) const;

	virtual void serializeSingleObject(class btSerializer * serializer) const;
};

// clang-format off

///do not change those serialization structures, it requires an updated sBulletDNAstr/sBulletDNAstr64
struct	btCollisionObjectDoubleData
{
	void					*m_broadphaseHandle;
	void					*m_collisionShape;
	btCollisionShapeData	*m_rootCollisionShape;
	char					*m_name;

	b3TransformDoubleData	m_worldTransform;
	b3TransformDoubleData	m_interpolationWorldTransform;
	b3Vector3DoubleData		m_interpolationLinearVelocity;
	b3Vector3DoubleData		m_interpolationAngularVelocity;
	b3Vector3DoubleData		m_anisotropicFriction;
	double					m_contactProcessingThreshold;	
	double					m_deactivationTime;
	double					m_friction;
	double					m_rollingFriction;
	double                  m_contactDamping;
	double                  m_contactStiffness;
	double					m_restitution;
	double					m_hitFraction; 
	double					m_ccdSweptSphereRadius;
	double					m_ccdMotionThreshold;
	int						m_hasAnisotropicFriction;
	int						m_collisionFlags;
	int						m_islandTag1;
	int						m_companionId;
	int						m_activationState1;
	int						m_internalType;
	int						m_checkCollideWith;
	int						m_collisionFilterGroup;
	int						m_collisionFilterMask;
	int						m_uniqueId;//m_uniqueId is introduced for paircache. could get rid of this, by calculating the address offset etc.
};

///do not change those serialization structures, it requires an updated sBulletDNAstr/sBulletDNAstr64
struct	btCollisionObjectFloatData
{
	void					*m_broadphaseHandle;
	void					*m_collisionShape;
	btCollisionShapeData	*m_rootCollisionShape;
	char					*m_name;

	b3TransformFloatData	m_worldTransform;
	b3TransformFloatData	m_interpolationWorldTransform;
	b3Vector3FloatData		m_interpolationLinearVelocity;
	b3Vector3FloatData		m_interpolationAngularVelocity;
	b3Vector3FloatData		m_anisotropicFriction;
	float					m_contactProcessingThreshold;	
	float					m_deactivationTime;
	float					m_friction;
	float					m_rollingFriction;
	float                   m_contactDamping;
    float                   m_contactStiffness;
	float					m_restitution;
	float					m_hitFraction; 
	float					m_ccdSweptSphereRadius;
	float					m_ccdMotionThreshold;
	int						m_hasAnisotropicFriction;
	int						m_collisionFlags;
	int						m_islandTag1;
	int						m_companionId;
	int						m_activationState1;
	int						m_internalType;
	int						m_checkCollideWith;
	int						m_collisionFilterGroup;
	int						m_collisionFilterMask;
	int						m_uniqueId;
};
// clang-format on

B3_FORCE_INLINE int btCollisionObject::calculateSerializeBufferSize() const
{
	return sizeof(btCollisionObjectData);
}

#endif  //BT_COLLISION_OBJECT_H
