﻿//
//  BoundaryIntegral.h
//  Droplet3D
//
//  Created by Fang Da on 10/10/15.
//
//

#ifndef __Droplet3D__BoundaryIntegral__
#define __Droplet3D__BoundaryIntegral__

#include "eigenheaders.h"
#include "surftrack.h"

class BPS3D;

class BoundaryIntegral
{
public:
    // 1D quadrature (precomputed reference domain coordinates and weights)
    static const std::vector<EigenVec2d> & quadrature_line(int N = 1);       // x is the ref domain coordinate (ref domain = (-1, 1)); y is the weight
    
    // 2D quadrature (precomputed reference domain coordinates and weights)
    static const std::vector<EigenVec3d> & quadrature_square(int N = 2);     // x and y are the u, v coordinates (ref domain = (-1, 1)^2); z is the weight
    static const std::vector<EigenVec3d> & quadrature_triangle(int N = 1);   // x and y are the u, v coordinates (ref domain = (0,0), (1,0), (1,1)); z is the weight
    
public:
    // fundamental solution
    // note that passing in x = y will result in NaN
    static double G  (const EigenVec3d & x, const EigenVec3d & y) { return -(1 / (4 * M_PI)) / (x - y).norm(); }
    static EigenVec3d dGdx(const EigenVec3d & x, const EigenVec3d & y) { return  (1 / (4 * M_PI)) * (x - y) / ((x - y).squaredNorm() * (x - y).norm()); }
    static EigenVec3d dGdy(const EigenVec3d & x, const EigenVec3d & y) { return  (1 / (4 * M_PI)) * (y - x) / ((x - y).squaredNorm() * (x - y).norm()); }

public:
    static EigenVec2d duffyTransform(const EigenVec3d & x0, const EigenVec3d & x1, const EigenVec3d & x2, const EigenVec2d & k, EigenVec3d & x, double & jacobian, EigenVec3d & c);
    static EigenVec2d duffyTransform(const EigenMat3d & xs, int singularity, const EigenVec2d & k, EigenVec3d & x, double & jacobian, EigenVec3d & c);
    static void lineTransform(const EigenVec3d & x0, const EigenVec3d & x1, double k, EigenVec3d & x, double & jacobian, EigenVec2d & c);
    static void lineTransform(const EigenMat32d & xs, double k, EigenVec3d & x, double & jacobian, EigenVec2d & c);

    inline static void nonSingularTransform(const EigenVec3d & x0, const EigenVec3d & x1, const EigenVec3d & x2, const EigenVec2d & k, EigenVec3d & x, double & jacobian, EigenVec3d & c)
    {
        assert(k.x() >= 0 && k.x() <= 1 && k.y() >= 0 && k.y() <= k.x());
        
        EigenVec3d n = (x1 - x0).cross(x2 - x0);
        double a = n.norm();
        n /= a;
        a /= 2;
        assert(a != 0);
        assert(n == n);
        
        EigenMat32d defmap;     // deformation from the triangle ref domain to the world frame: mapping (0,0) to x0, (1,0) to x1, (1,1) to x2
        defmap.col(0) = x1 - x0;
        defmap.col(1) = x2 - x1;
        
        // the world coordinates of the quadrature point
        x = defmap * k + x0;
        double jacobian_x = a * 2;              // the jacobian dx/dk
        
        // the overall jacobian
        jacobian = jacobian_x;                  // dx/dk
        
        // the baricentric coordinates of x in the face
        c[0] = 1 - k.x();
        c[1] = k.x() - k.y();
        c[2] = k.y();
        assert(c[0] >= 0 && c[0] <= 1);
        assert(c[1] >= 0 && c[1] <= 1);
        assert(c[2] >= 0 && c[2] <= 1);
    }

    inline static void nonSingularTransform(const EigenMat3d & xs, const EigenVec2d & k, EigenVec3d & x, double & jacobian, EigenVec3d & c)
    {
        nonSingularTransform(xs.col(0), xs.col(1), xs.col(2), k, x, jacobian, c);
    }
    
};



#endif /* defined(__Droplet3D__BoundaryIntegral__) */
