/*
  FLUIDS v.1 - SPH Fluid Simulator for CPU and GPU
  Copyright (C) 2008. Rama Hoetzlein, http://www.rchoetzlein.com

  ZLib license
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef DEF_FLUID
#define DEF_FLUID
	
	#include "Bullet3Common/b3Vector3.h"
	#include "global.h"
 
struct FluidParticle
{
    b3Vector3 position;
};
	struct Fluid {
	public:
        b3Vector3		pos;			// Basic particle (must match Particle class)
		unsigned			clr;
		int				next;
        b3Vector3		vel;
        b3Vector3		vel_eval;
		unsigned short	age;

		float			pressure;		// Smoothed Particle Hydrodynamics
		float			density;	
        b3Vector3		sph_force;
	};
    inline b3Vector4 fromClr(CLRVAL clr) {
        return  b3Vector4 (RED(clr), GRN(clr), BLUE(clr), ALPH(clr)); 
    }
    inline  CLRVAL toClr(const b3Vector4& xyzw) {
        return (CLRVAL)COLORA(xyzw[0], xyzw[1], xyzw[2], xyzw[3]); 
    }

    inline b3Vector4 Clamp(const b3Vector4& clr, const b3Vector4& a)
    {
        return  b3Vector4((clr[0] > a[0]) ? a[0] : clr[0],
            (clr[1] > a[1]) ? a[1] : clr[1],
            (clr[2] > a[2]) ? a[2] : clr[2],
            (clr[3] > a[3]) ? a[3] : clr[3]);
    }
#endif /*PARTICLE_H_*/
