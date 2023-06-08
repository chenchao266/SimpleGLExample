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


#include <Component/GeometryShape.h>
#include <Component/MultiGeometryShape.h>

#include <Component/TextureRectangle.h>

struct DistortionSurfaceProgram;
    class   DistortionSurface : public MultiGeometryShape
    {
    public:
        DistortionSurface( void );

        DistortionSurface( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture );

        DistortionSurface( const DistortionSurface &copy );
         
        void build( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture );

        void update(void);

    private:
        float _angle;
         
        ~DistortionSurface( void ){};

        /**
        * Add the default resource paths to osgDB::Registry.
        * Checks if already present.
        * paths: resources/textures and resources/shaders.
        */
 

    private:
        void update(const double& dt);
        void createShader(void);

    private:
        // ---------------------------------------------
        //            Callback declarations
        // ---------------------------------------------

        class DistortionDataType //: public Referenced
        {
        private:
            DistortionSurface& _surface;
            double _oldTime;
            double _newTime;

        public:
            DistortionDataType( DistortionSurface& surface );

            void update( const double& time );
        };

        DistortionSurfaceProgram* m_prog;
        //virtual void cull(CullVisitor* cv);
        virtual void update(unsigned frameNum, double currentTime);
      
    };

