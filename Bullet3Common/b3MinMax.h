/*
Copyright (c) 2003-2013 Gino van den Bergen / Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B3_GEN_MINMAX_H
#define B3_GEN_MINMAX_H

#include "b3Scalar.h"

template <class T>
B3_FORCE_INLINE const T& b3Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template <class T>
B3_FORCE_INLINE const T& b3Max(const T& a, const T& b)
{
	return a > b ? a : b;
}

template <class T>
B3_FORCE_INLINE const T& b3Clamped(const T& a, const T& lb, const T& ub)
{
	return a < lb ? lb : (ub < a ? ub : a);
}

template <class T>
B3_FORCE_INLINE void b3SetMin(T& a, const T& b)
{
	if (b < a)
	{
		a = b;
	}
}

template <class T>
B3_FORCE_INLINE void b3SetMax(T& a, const T& b)
{
	if (a < b)
	{
		a = b;
	}
}

template <class T>
B3_FORCE_INLINE void b3Clamp(T& a, const T& lb, const T& ub)
{
	if (a < lb)
	{
		a = lb;
	}
	else if (ub < a)
	{
		a = ub;
	}
}

inline float DegreesToRadians(float angle) { return angle * (float)M_PI / 180.0f; }
inline double DegreesToRadians(double angle) { return angle * M_PI / 180.0; }

inline float RadiansToDegrees(float angle) { return angle * 180.0f / (float)M_PI; }
inline double RadiansToDegrees(double angle) { return angle * 180.0 / M_PI; }

/** compute the volume of a tetrahedron. */
template<typename T>
inline float computeVolume(const T& a, const T& b, const T& c, const T& d)
{
    return fabsf(((b - c) ^ (a - b)).dot(d - b));
}

/** compute the volume of a prism. */
template<typename T>
inline float computeVolume(const T& f1, const T& f2, const T& f3,
    const T& b1, const T& b2, const T& b3)
{
    return computeVolume(f1, f2, f3, b1) +
        computeVolume(b1, b2, b3, f2) +
        computeVolume(b1, b3, f2, f3);
}

#endif  //B3_GEN_MINMAX_H
