
#include "CollisionShape2TriangleMesh.h"

//#include "btBulletCollisionCommon.h"
//#include "Bullet3Common/btShapeHull.h"  //to create a tesselation of a generic btConvexShape
#include "Bullet3Common/btCollisionShape.h"

void CollisionShape2TriangleMesh(btCollisionShape* collisionShape, const b3Transform& parentTransform, b3AlignedObjectArray<b3Vector3>& vertexPositions, b3AlignedObjectArray<b3Vector3>& vertexNormals, b3AlignedObjectArray<int>& indicesOut)

{
	//todo: support all collision shape types
	switch (collisionShape->getShapeType())
    {
    case SOFTBODY_SHAPE_PROXYTYPE:
    {
        //skip the soft body collision shape for now
        break;
    }
    ///...
    default:
    {
        if (collisionShape->isConvex())
        {///...
        }
        else
        {
            if (collisionShape->isCompound())
            {///...
            }
            else
            {
                if (collisionShape->getShapeType() == SDF_SHAPE_PROXYTYPE)
                {
                    //not yet
                }
                else
                {
                    b3Assert(0);
                }
            }
        }
    }
	};
}
