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

#ifndef BT_COLLISION_SHAPE_H
#define BT_COLLISION_SHAPE_H

#include "Bullet3Common/b3Transform.h"
#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/b3Matrix3x3.h"
#include "Bullet3Common/boundingSphere.h"
#include "Bullet3Common/BoundingBox.h"
//#include "btBroadphaseProxy.h"  //for the shape types
class btSerializer;
enum BroadphaseNativeTypes
{
    // polyhedral convex shapes
    BOX_SHAPE_PROXYTYPE,
    TRIANGLE_SHAPE_PROXYTYPE,
    TETRAHEDRAL_SHAPE_PROXYTYPE,
    CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE,
    CONVEX_HULL_SHAPE_PROXYTYPE,
    CONVEX_POINT_CLOUD_SHAPE_PROXYTYPE,
    CUSTOM_POLYHEDRAL_SHAPE_TYPE,
    //implicit convex shapes
    IMPLICIT_CONVEX_SHAPES_START_HERE,
    SPHERE_SHAPE_PROXYTYPE,
    MULTI_SPHERE_SHAPE_PROXYTYPE,
    CAPSULE_SHAPE_PROXYTYPE,
    CONE_SHAPE_PROXYTYPE,
    CONVEX_SHAPE_PROXYTYPE,
    CYLINDER_SHAPE_PROXYTYPE,
    UNIFORM_SCALING_SHAPE_PROXYTYPE,
    MINKOWSKI_SUM_SHAPE_PROXYTYPE,
    MINKOWSKI_DIFFERENCE_SHAPE_PROXYTYPE,
    BOX_2D_SHAPE_PROXYTYPE,
    CONVEX_2D_SHAPE_PROXYTYPE,
    CUSTOM_CONVEX_SHAPE_TYPE,
    //concave shapes
    CONCAVE_SHAPES_START_HERE,
    //keep all the convex shapetype below here, for the check IsConvexShape in broadphase proxy!
    TRIANGLE_MESH_SHAPE_PROXYTYPE,
    SCALED_TRIANGLE_MESH_SHAPE_PROXYTYPE,
    ///used for demo integration FAST/Swift collision library and Bullet
    FAST_CONCAVE_MESH_PROXYTYPE,
    //terrain
    TERRAIN_SHAPE_PROXYTYPE,
    ///Used for GIMPACT Trimesh integration
    GIMPACT_SHAPE_PROXYTYPE,
    ///Multimaterial mesh
    MULTIMATERIAL_TRIANGLE_MESH_PROXYTYPE,

    EMPTY_SHAPE_PROXYTYPE,
    STATIC_PLANE_PROXYTYPE,
    CUSTOM_CONCAVE_SHAPE_TYPE,
    SDF_SHAPE_PROXYTYPE = CUSTOM_CONCAVE_SHAPE_TYPE,
    CONCAVE_SHAPES_END_HERE,

    COMPOUND_SHAPE_PROXYTYPE,

    SOFTBODY_SHAPE_PROXYTYPE,
    HFFLUID_SHAPE_PROXYTYPE,
    HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE,
    INVALID_SHAPE_PROXYTYPE,

    MAX_BROADPHASE_COLLISION_TYPES

};
class CullVisitor;
///The btCollisionShape class provides an interface for collision shapes that can be shared among btCollisionObjects.
B3_ATTRIBUTE_ALIGNED16(class)
btCollisionShape
{
protected:
    int m_shapeType;
    void* m_userPointer;
    int m_userIndex;
    b3Vector3 scale;
    BoundingSphere                          _initialBound;
    mutable BoundingSphere                  _boundingSphere;
    BoundingBox                         _initialBoundingBox; 
    mutable BoundingBox                 _boundingBox;
public:
    B3_DECLARE_ALIGNED_ALLOCATOR();

    btCollisionShape() : m_shapeType(INVALID_SHAPE_PROXYTYPE), m_userPointer(0), m_userIndex(-1)
    {
    }

    virtual ~btCollisionShape()
    {
    }

    ///getAabb returns the axis aligned bounding box in the coordinate frame of the given transform t.
    virtual void getAabb(const b3Transform& t, b3Vector3& aabbMin, b3Vector3& aabbMax) const = 0;

    virtual void getBoundingSphere(b3Vector3 & center, b3Scalar & radius) const;

    ///getAngularMotionDisc returns the maximum radius needed for Conservative Advancement to handle time-of-impact with rotations.
    virtual b3Scalar getAngularMotionDisc() const;

    virtual b3Scalar getContactBreakingThreshold(b3Scalar defaultContactThresholdFactor) const;

    ///calculateTemporalAabb calculates the enclosing aabb for the moving object over interval [0..timeStep)
    ///result is conservative
    void calculateTemporalAabb(const b3Transform& curTrans, const b3Vector3& linvel, const b3Vector3& angvel, b3Scalar timeStep, b3Vector3& temporalAabbMin, b3Vector3& temporalAabbMax) const;

    B3_FORCE_INLINE bool isPolyhedral() const
    {
        return (getShapeType() < IMPLICIT_CONVEX_SHAPES_START_HERE);
    }

    B3_FORCE_INLINE bool isConvex() const
    {
        return (getShapeType() < CONCAVE_SHAPES_START_HERE);
    }

    B3_FORCE_INLINE bool isConvex2d(int proxyType)
    {
        return (getShapeType() == BOX_2D_SHAPE_PROXYTYPE) || (getShapeType() == CONVEX_2D_SHAPE_PROXYTYPE);
    }

    B3_FORCE_INLINE bool isConcave() const
    {
        return (getShapeType() > CONCAVE_SHAPES_START_HERE) &&
            (getShapeType() < CONCAVE_SHAPES_END_HERE);
    }
    B3_FORCE_INLINE bool isCompound() const
    {
        return (getShapeType() == COMPOUND_SHAPE_PROXYTYPE);
    }

    B3_FORCE_INLINE bool isSoftBody() const
    {
        return (getShapeType() == SOFTBODY_SHAPE_PROXYTYPE);
    }

    ///isInfinite is used to catch simulation error (aabb check)
    B3_FORCE_INLINE bool isInfinite() const
    {
        return (getShapeType() == STATIC_PLANE_PROXYTYPE);
    }

#ifndef __SPU__
    virtual void setLocalScaling(const b3Vector3& scaling) { scale = scaling; };
    virtual const b3Vector3& getLocalScaling() const { return scale; };
    virtual void calculateLocalInertia(b3Scalar mass, b3Vector3 & inertia) const {}

    //debugging support
    virtual const char* getName() const { return ""; };
#endif  //__SPU__

    int getShapeType() const
    {
        return m_shapeType;
    }

    /** Set the initial bounding volume to use when computing the overall bounding volume.*/
    void setInitialBound(const BoundingSphere& bsphere) { _initialBound = bsphere; dirtyBound(); }

    /** Set the initial bounding volume to use when computing the overall bounding volume.*/
    const BoundingSphere& getInitialBound() const { return _initialBound; }

    /** Mark this node's bounding sphere dirty.
        Forcing it to be computed on the next call to getBound().*/
    void dirtyBound() {};

    /** Set the initial bounding volume to use when computing the overall bounding volume.*/
    void setInitialBoundingBox(const BoundingBox& bbox) { _initialBoundingBox = bbox; dirtyBound(); }

    /** Set the initial bounding volume to use when computing the overall bounding volume.*/
    const BoundingBox& getInitialBoundingBox() const { return _initialBoundingBox; }
 
    inline const BoundingSphere& getBound() const
    {
        if (!_boundingSphere.valid())
        {
            _boundingSphere = _initialBound; 
            _boundingSphere.expandBy(computeBound());
             
        }
        return _boundingSphere;
    }
    inline const BoundingSphere& getBoundingBox() const
    {
        if (!_boundingBox.valid())
        {
            _boundingBox = _initialBoundingBox;
            _boundingBox.expandBy(computeBoundingBox());

        }
        return _boundingBox;
    }
    virtual BoundingSphere computeBound() const { return BoundingSphere(); };
    virtual BoundingBox computeBoundingBox() const { return BoundingBox(); };
    ///the getAnisotropicRollingFrictionDirection can be used in combination with setAnisotropicFriction
    ///See Bullet/Demos/RollingFrictionDemo for an example
    virtual b3Vector3 getAnisotropicRollingFrictionDirection() const
    {
        return b3Vector3(1, 1, 1);
    }
    virtual void setMargin(b3Scalar margin) {};
    virtual b3Scalar getMargin() const { return 0.04; };

	///optional user data pointer
	void setUserPointer(void* userPtr)
	{
		m_userPointer = userPtr;
	}

	void* getUserPointer() const
	{
		return m_userPointer;
	}
	void setUserIndex(int index)
	{
		m_userIndex = index;
	}

	int getUserIndex() const
	{
		return m_userIndex;
	}

	virtual int calculateSerializeBufferSize() const;

	///fills the dataBuffer and returns the struct name (and 0 on failure)
	virtual const char* serialize(void* dataBuffer, btSerializer* serializer) const;

	virtual void serializeSingleShape(btSerializer * serializer) const;
    virtual void cull(CullVisitor* cv) {};
    virtual void update(unsigned frameNum, double currentTime) {};
    virtual void setMatrix(const Matrixf&) {};
    virtual Matrixf getMatrix() { return Matrixf::identity(); };
    void  setNodeMask(int) {};
    int  getNodeMask() { return 0; };
};

// clang-format off
// parser needs * with the name
///do not change those serialization structures, it requires an updated sBulletDNAstr/sBulletDNAstr64
struct	btCollisionShapeData
{
	char	*m_name;
	int		m_shapeType;
	char	m_padding[4];
};
// clang-format on
B3_FORCE_INLINE int btCollisionShape::calculateSerializeBufferSize() const
{
	return sizeof(btCollisionShapeData);
}

#endif  //BT_COLLISION_SHAPE_H
