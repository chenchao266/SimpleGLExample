/**************************************************************************
	ORIGINAL AUTHOR: 
		Emud Mokhberi (emud@ucla.edu)
	MODIFIED BY:
	
	CONTRIBUTORS:
	

-----------------------------------------------
	
 ***************************************************************
 ******General License Agreement and Lack of Warranty ***********
 ****************************************************************

 This software is distributed for noncommercial use in the hope that it will 
 be useful but WITHOUT ANY WARRANTY. The author(s) do not accept responsibility
 to anyone for the consequences of using it or for whether it serves any 
 particular purpose or works at all. No guarantee is made about the software 
 or its performance.

 You are allowed to modify the source code, add your name to the
 appropriate list above and distribute the code as long as 
 this license agreement is distributed with the code and is included at 
 the top of all header (.h) files.

 Commercial use is strictly prohibited.
***************************************************************************/


#ifndef MAIN2D_H_
#define MAIN2D_H_ 1

#include <string>
 

#include <iostream>
 

#include <iomanip>
 

#include <algorithm>
 
#include <vector>
 
#include <list>
 
#include <set>
 
#include <map>
 
#include <deque>
  
#include <math.h>
 

#include <time.h>
 


#include <assert.h>
#include <fstream>
 

#include <sstream>
#include <iomanip>
#include <stdio.h>

//------------------------------CONSTANTS---------------------------------------------
typedef double Float;
typedef double Double;
 
const static Float HALF_PI = 2.0 * atan(1.0);
const static Float _PI = 4.0 * atan(1.0);
const static Float TWO_PI = 8.0 * atan(1.0);
const static Float INV_PI = 1.0/_PI;
const static Float INV_TWO_PI = 1.0/TWO_PI;
const static Float PI_INV180 = _PI   / 180.;
const static Float INV_PI180 = INV_PI * 180;

const static Float ONE_THIRD = 1./3.;
const static Float INV_255 = 1. / 255;
const static Float SQRT_TWO = sqrt(2.);
//-------------------------2D LEVEL SET CONSTANTS---------------------------------------------
 
const static Double HH = 1;
const static int  PARTICLES_PER_NODE2D = 16;
const static int  PARTICLES_PER_NODE = 32;
const static int  NX = 100;
const static int  NY = 100;
const static int  NZ = 100;

const static Float MAX_U               = NX * 0.005;
const static Float MAX_V               = NY * 0.005;
const static Float MAX_W               = NZ * 0.005;
const static Float RADIUS_MIN			= 0.1;
const static Float RADIUS_MAX			= 0.5;
const static Float RESEED_THRESHOLD	= 2.0 * HH;
//const static Float PARTICLE_DELETE		= 1.5 * RADIUS_MIN;
const static Float PARTICLE_DELETE		= 100 * RADIUS_MIN;
const static Float DT2D				= 4.9 / ((MAX_U + MAX_V) / HH);
const static Float DT					= 4.9 / ((MAX_U + MAX_V + MAX_W) / HH);
const static Float FASTMARCH_LIMIT		= 6.0 * HH; // extent of influence of fast marching
const static Float SEMILAGRA_LIMIT		= 5.0 * HH; // extent of influence of semi-lagrangian
//const static Float SEMILAGRA_LIMIT		= 100.0 * HH; // extent of influence of semi-lagrangian
const static int PARTICLES_PER_INTERFACE_NODE = 2 * PARTICLES_PER_NODE;

#define SAMPLEPHI                  LinearSample      
//#define SAMPLEPHI                  CubicSample
//#define FOR_LS2D for(int j=1; j<=NY; j++) { for(int i=1; i<=NX; i++) {
//#define FOR_ALL_LS2D for(int j=1; j<(NY+2); j++) { for(int i=1; i<(NX+2); i++) {
//#define FOR_LS     for(int k=1; k<=NZ; k++) { for(int j=1; j<=NY; j++) { for(int i=1; i<=NX; i++) {
//#define FOR_ALL_LS for(int k=0; k<(NZ+2); k++) { for(int j=0; j<(NY+2); j++) { for(int i=0; i<(NX+2); i++) {
//#define END_FOR_THREE }}}
//#define END_FOR_TWO }}
//#define FOR_GRID2D  for(int i=0; i < size; i++)
//#define FOR_GRID   for(int i=0; i < size; i++)
//#define FOR_GRIDZY for(int k=0; k <= Nz; k++) { for(int j=0; j <= Ny; j++) { 
//#define FOR_GRIDZX for(int k=0; k <= Nz; k++) { for(int i=0; i <= Nx; i++) { 
//#define FOR_GRIDYX for(int j=0; j <= Ny; j++) { for(int i=0; i <= Nx; i++) { 

class FastMarch2D;
class Grid2D;
class LevelSet2D;
class Particle2D;
class ParticleSet2D;
class Timer;
 
class Velocity2D;
class Water2D;

class FastMarch;
class Grid;
class LevelSet;
class Particle;
class ParticleSet;
class Timer;
 
class Velocity;
class Water;
 
template<typename T>
inline bool CmpFtoZero(T compare) {
    if((compare < 0.00001) && (compare > -0.00001)) return true;
    else return false;
}
  
// Monotonic Cubic Interpolation
// interpolation between fk1 and fk2. alpha = position - k1;
template<typename T>
inline T MCerp(const T &alpha, const T &fk0, const T &fk1,
                                       const T &fk2, const T &fk3)
{
    static T dk02, dk13, delk;
    delk = fk2 - fk1;
    if(CmpFtoZero(delk)) dk02 = dk13 = 0;
    else {
        dk02 = 0.5 * (fk2 - fk0);
        dk13 = 0.5 * (fk3 - fk1);
        if(dk02 * delk < 0.) dk02 = 0;
        if(dk13 * delk < 0.) dk13 = 0;
    }
    return (dk02 + dk13 - 2 * delk) * alpha * alpha * alpha 
            + (3. * delk - 2. * dk02 - dk13) * alpha * alpha + dk02 * alpha + fk1;
}
template<typename T>
inline T Lerp(T alpha, T a, T b) { return (1.0 - alpha) * a + alpha * b; }

template<typename T>
inline int Step(T x, T alpha) { if(alpha < x) return 0; return 1; }
template<typename T>
inline int Round(T alpha) { return (int)floor(alpha+0.5); }
template<typename T>
inline T square(T x) { return x*x; }
template<typename T>
inline T Radians(T deg) {return PI_INV180 * deg; }
template<typename T>
inline T Degrees(T rad) {return INV_PI180 * rad; }
template<typename T>
inline T Clamp(T alpha, T a, T b) { 
	if(alpha < a) return a;
	if(alpha > b) return b;
	return alpha;
}
template<typename T>
inline T SmoothStep(T min, T max, T alpha) {
	T temp = Clamp((alpha - min) / (max - min), (T)0.0, (T)1.0);
	return -2.0 * temp * temp * temp + 3.0 * temp * temp;
}

template<typename T>
inline int Mod(int a, int b, const T &bInv) {
	int n = int(a*bInv);
	a -= n*b;
	if (a < 0)
		a += b;
	return a;
}

//----------------need random code for this - else take it out------------------------------
extern Float genrand();
extern unsigned long genrandlong();

inline Float RandomFloat(Float min = 0.0, Float max = 1.0) {
	return Lerp(genrand(), min, max);
}

inline unsigned long RandomInt() {
	return genrandlong();
}

#endif