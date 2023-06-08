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
 
#include "FFTOceanTechnique.h"
#include "MipmapGeometryVBO.h" 

#include <Component/Timer.h>
 
#include <Component/CullVisitor.h>
struct OceanSurfaceVBOProgram;
 
    /** 
    * Creates and manages the ocean surface geometry and rendering. 
    * Uses a modified geomipmapping algorithm to provide level of detail.
    * LOD is managed automatically within the update and cull traversals.
    */
    class  FFTOceanSurfaceVBO : public FFTOceanTechnique
    {
    private:
        std::shared_ptr<Vec3Array> _masterVertices;
        std::shared_ptr<Vec3Array> _masterNormals;

        std::vector< OceanTile > _mipmapData;
        std::vector< std::vector< std::shared_ptr<MipmapGeometryVBO> > > _mipmapGeom;  /**< Geometry tiles. */

    public:
        FFTOceanSurfaceVBO(unsigned int FFTGridSize = 64,
            unsigned int resolution = 256,
            unsigned int numTiles = 17, 
            const Vec2f& windDirection = Vec2f(1.1f, 1.1f),
            float windSpeed = 12.f,
            float depth = 1000.f,
            float reflectionDamping = 0.35f,
            float waveScale = 1e-8f,
            bool isChoppy = true,
            float choppyFactor = -2.5f,
            float animLoopTime = 10.f,
            unsigned int numFrames = 256 );

        FFTOceanSurfaceVBO( const FFTOceanSurfaceVBO& copy  );
         
        virtual ~FFTOceanSurfaceVBO(void);

    public:
        
        float getSurfaceHeightAt(float x, float y, Vec3f* normal = NULL);

        /**
        * Checks for mipmap or frame changes and updates the geometry accordingly.
        * Will rebuild state or geometry if found to be dirty.
        */
        void update( unsigned int frame, const double& dt, const Vec3f& eye );

        /**
        * Sets up geometry and mipmaping data.
        * Forces stateset rebuid.
        */
        void build( void );

        void setMinDistances(std::vector<float> &minDistances);

    private:
        /**
        * Creates ocean surface stateset. 
        * Loads shaders and adds uniforms and textures;
        */
        void initStateSet( void );

        /**
        * Computes the ocean FFTs and stores vertices/normals for mipmap levels.
        */
        void computeSea( unsigned int totalFrames );

        /**
        * Sets up with the ocean surface with mipmap geometry.
        */
        void createOceanTiles( void );

        void updateVertices(unsigned int frame);

        bool updateLevels(const Vec3f& eye);

        /**
        * Compute noise coordinates for the fragment shader.
        * @param noiseSize Size of noise tile (m).
        * @param movement Number of tiles moved x,y.
        * @param speed Speed of movement(m/s).
        * @parem time Simulation Time.
        */
        Vec3f computeNoiseCoords(float noiseSize, const Vec2f& movement, float speed, double time);

        /**
        * Creates a custom DOT3 noise map for the ocean surface.
        * This will execute an FFT to generate a height field from which the normal map is generated.
        * Default behaviour is to create a normal map using the params from the ocean geometry setup.
        */
        std::shared_ptr<Texture2D> createNoiseMap( unsigned int FFTSize, 
            const Vec2f& windDir, 
            float windSpeed, 
            float waveScale,
            float tileResolution );

        /** 
        * Convenience method for retrieving mipmap geometry from _oceanGeom. 
        */
        inline MipmapGeometryVBO* getTile( unsigned int x, unsigned int y ){    
            return _mipmapGeom.at(y).at(x).get();
        }

        /** 
        * Convenience method for loading the ocean shader. 
        * @return NULL if shader files were not found
        */
        void createShader(void);
        OceanSurfaceVBOProgram* m_prog;
    };
 