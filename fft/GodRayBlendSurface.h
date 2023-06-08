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
#include "ScreenAlignedQuad.h"
#include <Component/GeometryShape.h>
#include <Component/MultiGeometryShape.h>
 
#include <string>

struct GodRayBlendProgram;
    /** 
    * Screen aligned quad used to display the god ray frame buffer.
    */
    class  GodRayBlendSurface : public MultiGeometryShape
    {
    private:
        Vec3f   _HGg;                          /**< Controls degree of forward light scattering. */
        Vec3f  _sunDir;                       /**< sun direction */
        float       _intensity;                    /**< Intensity tweak */
        GLuint _stateset;
        std::shared_ptr<Vec3Array> _normalArray; /**< Stores the ray vectors for the view direction at the corners of the screen aligned quad */

    public:
        /** 
        * Default Constructor.
        */
        GodRayBlendSurface( void );
        
        /** 
        * Constructor.
        * Calls build().
        */
        GodRayBlendSurface( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture );
        
        GodRayBlendSurface( const GodRayBlendSurface &copy );
         
    protected:
        ~GodRayBlendSurface(void){};

    public:
        /** 
        * Removes all drawables and creates and adds quad geometry, program and stateset. 
        */
        void build( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture );
    
        /**
        * Updates the _normalArray with new view vectors.
        */ 
        void update( const Matrixf& view, const Matrixf& proj );

        /**
        * Sets the degree of forward light scattering.
        */
        void setEccentricity( float g );

        /**
        * Blend intensity tweak.
        */
        void setIntensity( float i );

        /**
        * Set sun direction
        */ 
        void setSunDirection( const Vec3f& sunDir );

    private:
        /**
        * Loads the shaders and returns program.
        */ 
        void createShader(void);

    // ---------------------------------------------
    //            Callback declarations
    // ---------------------------------------------

    protected:
        /**
        * Animation data structure.
        */
        class GodRayBlendDataType//: public Referenced
        {
        private:
            GodRayBlendSurface& _blendSurface;
            Matrixf _view;
            Matrixf _projection;

        public:
            GodRayBlendDataType( GodRayBlendSurface& godRays );

            GodRayBlendDataType( const GodRayBlendDataType& copy );

            inline void setViewMatrix( const Matrixf& viewMatrix ){
                _view=viewMatrix;
            }
            inline void setProjMatrix( const Matrixf& projMatrix ){
                _projection=projMatrix;
            }

            void update( void );
        };

        /**
        * Update/Cull animation callback.
        * Cull callback stores view and projection matrices needed for animation.
        * Update callback calls parents update function.
        * @see GodRayBlendSurface::update().
        */
        GodRayBlendProgram* m_prog;
            virtual void cull(CullVisitor* nv);
            virtual void update(unsigned frameNum, double currentTime);
       
    };

