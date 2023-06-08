//
//  eigenheaders.h
//  MultiTracker
//
//  Created by Fang Da on 10/24/14.
//
//

#ifndef MultiTracker_eigenheaders_h
#define MultiTracker_eigenheaders_h

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "surftrack.h"

typedef Eigen::Matrix<double, 4, 4> EigenMat4d;
typedef Eigen::Matrix<double, 3, 3> EigenMat3d;
typedef Eigen::Matrix<double, 3, 2> EigenMat32d;
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatXd;
typedef Eigen::Matrix<double, 4, 1> EigenVec4d;
typedef Eigen::Matrix<double, 3, 1> EigenVec3d;
typedef Eigen::Matrix<double, 2, 1> EigenVec2d;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> EigenVecXd;

typedef Eigen::Matrix<int, 3, 1> EigenVec3i;
typedef Eigen::Matrix<int, 2, 1> EigenVec2i;
typedef Eigen::Matrix<size_t, 3, 1> EigenVec3st;

typedef Eigen::SparseMatrix<double> EigenSparseMatrix;

EigenVec3d vc(const Vec3d & v);
Vec3d vc(const EigenVec3d & v);

class Vec2iComp
{
public:
    bool operator () (const Vec2i & v1, const Vec2i & v2) const { return v1[0] < v2[0] || (v1[0] == v2[0] && v1[1] < v2[1]); }
};

#endif
