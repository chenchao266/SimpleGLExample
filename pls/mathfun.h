#pragma once
#include <iostream>
#include <sstream>
#include <fstream>

#include <Bullet3Common/geometry.h>
 
 
double CalcWENO(double v1, double v2, double v3, double v4, double v5);
 
inline double square(double x)
{
    return x * x;
};
 
struct line2d
{
    Vec2d v[2];
};


struct triangle3d
{
    Vec3d v[3];
};

inline bool vec3flt(Vec3d& lhs, Vec3d& rhs)
{
    if (lhs[0] < rhs[0]) return true;
    if (lhs[0] > rhs[0]) return false;

    if (lhs[1] < rhs[1]) return true;
    if (lhs[1] > rhs[1]) return false;

    if (lhs[2] < rhs[2]) return true;
    if (lhs[2] > rhs[2]) return false;

    return false;
}

inline bool tri3flt(triangle3d& lhs, triangle3d& rhs)
{
    if (vec3flt(lhs.v[0], rhs.v[0])) return true;
    if (vec3flt(rhs.v[0], lhs.v[0])) return false;

    if (vec3flt(lhs.v[1], rhs.v[1])) return true;
    if (vec3flt(rhs.v[1], lhs.v[1])) return false;

    if (vec3flt(lhs.v[2], rhs.v[2])) return true;
    if (vec3flt(rhs.v[2], lhs.v[2])) return false;

    return false;
}