#ifndef COLLISION_SHAPE_2_GRAPHICS_H
#define COLLISION_SHAPE_2_GRAPHICS_H

#include "Bullet3Common/b3AlignedObjectArray.h"
#include "Bullet3Common/b3Transform.h"
class btCollisionShape;

void CollisionShape2TriangleMesh(btCollisionShape* collisionShape, const b3Transform& parentTransform, b3AlignedObjectArray<b3Vector3>& vertexPositions, b3AlignedObjectArray<b3Vector3>& vertexNormals, b3AlignedObjectArray<int>& indicesOut);

#endif  //COLLISION_SHAPE_2_GRAPHICS_H
