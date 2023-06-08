#ifndef LOSTOPOS_VEC_H
#define LOSTOPOS_VEC_H

#include <cassert>
#include <cmath>
#include <hashtable.h>
#include <iostream>
#include <util.h>
#include <Bullet3Common/geometry.h>

 

// Defines a thin wrapper around fixed size C-style arrays, using template parameters,
// which is useful for dealing with vectors of different dimensions.
// For example, float[3] is equivalent to vec<3,float>.
// Entries in the vector are accessed with the overloaded [] operator, so
// for example if x is a vec<3,float>, then the middle entry is x[1].
// For convenience, there are a number of typedefs for abbreviation:
//   vec<3,float> -> Vec3f
//   vec<2,int>   -> Vec2i
// and so on.
// Arithmetic operators are appropriately overloaded, and functions are defined
// for additional operations (such as dot-products, norms, cross-products, etc.)
     
template <size_t N, typename T> std::ostream& operator<<(std::ostream& out, const vec<N, T>& v) {
    for (unsigned int i = 0; i < N; i++) {
        out << v[i] << " ";
    }
    return out;
}
  
template<size_t N, class T>
std::istream &operator>>(std::istream &in, vec<N,T> &u)
{
    in>>u.u[0];
    for(unsigned int i=1; i<N; ++i)
        in>>u.u[i];
    return in;
}
 
template<size_t N, class T>
inline void minmax(const vec<N,T> &x0, const vec<N,T> &x1, vec<N,T> &xmin, vec<N,T> &xmax)
{
    for(unsigned int i=0; i<N; ++i)
        minmax(x0.u[i], x1.u[i], xmin.u[i], xmax.u[i]);
}

template<size_t N, class T>
inline void minmax(const vec<N,T> &x0, const vec<N,T> &x1, const vec<N,T> &x2, vec<N,T> &xmin, vec<N,T> &xmax)
{
    for(unsigned int i=0; i<N; ++i)
        minmax(x0.u[i], x1.u[i], x2.u[i], xmin.u[i], xmax.u[i]);
}

template<size_t N, class T>
inline void minmax(const vec<N,T> &x0, const vec<N,T> &x1, const vec<N,T> &x2, const vec<N,T> &x3,
                   vec<N,T> &xmin, vec<N,T> &xmax)
{
    for(unsigned int i=0; i<N; ++i)
        minmax(x0.u[i], x1.u[i], x2.u[i], x3.u[i], xmin.u[i], xmax.u[i]);
}

template<size_t N, class T>
inline void minmax(const vec<N,T> &x0, const vec<N,T> &x1, const vec<N,T> &x2, const vec<N,T> &x3, const vec<N,T> &x4,
                   vec<N,T> &xmin, vec<N,T> &xmax)
{
    for(unsigned int i=0; i<N; ++i)
        minmax(x0.u[i], x1.u[i], x2.u[i], x3.u[i], x4.u[i], xmin.u[i], xmax.u[i]);
}

template<size_t N, class T>
inline void minmax(const vec<N,T> &x0, const vec<N,T> &x1, const vec<N,T> &x2, const vec<N,T> &x3, const vec<N,T> &x4,
                   const vec<N,T> &x5, vec<N,T> &xmin, vec<N,T> &xmax)
{
    for(unsigned int i=0; i<N; ++i)
        minmax(x0.u[i], x1.u[i], x2.u[i], x3.u[i], x4.u[i], x5.u[i], xmin.u[i], xmax.u[i]);
}

template<size_t N, class T>
inline void update_minmax(const vec<N,T> &x, vec<N,T> &xmin, vec<N,T> &xmax)
{
    for(unsigned int i=0; i<N; ++i) update_minmax(x[i], xmin[i], xmax[i]);
}

template<size_t N, class T>
inline unsigned int hash(const vec<N, T> &a)
{
    unsigned int h = a.u[0];
    for (unsigned int i = 1; i < N; ++i)
        h = hash(h ^ a.u[i]);
    return h;
}

#endif
