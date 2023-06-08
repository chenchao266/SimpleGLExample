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

#pragma once
#include <Component/GeometryShape.h>
#include <Bullet3Common/Polytope.h>
 
class State
{

};
class CullVisitor
{
protected:


public:
    CullVisitor(void) {};

    CullVisitor(const CullVisitor& copy) {};

    /** Add a drawable to current render graph.*/
    void addDrawable(GeometryShape* drawable, Matrixf* matrix) {};

    /** Add a drawable and depth to current render graph.*/
    void addDrawableAndDepth(GeometryShape* drawable, Matrixf* matrix, float depth) {};
    Matrixf* getProjectionMatrix() { return nullptr; };
    Matrixf* getModelViewMatrix() { return nullptr; };
    Matrixf* getViewMatrix() { return nullptr; };
    void updateCalculatedNearFar(Matrixf*, const BoundingBox&) {};
    void getViewMatrixAsLookAt(Vec3f& eye, Vec3f& centre, Vec3f& up) {};
    void getProjectionMatrixAsPerspective(double& fov, double& aspectRatio, double& near, double& far) {};
    bool clampProjectionMatrix(Matrixf* projection, float& znear, float& zfar) const { return false; };

    float getFrustumVolume()
    {
        return 0;
    }
    void setFrustum(Polytope& cv) { _frustum = cv; }
    bool isShadowMap() { return false; }
    Polytope& getFrustum() { return _frustum; }
    const Polytope& getFrustum() const { return _frustum; }
    Vec3f getEyePoint() const { return _eye; }
    Vec3f getLookVectorLocal() const { return _vecLocal; }
    Vec4f getViewport() { return _vp; }
    virtual ~CullVisitor(void) {  };
    void pushModelViewMatrix(Matrixf) {};
    void popModelViewMatrix() {};
    void setTraversalMask(int) {};
    int getTraversalMask() { return 0; };

protected:
    Polytope    _frustum;
    Vec3f       _eye;
    Vec3f       _vecLocal;
    Vec4f       _vp;
};


