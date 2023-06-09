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
 
#include <stdlib.h>
#include <cmath>

 
    /** 
    * A selection of random number generator utility functions 
    **/
    namespace RandUtils
    {
        /** uniformly distributed random int (0 -> RAND_MAX) */
        inline static int myRand( void )
        {
            return rand();
        }

        /** uniformly distributed random number (0 -> 1) */
        inline float unitRand( void )
        {
            return (float)myRand()/RAND_MAX;
        }

        /** uniformly distributed random number (min -> max) */
        inline float rangedRand( float min, float max )
        {
            return min + unitRand() * (max - min);
        }

        /** Gaussian distributed random number pair 
        * http://www.taygeta.com/random/gaussian.html 
        */
        inline void gaussianRand( float& a, float& b )
        {
            float x1, x2, length2;

            do {
                x1 = rangedRand(-1.f,1.f);
                x2 = rangedRand(-1.f,1.f);
                length2 = x1 * x1 + x2 * x2;
            } 
            while ( length2 >= 1.f );

            length2 = sqrt( (-2.f * log( length2 ) ) / length2 );

            a = x1 * length2;
            b = x2 * length2;
        }
    }
 
