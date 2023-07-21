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

#ifndef B3_SCALAR_H
#define B3_SCALAR_H

 
#define _USE_MATH_DEFINES
#include <math.h>
  //size_t for MSVC 6.0
#include <float.h>

//Original repository is at http://github.com/erwincoumans/bullet3
 
#if defined(DEBUG) || defined(_DEBUG)
#define B3_DEBUG
#endif

//#include "b3Logging.h"  //for b3Error
#define B3_PROFILE(name)
#define b3StartProfile(a)
#define b3StopProfile
#define b3Warning(...)
#define b3Error(...)
#define b3Printf(...) 

#define B3_USE_SSE
#define B3_USE_SSE_IN_API
#ifdef _WIN32

#include <intrin.h>

#if  defined(__GNUC__)	// it should handle both MINGW and CYGWIN
#define B3_FORCE_INLINE             __inline__ __attribute__((always_inline))
#define B3_ATTRIBUTE_ALIGNED16(a)   a __attribute__((aligned(16)))
#define B3_ATTRIBUTE_ALIGNED64(a)   a __attribute__((aligned(64)))
#define B3_ATTRIBUTE_ALIGNED128(a)  a __attribute__((aligned(128)))
#elif ( defined(_MSC_VER) && _MSC_VER < 1300 )
#define B3_FORCE_INLINE inline
#define B3_ATTRIBUTE_ALIGNED16(a) a
#define B3_ATTRIBUTE_ALIGNED64(a) a
#define B3_ATTRIBUTE_ALIGNED128(a) a
#else
//#define B3_HAS_ALIGNED_ALLOCATOR
#pragma warning(disable : 4324)  // disable padding warning
//			#pragma warning(disable:4530) // Disable the exception disable but used in MSCV Stl warning.
#pragma warning(disable : 4996)  //Turn off warnings about deprecated C routines
//			#pragma warning(disable:4786) // Disable the "debug name too long" warning

#define B3_FORCE_INLINE __forceinline
#define B3_ATTRIBUTE_ALIGNED16(a) __declspec(align(16)) a
#define B3_ATTRIBUTE_ALIGNED64(a) __declspec(align(64)) a
#define B3_ATTRIBUTE_ALIGNED128(a) __declspec(align(128)) a

#endif  //__MINGW32__

#ifdef B3_DEBUG
#ifdef _MSC_VER
#include <stdio.h>
#define b3Assert(x) 
#else  //_MSC_VER
#include <assert.h>
#define b3Assert assert
#endif  //_MSC_VER
#else
#define b3Assert(x)
#endif
//b3FullAssert is optional, slows down a lot
#define b3FullAssert(x)

#define b3Likely(_c) _c
#define b3Unlikely(_c) _c

#endif

///The b3Scalar type abstracts floating point numbers, to easily switch between double and single floating point precision.
#if defined(B3_USE_DOUBLE_PRECISION)
typedef double b3Scalar;
//this number could be bigger in double precision
#define B3_LARGE_FLOAT 1e30
#else
typedef float b3Scalar;
//keep B3_LARGE_FLOAT*B3_LARGE_FLOAT < FLT_MAX
#define B3_LARGE_FLOAT 1e18f
#endif

#ifdef B3_USE_SSE
typedef __m128 b3SimdFloat4;
#endif  //B3_USE_SSE

#if defined B3_USE_SSE_IN_API && defined(B3_USE_SSE)
#ifdef _WIN32

#ifndef B3_NAN
static int b3NanMask = 0x7F800001;
#define B3_NAN (*(float *)&b3NanMask)
#endif

#ifndef B3_INFINITY_MASK
static int b3InfinityMask = 0x7F800000;
#define B3_INFINITY_MASK (*(float *)&b3InfinityMask)
#endif
#ifndef B3_NO_SIMD_OPERATOR_OVERLOADS
inline __m128 operator+(const __m128 A, const __m128 B)
{
	return _mm_add_ps(A, B);
}

inline __m128 operator-(const __m128 A, const __m128 B)
{
	return _mm_sub_ps(A, B);
}

inline __m128 operator*(const __m128 A, const __m128 B)
{
	return _mm_mul_ps(A, B);
}
#endif //B3_NO_SIMD_OPERATOR_OVERLOADS
#define b3CastfTo128i(a) (_mm_castps_si128(a))
#define b3CastfTo128d(a) (_mm_castps_pd(a))
#define b3CastiTo128f(a) (_mm_castsi128_ps(a))
#define b3CastdTo128f(a) (_mm_castpd_ps(a))
#define b3CastdTo128i(a) (_mm_castpd_si128(a))
#define b3Assign128(r0, r1, r2, r3) _mm_setr_ps(r0, r1, r2, r3)

#else  //_WIN32

#define b3CastfTo128i(a) ((__m128i)(a))
#define b3CastfTo128d(a) ((__m128d)(a))
#define b3CastiTo128f(a) ((__m128)(a))
#define b3CastdTo128f(a) ((__m128)(a))
#define b3CastdTo128i(a) ((__m128i)(a))
#define b3Assign128(r0, r1, r2, r3) \
	(__m128) { r0, r1, r2, r3 }
#endif  //_WIN32
#endif  //B3_USE_SSE_IN_API

#ifdef B3_USE_NEON
#include <arm_neon.h>

typedef float32x4_t b3SimdFloat4;
#define B3_INFINITY INFINITY
#define B3_NAN NAN
#define b3Assign128(r0, r1, r2, r3) \
	(float32x4_t) { r0, r1, r2, r3 }
#endif

#define B3_DECLARE_ALIGNED_ALLOCATOR()                                                                   \
	B3_FORCE_INLINE void *operator new(size_t sizeInBytes) { return b3AlignedAlloc(sizeInBytes, 16); }   \
	B3_FORCE_INLINE void operator delete(void *ptr) { b3AlignedFree(ptr); }                              \
	B3_FORCE_INLINE void *operator new(size_t, void *ptr) { return ptr; }                                \
	B3_FORCE_INLINE void operator delete(void *, void *) {}                                              \
	B3_FORCE_INLINE void *operator new[](size_t sizeInBytes) { return b3AlignedAlloc(sizeInBytes, 16); } \
	B3_FORCE_INLINE void operator delete[](void *ptr) { b3AlignedFree(ptr); }                            \
	B3_FORCE_INLINE void *operator new[](size_t, void *ptr) { return ptr; }                              \
	B3_FORCE_INLINE void operator delete[](void *, void *) {}

#if defined(B3_USE_DOUBLE_PRECISION) //|| defined(B3_FORCE_DOUBLE_FUNCTIONS)

B3_FORCE_INLINE b3Scalar b3Sqrt(b3Scalar x)
{
	return sqrt(x);
}
B3_FORCE_INLINE b3Scalar b3Fabs(b3Scalar x) { return fabs(x); }
B3_FORCE_INLINE b3Scalar b3Cos(b3Scalar x) { return cos(x); }
B3_FORCE_INLINE b3Scalar b3Sin(b3Scalar x) { return sin(x); }
B3_FORCE_INLINE b3Scalar b3Tan(b3Scalar x) { return tan(x); }
B3_FORCE_INLINE b3Scalar b3Acos(b3Scalar x)
{
	if (x < b3Scalar(-1)) x = b3Scalar(-1);
	if (x > b3Scalar(1)) x = b3Scalar(1);
	return acos(x);
}
B3_FORCE_INLINE b3Scalar b3Asin(b3Scalar x)
{
	if (x < b3Scalar(-1)) x = b3Scalar(-1);
	if (x > b3Scalar(1)) x = b3Scalar(1);
	return asin(x);
}
B3_FORCE_INLINE b3Scalar b3Atan(b3Scalar x) { return atan(x); }
B3_FORCE_INLINE b3Scalar b3Atan2(b3Scalar x, b3Scalar y) { return atan2(x, y); }
B3_FORCE_INLINE b3Scalar b3Exp(b3Scalar x) { return exp(x); }
B3_FORCE_INLINE b3Scalar b3Log(b3Scalar x) { return log(x); }
B3_FORCE_INLINE b3Scalar b3Pow(b3Scalar x, b3Scalar y) { return pow(x, y); }
B3_FORCE_INLINE b3Scalar b3Fmod(b3Scalar x, b3Scalar y) { return fmod(x, y); }

#else

B3_FORCE_INLINE b3Scalar b3Sqrt(b3Scalar y)
{
#ifdef USE_APPROXIMATION
	double x, z, tempf;
	unsigned long *tfptr = ((unsigned long *)&tempf) + 1;

	tempf = y;
	*tfptr = (0xbfcdd90a - *tfptr) >> 1; /* estimate of 1/sqrt(y) */
	x = tempf;
	z = y * b3Scalar(0.5);
	x = (b3Scalar(1.5) * x) - (x * x) * (x * z); /* iteration formula     */
	x = (b3Scalar(1.5) * x) - (x * x) * (x * z);
	x = (b3Scalar(1.5) * x) - (x * x) * (x * z);
	x = (b3Scalar(1.5) * x) - (x * x) * (x * z);
	x = (b3Scalar(1.5) * x) - (x * x) * (x * z);
	return x * y;
#else
	return sqrtf(y);
#endif
}
B3_FORCE_INLINE b3Scalar b3Fabs(b3Scalar x) { return fabsf(x); }
B3_FORCE_INLINE b3Scalar b3Cos(b3Scalar x) { return cosf(x); }
B3_FORCE_INLINE b3Scalar b3Sin(b3Scalar x) { return sinf(x); }
B3_FORCE_INLINE b3Scalar b3Tan(b3Scalar x) { return tanf(x); }
B3_FORCE_INLINE b3Scalar b3Acos(b3Scalar x)
{
	if (x < b3Scalar(-1))
		x = b3Scalar(-1);
	if (x > b3Scalar(1))
		x = b3Scalar(1);
	return acosf(x);
}
B3_FORCE_INLINE b3Scalar b3Asin(b3Scalar x)
{
	if (x < b3Scalar(-1))
		x = b3Scalar(-1);
	if (x > b3Scalar(1))
		x = b3Scalar(1);
	return asinf(x);
}
B3_FORCE_INLINE b3Scalar b3Atan(b3Scalar x) { return atanf(x); }
B3_FORCE_INLINE b3Scalar b3Atan2(b3Scalar x, b3Scalar y) { return atan2f(x, y); }
B3_FORCE_INLINE b3Scalar b3Exp(b3Scalar x) { return expf(x); }
B3_FORCE_INLINE b3Scalar b3Log(b3Scalar x) { return logf(x); }
B3_FORCE_INLINE b3Scalar b3Pow(b3Scalar x, b3Scalar y) { return powf(x, y); }
B3_FORCE_INLINE b3Scalar b3Fmod(b3Scalar x, b3Scalar y) { return fmodf(x, y); }

#endif

#define B3_2_PI b3Scalar(M_PI * 2)
#define B3_PI b3Scalar(M_PI)
#define B3_HALF_PI b3Scalar(M_PI_2)
#define B3_RADS_PER_DEG (B3_2_PI / b3Scalar(360.0))
#define B3_DEGS_PER_RAD (b3Scalar(360.0) / B3_2_PI)
#define B3_SQRT12 b3Scalar(M_SQRT1_2)

#define b3RecipSqrt(x) ((b3Scalar)(b3Scalar(1.0) / b3Sqrt(b3Scalar(x)))) /* reciprocal square root */

#ifdef B3_USE_DOUBLE_PRECISION
#define B3_EPSILON DBL_EPSILON
#define B3_INFINITY DBL_MAX
#else
#define B3_EPSILON FLT_EPSILON
#define B3_INFINITY FLT_MAX
#endif

B3_FORCE_INLINE b3Scalar b3Atan2Fast(b3Scalar y, b3Scalar x)
{
	b3Scalar coeff_1 = B3_PI / 4.0f;
	b3Scalar coeff_2 = 3.0f * coeff_1;
	b3Scalar abs_y = b3Fabs(y);
	b3Scalar angle;
	if (x >= 0.0f)
	{
		b3Scalar r = (x - abs_y) / (x + abs_y);
		angle = coeff_1 - coeff_1 * r;
	}
	else
	{
		b3Scalar r = (x + abs_y) / (abs_y - x);
		angle = coeff_2 - coeff_1 * r;
	}
	return (y < 0.0f) ? -angle : angle;
}

B3_FORCE_INLINE bool b3FuzzyZero(b3Scalar x) { return b3Fabs(x) < B3_EPSILON; }

B3_FORCE_INLINE bool b3Equal(b3Scalar a, b3Scalar eps)
{
	return (((a) <= eps) && !((a) < -eps));
}
B3_FORCE_INLINE bool b3GreaterEqual(b3Scalar a, b3Scalar eps)
{
	return (!((a) <= eps));
}

B3_FORCE_INLINE int b3IsNegative(b3Scalar x)
{
	return x < b3Scalar(0.0) ? 1 : 0;
}

B3_FORCE_INLINE b3Scalar b3Radians(b3Scalar x) { return x * B3_RADS_PER_DEG; }
B3_FORCE_INLINE b3Scalar b3Degrees(b3Scalar x) { return x * B3_DEGS_PER_RAD; }

#define B3_DECLARE_HANDLE(name) \
	typedef struct name##__     \
	{                           \
		int unused;             \
	} * name

#ifndef b3Fsel
B3_FORCE_INLINE b3Scalar b3Fsel(b3Scalar a, b3Scalar b, b3Scalar c)
{
	return a >= 0 ? b : c;
}
#endif
#define b3Fsels(a, b, c) (b3Scalar) b3Fsel(a, b, c)

B3_FORCE_INLINE bool b3MachineIsLittleEndian()
{
	long int i = 1;
	const char *p = (const char *)&i;
	if (p[0] == 1)  // Lowest address contains the least significant byte
		return true;
	else
		return false;
}

///b3Select avoids branches, which makes performance much better for consoles like Playstation 3 and XBox 360
///Thanks Phil Knight. See also http://www.cellperformance.com/articles/2006/04/more_techniques_for_eliminatin_1.html
B3_FORCE_INLINE unsigned b3Select(unsigned condition, unsigned valueIfConditionNonZero, unsigned valueIfConditionZero)
{
	// Set testNz to 0xFFFFFFFF if condition is nonzero, 0x00000000 if condition is zero
	// Rely on positive value or'ed with its negative having sign bit on
	// and zero value or'ed with its negative (which is still zero) having sign bit off
	// Use arithmetic shift right, shifting the sign bit through all 32 bits
	unsigned testNz = (unsigned)(((int)condition | -(int)condition) >> 31);
	unsigned testEqz = ~testNz;
	return ((valueIfConditionNonZero & testNz) | (valueIfConditionZero & testEqz));
}
B3_FORCE_INLINE int b3Select(unsigned condition, int valueIfConditionNonZero, int valueIfConditionZero)
{
	unsigned testNz = (unsigned)(((int)condition | -(int)condition) >> 31);
	unsigned testEqz = ~testNz;
	return static_cast<int>((valueIfConditionNonZero & testNz) | (valueIfConditionZero & testEqz));
}
B3_FORCE_INLINE float b3Select(unsigned condition, float valueIfConditionNonZero, float valueIfConditionZero)
{
#ifdef B3_HAVE_NATIVE_FSEL
	return (float)b3Fsel((b3Scalar)condition - b3Scalar(1.0f), valueIfConditionNonZero, valueIfConditionZero);
#else
	return (condition != 0) ? valueIfConditionNonZero : valueIfConditionZero;
#endif
}

template <typename T>
B3_FORCE_INLINE void b3Swap(T &a, T &b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

//PCK: endian swapping functions
B3_FORCE_INLINE unsigned b3SwapEndian(unsigned val)
{
	return (((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24));
}

B3_FORCE_INLINE unsigned short b3SwapEndian(unsigned short val)
{
	return static_cast<unsigned short>(((val & 0xff00) >> 8) | ((val & 0x00ff) << 8));
}

B3_FORCE_INLINE unsigned b3SwapEndian(int val)
{
	return b3SwapEndian((unsigned)val);
}

B3_FORCE_INLINE unsigned short b3SwapEndian(short val)
{
	return b3SwapEndian((unsigned short)val);
}

///b3SwapFloat uses using char pointers to swap the endianness
////b3SwapFloat/b3SwapDouble will NOT return a float, because the machine might 'correct' invalid floating point values
///Not all values of sign/exponent/mantissa are valid floating point numbers according to IEEE 754.
///When a floating point unit is faced with an invalid value, it may actually change the value, or worse, throw an exception.
///In most systems, running user mode code, you wouldn't get an exception, but instead the hardware/os/runtime will 'fix' the number for you.
///so instead of returning a float/double, we return integer/long long integer
B3_FORCE_INLINE unsigned int b3SwapEndianFloat(float d)
{
	unsigned int a = 0;
	unsigned char *dst = (unsigned char *)&a;
	unsigned char *src = (unsigned char *)&d;

	dst[0] = src[3];
	dst[1] = src[2];
	dst[2] = src[1];
	dst[3] = src[0];
	return a;
}

// unswap using char pointers
B3_FORCE_INLINE float b3UnswapEndianFloat(unsigned int a)
{
	float d = 0.0f;
	unsigned char *src = (unsigned char *)&a;
	unsigned char *dst = (unsigned char *)&d;

	dst[0] = src[3];
	dst[1] = src[2];
	dst[2] = src[1];
	dst[3] = src[0];

	return d;
}

// swap using char pointers
B3_FORCE_INLINE void b3SwapEndianDouble(double d, unsigned char *dst)
{
	unsigned char *src = (unsigned char *)&d;

	dst[0] = src[7];
	dst[1] = src[6];
	dst[2] = src[5];
	dst[3] = src[4];
	dst[4] = src[3];
	dst[5] = src[2];
	dst[6] = src[1];
	dst[7] = src[0];
}

// unswap using char pointers
B3_FORCE_INLINE double b3UnswapEndianDouble(const unsigned char *src)
{
	double d = 0.0;
	unsigned char *dst = (unsigned char *)&d;

	dst[0] = src[7];
	dst[1] = src[6];
	dst[2] = src[5];
	dst[3] = src[4];
	dst[4] = src[3];
	dst[5] = src[2];
	dst[6] = src[1];
	dst[7] = src[0];

	return d;
}

// returns normalized value in range [-B3_PI, B3_PI]
B3_FORCE_INLINE b3Scalar b3NormalizeAngle(b3Scalar angleInRadians)
{
	angleInRadians = b3Fmod(angleInRadians, B3_2_PI);
	if (angleInRadians < -B3_PI)
	{
		return angleInRadians + B3_2_PI;
	}
	else if (angleInRadians > B3_PI)
	{
		return angleInRadians - B3_2_PI;
	}
	else
	{
		return angleInRadians;
	}
}

///rudimentary class to provide type info
struct b3TypedObject
{
	b3TypedObject(int objectType)
		: m_objectType(objectType)
	{
	}
	int m_objectType;
	inline int getObjectType() const
	{
		return m_objectType;
	}
};

///align a pointer to the provided alignment, upwards
template <typename T>
T *b3AlignPointer(T *unalignedPtr, size_t alignment)
{
	struct b3ConvertPointerSizeT
	{
		union {
			T *ptr;
			size_t integer;
		};
	};
	b3ConvertPointerSizeT converter;

	const size_t bit_mask = ~(alignment - 1);
	converter.ptr = unalignedPtr;
	converter.integer += alignment - 1;
	converter.integer &= bit_mask;
	return converter.ptr;
}


#define SIMD_PI b3Scalar(3.1415926535897932384626433832795029)
#define SIMD_2_PI (b3Scalar(2.0) * SIMD_PI)
#define SIMD_HALF_PI (SIMD_PI * b3Scalar(0.5))
#define SIMD_RADS_PER_DEG (SIMD_2_PI / b3Scalar(360.0))
#define SIMD_DEGS_PER_RAD (b3Scalar(360.0) / SIMD_2_PI)
#define SIMDSQRT12 b3Scalar(0.7071067811865475244008443621048490)
#define btRecipSqrt(x) ((b3Scalar)(b3Scalar(1.0) / btSqrt(b3Scalar(x)))) /* reciprocal square root */
#define btRecip(x) (b3Scalar(1.0) / b3Scalar(x))

#ifndef M_PI
const static double M_PI = 3.1415926535897932384626433832795029;
#endif

#ifdef B3_USE_DOUBLE_PRECISION
#define SIMD_EPSILON DBL_EPSILON
#define SIMD_INFINITY DBL_MAX
#define BT_ONE 1.0
#define BT_ZERO 0.0
#define BT_TWO 2.0
#define BT_HALF 0.5
#else
#define SIMD_EPSILON FLT_EPSILON
#define SIMD_INFINITY FLT_MAX
#define BT_ONE 1.0f
#define BT_ZERO 0.0f
#define BT_TWO 2.0f
#define BT_HALF 0.5f
#endif
#endif  //B3_SCALAR_H
