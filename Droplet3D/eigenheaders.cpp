//
//  eigenheaders.cpp
//  MultiTracker
//
//  Created by Fang Da on 10/27/14.
//
//

#include "eigenheaders.h"

Vec3d vc(const EigenVec3d & v)
{
    return Vec3d(v[0], v[1], v[2]);
}

EigenVec3d vc(const Vec3d & v)
{
    return EigenVec3d(v[0], v[1], v[2]);
}

