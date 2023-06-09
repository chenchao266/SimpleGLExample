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
 
#include <Bullet3Common/geometry.h>
#include <Component/Array.h>

 
    /** Implementation of Jerry Tessendorf's FFT Ocean Simulation
    * For more information see his paper "Simulating Ocean Water"
    * http://graphics.ucsd.edu/courses/rendering/2005/jdewall/tessendorf.pdf
    */
    class  FFTSimulation
    {
    private:
        // Implementation hidden so that clients do not need to depend on the 
        // FFT library's headers. All calls to FFTSimulation are delegated to
        // the private Implementation class.
        class Implementation;
        Implementation* _implementation;

    public:

        /** Constructor.
        * Provides default parameters for a calm ocean surface.
        * Computes base amplitudes and initialises FFT plans and arrays.
        * @param fourierSize Size of FFT grid 2^n ie 128,64,32 etc.
        * @param windDir Direction of wind.
        * @param windSpeed Speed of wind (m/s).
        * @param waveScale Wave height modifier.
        * @param loopTime Time for animation to repeat (secs).
        */
        FFTSimulation(
            int fourierSize = 64,
            const Vec2f& windDir = Vec2f(1.0f, 1.0f),
            float windSpeed  = 12.f,
            float depth = 1000.f,
            float reflectionDamping = 0.35f,
            float waveScale = 1e-9,    
            float tileRes = 256.f,
            float loopTime  = 10.f
            );

        /** Destructor.
        * Cleans up FFT plans and arrays.
        */
       virtual ~FFTSimulation(void);

        /** Set the current time and computes the current fourier amplitudes */
        void setTime(float time);    

        /** Compute the current height field. 
        * Executes an FFT transform to convert the current fourier amplitudes to real height values.
        * @param heights must be created before passing in. Function will resize and overwrite the contents with current data.
        */
        void computeHeights( FloatArray* heights ) const;

        /** Compute the (x,y) displacements for choppy wave effect
        * Executes two FFT transforms to convert the current fourier amplitudes to real x,y displacements
        * @param scaleFactor defines the magnitude of the displacements. Typically a negative value ( -3.0 > val < -1.0 ).
        * @param waveDisplacements must be created before passing in. Function will resize and overwrite the contents with the computed displacements.
        */
        void computeDisplacements( const float& scaleFactor, Vec2Array* waveDisplacements ) const;
    };
 