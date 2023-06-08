/*
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
 
#include <Component/MultiGeometryShape.h>
#include <Component/CullVisitor.h>
 
 
    class  OceanTechnique : public MultiGeometryShape
    {
    public:
        OceanTechnique( void );
        OceanTechnique( const OceanTechnique& copy );
         
        virtual void build( void );

        virtual void stopAnimation( void ){
            _isAnimating = false;
        }

        virtual void startAnimation( void ){
            _isAnimating = true;
        }

        bool isAnimating( void ) const{
            return _isAnimating;
        }

        /**
        * Returns the average height over the whole surface (in local space)
        */
        virtual float getSurfaceHeight(void) const;

        /**
        * Returns the height at the given point (in local space). If the 
        * address of a Vec3f is passed in the normal argument, the normal at
        * that position will be calculated and stored in it.
        */
        virtual float getSurfaceHeightAt(float x, float y, Vec3f* normal = NULL) = 0;

        /**
        * Returns the maximum height over the whole surface (in local space)
        */
        virtual float getMaximumHeight(void) const;

        inline bool isDirty(void) const{
            return _isDirty;
        }

        inline void dirty(void){
            _isDirty=true;
        }

        /** 
        * Check if the ocean surface is visible or not. Basic test is very
        * simple, subclasses can do a better test.
        */    
        bool isVisible( CullVisitor& cv, bool eyeAboveWater );

        /** Base class for the OceanTechnique event handler. Subclasses of
         *  OceanTechnique can subclass this to provide support for
         *  manipulating their particular properties, calling the base class
         *  handle() to inherit the base class's events (or not as desired).
         *  If subclasses subclass this handler, they need to override
         *  getEventHandler() in order for it to return the right concrete
         *  event handler instance.
         */
 
            virtual bool handle(int key);
   
        virtual ~OceanTechnique(void){};

        /**
        * Add the default resource paths to osgDB::Registry.
        * Checks if already present.
        * paths: resources/textures and resources/shaders.
        */
 
    protected:
        bool _isDirty;
        bool _isAnimating;
 
    };
 