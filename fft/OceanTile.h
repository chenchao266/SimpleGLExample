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
 
#include <Component/Array.h>
#include <memory>
class Texture2D;
 
    /** 
    * Stores vertices and normals for the geomipmp levels.
    */
    class   OceanTile
    {
    private:
        unsigned int _resolution;               /**< FFT Size (2^n). */
        unsigned int _rowLength;                /**< Row length the FFT size with skirt (N+1). */
        unsigned int _numVertices;              /**< Total number of vertices _rowLength^2. */
        std::shared_ptr<Vec3Array> _vertices; /**< Vertex array. */
        std::shared_ptr<Vec3Array> _normals;  /**< Normal array. */
        float _spacing;                         /**< Vertex spacing. */
        float _maxDelta;                        /**< Max change in height between levels */
        float _averageHeight;                   /**< Average height (z) of vertices */
        float _maxHeight;                       /**< Maximum height (z) of vertices */
        bool  _useVBO;                          /**< Add relative position to tile placement */
        
    public:
        /** 
        * Default constructor.
        * Initialises values to 0.
        */
        OceanTile( void );

        /** 
        * Constructor.
        * Copies heights into _vertices adding an extra row and column as a skirt, size: (N+1)*(N+1).
        * Data from the first row and column are copied into the last column and row.
        * Computes average height and normals of tile. Displacements are optional.
        */
        OceanTile( FloatArray* heights, 
                   const unsigned int resolution, 
                   const float spacing,
                   Vec2Array* displacements = NULL,
                   bool useVBO = false);

        /** 
        * Down sampling constructor.
        * Down samples the passed OceanTile data and populates _vertices adding a skirt.
        * Down sampled vertices are averages of the surrounding 4 vertices.
        */
        OceanTile( const OceanTile& tile, 
                   unsigned int resolution, 
                   const float spacing );

        /**
        * Copy constructor.
        */
        OceanTile( const OceanTile& copy );

        OceanTile& operator=(const OceanTile& rhs);

        ~OceanTile( void );

        /** 
        * Creates a DOT3 normal map based on the heightfield.
        * @return Texture2D (size: _tileResolution*_tileResolution).
        */

        std::shared_ptr<Texture2D> createNormalMap( void );

        inline Vec3Array *getVertices( void ) const
        {
            return _vertices.get();
        }
        inline Vec3Array *getNormals( void ) const
        {
            return _normals.get();
        }
        inline const Vec3f& getVertex( unsigned int x, unsigned int y ) const    {
            return _vertices->at( x + y * _rowLength );
        }

        inline const Vec3f& getVertex( unsigned int v ) const{
            return _vertices->at(v);
        }

        inline const Vec3f& getNormal( unsigned int x, unsigned int y ) const{
            return _normals->at( x + y * _rowLength );
        }

        inline const Vec3f& getNormal( unsigned int n ) const{
            return _normals->at(n);
        }

        inline const unsigned int& getNumVertices( void ) const{
            return _numVertices;
        }

        inline const unsigned int& getResolution( void ) const{
            return _resolution;
        }

        inline const unsigned int& getRowLen( void ) const {
            return _rowLength;
        }

        inline const float& getSpacing( void ) const {
            return _spacing;
        }

        inline const float getMaxDelta( void ) const {
            return _maxDelta;
        }

        inline const float getAverageHeight(void) const{
            return _averageHeight;
        }

        inline const float getMaximumHeight(void) const{
            return _maxHeight;
        }

        inline const bool getUseVBO(void) const {
            return _useVBO;
        }

        float biLinearInterp(float x, float y ) const;

        Vec3f normalBiLinearInterp(float x, float y ) const;

    private:

        /** Compute normals for an N+2 x N+2 grid to ensure continuous normals around the edges.
        * Using first row as last row and first column as last column.
        * Discards first row and column when storing internally.
        */
        void computeNormals( void );

        /** Computes the maximum difference in height between vertices from differant mipmap levels.
        * Not used at the moment as the geometry data is constantly changing due to the animation.
        * It may be possible to use an average delta value to make the mipmapping smoother.
        * More info: http://www.flipcode.com/archives/article_geomipmaps.pdf
        */
        void computeMaxDelta( void );

        /** Bilinear interpolation between 4 hi-res points and a lower level counter part.
        * Not used at the moment.
        * @see computeMaxDelta();
        */
        float biLinearInterp(int lx, int hx, int ly, int hy, int tx, int ty ) const;

        /** Convenience method for computing array position. */
        inline unsigned int array_pos( unsigned int x, unsigned int y, unsigned int rowLen )
        {
            return x + y * rowLen;
        }
    };
 
