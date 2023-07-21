#ifndef GLOBAL_H
#define GLOBAL_H
#include "Bullet3Common/b3Scalar.h"
//#define MAX(a, b) ((a)>=(b) ? (a) : (b))
//#define MIN(a, b) ((a)<=(b) ? (a) : (b))

typedef unsigned int		uint;
typedef unsigned short		ushort;
typedef unsigned char		uchar;
 
#define drand48() (rand() / RAND_MAX)

//#define USE_ASSERT
//#define DEBUG_MODE

//#define HIDE_FROZEN_PARTICLES
//#define USE_VBO_CPU
#define USE_CUDA
#define USE_VBO_CUDA
//#define USE_FFMPEG
//#define DIRECT_FORCING

#define ENABLE_DEBUG
 
#define MATH_PRECISION double

 
const static int TEX_SIZE = 2048;
const static float LIGHT_NEAR = 0.5;
const static float LIGHT_FAR = 300.0;
const static float DEGtoRAD = (M_PI / 180.0);
 
const static float EPSILON = 0.00001f;
const static uint GRID_UCHAR = 0xFF;
const static uint GRID_UNDEF = 0xFFFFFFFF;

const static int  MAX_PARAM = 60;
 

#define CLRVAL			uint
#define COLOR(r,g,b)	( (uint(r*255.0f)<<24) | (uint(g*255.0f)<<16) | (uint(b*255.0f)<<8) )
#define COLORA(r,g,b,a)	( (uint(a*255.0f)<<24) | (uint(b*255.0f)<<16) | (uint(g*255.0f)<<8) | uint(r*255.0f) )
//#define COLOR(r,g,b)	( (uint((b)*255.0f)<<16) | (uint((g)*255.0f)<<8) | uint((r)*255.0f) )
//#define COLORA(r,g,b,a)	( (uint((a)*255.0f)<<24) | (uint((b)*255.0f)<<16) | (uint((g)*255.0f)<<8) | uint((r)*255.0f) )


#define ALPH(c)			(float((c>>24) & 0xFF)/255.0)
#define BLUE(c)			(float((c>>16) & 0xFF)/255.0)
#define GRN(c)			(float((c>>8)  & 0xFF)/255.0)
#define RED(c)			(float( c      & 0xFF)/255.0)

enum Dimensions
{
    GRID_RESOLUTION = 1024,		  // < 16-bit grid, must be power-of-two
    NUM_PARTICLES = 4096,
    MAX_NUM_RIGID_PARTICLES = 16384,

};

enum SpecificSimAlgorithm
{
	SPH_BASIC = 0,
	SPH_PENALTY_FORCE,
	SPH_DIRECT_FORCE,
	SPH_IHMSEN,
	SPH_APPROXIMATE,
	WCSPH_BASIC,
	WCSPH_PENALTY_FORCE,
	WCSPH_DIRECT_FORCE,
	WCSPH_IHMSEN,
	WCSPH_VERSATILECOUPLING,
	WCSPH_SPH_APPROXIMATE,
	PCISPH_BASIC,
	PCISPH_PENALTY_FORCE,
	PCISPH_DIRECT_FORCE,
	PCISPH_IHMSEN,
	PCISPH_VERSATILECOUPLING
};

const static unsigned int max_neighs = 50; // Usually each particle has 30-40 neighs 
const static unsigned int MAX_PCISPH_LOOPS	= 50; // 50 //3 TODO: need to determine this value for performance/visual effects
const static unsigned int MIN_PCISPH_LOOPS = 3;

#define SPH_PROFILING

#ifdef SPH_PROFILING
#define SPH_PROFILING_FREQ 100
#endif
 
#define  _USE_MATH_DEFINES

#endif
