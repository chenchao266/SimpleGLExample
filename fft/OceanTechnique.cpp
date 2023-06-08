/*
* This source file is part of the osgOcean library
* 
* Copyright (C) 2009 Kim Bale
* Copyright (C) 2009 The University of Hull, UK
* 
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.

* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
* http://www.gnu.org/copyleft/lesser.txt.
*/

#include "OceanTechnique.h"
#include <iostream>
#include <sstream>
#include <fstream>
OceanTechnique::OceanTechnique(void)
    :_isDirty    ( true )
    ,_isAnimating( true )
{}

OceanTechnique::OceanTechnique( const OceanTechnique& copy  )
    :MultiGeometryShape  ( copy )
    ,_isDirty    ( true )
    ,_isAnimating( copy._isAnimating )
{}

void OceanTechnique::build(void)
{
    std::cout << "OceanTechnique::build() Not Implemented" << std::endl;
}

float OceanTechnique::getSurfaceHeight(void) const
{
    std::cout << "OceanTechnique::getSurfaceHeight() Not Implemented" << std::endl;
    return 0.f;
}

float OceanTechnique::getMaximumHeight(void) const
{
    std::cout << "OceanTechnique::getMaximumHeight() Not Implemented" << std::endl;
    return 0.f;
}

/** Check if the ocean surface is visible or not. Basic test is very 
    simple, subclasses can do a better test. */
bool OceanTechnique::isVisible( CullVisitor& cv, bool eyeAboveWater )
{
    if (getNodeMask() == 0) return false;

    // Use a cutoff to unconditionally cull ocean surface if we can't see it.
    // This test is valid when the eye is close to the horizon, but further up
    // it will be too conservative (i.e. it will return true even when the 
    // surface is not visible because it does the test relative to a horizontal
    // plane at the eye position).
   
    if ((*cv.getProjectionMatrix())(3,3) == 0.0)     // Perspective
    {
        double fovy, ratio, zNear, zFar;
        cv.getProjectionMatrixAsPerspective(fovy, ratio, zNear, zFar);

        static const float cutoff = fovy / 2.0;
        Vec3f lookVector = cv.getLookVectorLocal();
        float dotProduct = lookVector .dot( Vec3f(0,0,1));
        return ( eyeAboveWater && dotProduct <  cutoff) ||
               (!eyeAboveWater && dotProduct > -cutoff);
    }
    else                                                      // Ortho
    {
        return true;
    }

    // A better way would be to check if any of the frustum corners intersect 
    // the plane at (0,0,ocean_height) with normal (0,0,1), and if not then 
    // return true.
}
  
// --------------------------------------------------------
//  EventHandler implementation
// --------------------------------------------------------
 

bool OceanTechnique::handle(int key)
{ 
    return false;
}
 