﻿/*
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
 
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <Component/Array.h>


 
	class   WaterTrochoids
	{
	private:
		static const int NUM_WAVES = 16;

		// Wave data structure
		struct Wave {
			float A;        // amplitude
			float w;        // frequency
			float kx,ky;    // wave vector components
			float kmod;     // wave vector module
			float phi0;     // phase at time 0
			float phase;    // current phase
			float Ainvk;    // amplitude/K module
		};

		std::vector<Wave> _waves;

		// Parameters used for the waves initialization
		float _amplitude;    // base amplitude (must be <1.0 or they loop over themselves)
		float _amplitudeMul; // amplitude multiplier
		float _lambda0;      // base wavelength
		float _lambdaMul;    // wavelength multiplier
		float _direction;    // main waves direction
		float _angleDev;     // deviation from main direction

	public:
		WaterTrochoids( void );

		WaterTrochoids( const WaterTrochoids& copy );

		WaterTrochoids(float amplitude,
			float amplitudeMul,
			float baseWavelen,
			float wavelenMul,
			float direction,
			float angleDev );

		~WaterTrochoids( void );

		// create base waves
		void createWaves( void );

		// update waves
		void updateWaves( float time );

		// pack waves for vertex shader
		void packWaves( FloatArray* constant ) const;

	private:

		inline float nextRandomDouble(float lBound, float uBound) 
		{
			static int seed = 0;
			srand(seed);
			seed++;
			return (float)rand()/(float)RAND_MAX * (uBound - lBound) + lBound;
		}
	};
 
