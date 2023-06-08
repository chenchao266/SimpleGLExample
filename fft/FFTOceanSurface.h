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
#include "MipmapGeometry.h" 
#include "OceanTile.h"

#include <Component/Timer.h>
 
#include <Component/CullVisitor.h>
struct OceanSurfaceProgram;
    /** 
    * Creates and manages the ocean surface geometry and rendering. 
    * Uses a modified geomipmapping algorithm to provide level of detail.
    * LOD is managed automatically within the update and cull traversals.
    */
    class  FFTOceanSurface : public FFTOceanTechnique
    {
    private:
        unsigned int _totalPoints;                      /**< Total number of points on width/height. */ 
        unsigned int _numVertices;                      /**< Total number of vertices in array. */
        unsigned int _newNumVertices;                   /**< Number of vertices after updateMipmaps is called */

        std::shared_ptr<Vec3Array> _activeVertices;   /**< Active vertex buffer. */
        std::shared_ptr<Vec3Array> _activeNormals;    /**< Active normal buffer. */

        std::vector< std::vector<OceanTile> > _mipmapData;                      /**< Wave tile data. */
        std::vector< std::vector< std::shared_ptr<MipmapGeometry> > > _mipmapGeom;  /**< Geometry tiles. */

    public:
        FFTOceanSurface(unsigned int FFTGridSize = 64,
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

        FFTOceanSurface( const FFTOceanSurface& copy );
          
        virtual ~FFTOceanSurface(void);

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

        /**
        * Computes and assigns mipmap primitives to the geometry.
        */
        void computePrimitives( void );

        /**
        * Copies vertices needs for the tiles into _activeVertices array.
        */
        void computeVertices( unsigned int frame );
        
        /**
        * Checks for any changes in mipmap resolution based on eye position.
        * @return true if any updates have occured.
        */
        bool updateMipmaps( const Vec3f& eye, unsigned int frame );

        /**
        * Adds primitives for main body of vertices.
        */
        void addMainBody( MipmapGeometry* cTile );

        /**
        * Adds primitives for the right skirt.
        */
        void addRightBorder ( MipmapGeometry* cTile, MipmapGeometry* xTile );

        /**
        * Adds primitives for the bottom skirt.
        */
        void addBottomBorder( MipmapGeometry* cTile, MipmapGeometry* yTile );

        /**
        * Adds primitives for the corner piece.
        */
        void addCornerPatch( MipmapGeometry* cTile, MipmapGeometry* xTile, MipmapGeometry* yTile, MipmapGeometry* xyTile );

        /**
        * Adds primitives for main body of the lowest resolution tile (2x2 vertices).
        * This is a special case treated similar to a corner piece.
        */
        void addMaxDistMainBody( MipmapGeometry* cTile, MipmapGeometry* xTile, MipmapGeometry* yTile, MipmapGeometry* xyTile );

        /**
        * Adds primitives for edge of the lowest resolution tile (2x2 vertices).
        */
        void addMaxDistEdge( MipmapGeometry* cTile, MipmapGeometry* xTile, MipmapGeometry* yTile );

        /**
        * Compute noise coordinates for the fragment shader.
        * @param noiseSize Size of noise tile (m).
        * @param movement Number of tiles moved x,y.
        * @param speed Speed of movement(m/s).
        * @parem time Simulation Time.
        */
        Vec3f computeNoiseCoords(float noiseSize, const Vec2f& movement, float speed, float time);

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
        inline MipmapGeometry* getTile( unsigned int x, unsigned int y ){    
            return _mipmapGeom.at(y).at(x).get();
        }

        /** 
        * Convenience method for loading the ocean shader. 
        * @return NULL if shader files were not found
        */
        void createShader(void);
        OceanSurfaceProgram* m_prog;
    };
 