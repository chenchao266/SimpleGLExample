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
#include <Component/GeometryShape.h>
 
#include <assert.h>

 
	/** 
	* Custom geometry type used to display mipmapped ocean tiles.
    * Will compute and update primitive indices by calling updatePrimitives().
	*/
	class MipmapGeometryVBO : public GeometryShape
	{
	public:
		int _numLevels;		            /**< Number of mipmap levels in use. */
        int _level;			            /**< Mipmap level - 0 is highest level. */
        int _levelRight;                /**< Level of the tile to the right */
        int _levelBelow;                /**< Level of the tile below */
        unsigned int _maxResolution;    /**< Tile size at level 0 */
        unsigned int _rowLen;			/**< number of elements in a row (_resolution+1) */
        unsigned int _resolution;		/**< Tile size (n^2). */
        unsigned int _resRight;         /**< Size of the tile to the right */
        unsigned int _resBelow;         /**< Size of the tile below */
        float _worldSize;               /**< Size of the tile in metres */
        Vec3f _offset;             /**< Tile position in world coords */
        PrimitiveSetList _mainBody;     /**< Storage for main body primitives */
        PrimitiveSetList _rightBorder;  /**< Storage for right border primitives */
        PrimitiveSetList _belowBorder;  /**< Storage for below border primitives */
        PrimitiveSetList _cornerPiece;  /**< Storage for corner primitives */
	
	public:
		/** 
		* Default constructor. 
		* Initialises all values to 0.
		*/
		MipmapGeometryVBO( void );
	
		/** 
		* Constructor. 
		* @param numLevels Total number of mipmap levels in use.
        * @param worldSize Size of tile in world coords (metres)
		*/
		MipmapGeometryVBO( unsigned int numLevels, float worldSize );

		MipmapGeometryVBO( const MipmapGeometryVBO& copy );
         
        /**
        * Updates the tiles primitives if there has been a change in mipmap level.
        * @return true if an update has occurred, false if no update was necessary.
        */
        bool updatePrimitives( unsigned int level, unsigned int levelRight, unsigned int levelBelow );

        inline void initialiseArrays( Array* vertices, Array* normals )
        {
            if( vertices && normals )
            {
                setVertexArray( vertices );
                setNormalArray( normals );
            }
        }
         
		virtual ~MipmapGeometryVBO( void );

    private: 
        /**
        * Add primitives for the main body 
        */
        void addMainBody( void );

        void addZeroTile( void );

        /** 
        * Add primitives for the bottom border 
        */
        void addBottomBorder( void );
        
        /** 
        * Add primitives for the right border
        */
        void addRightBorder( void );
        
        /** 
        * Add primitives for the corner piece 
        */
        void addCornerPiece( void );
        
        /** 
        * Add corner piece for a tile of resolution 1 - special case
        */
        void addZeroCornerPiece(void);
        
        /** 
        * Inserts the primitive component lists (border,corner,border) into the main primitive list.
        */
        void assignPrimitives(void);

        /**
        * Checks to see if an update to the primitive set is required based on the position and level of the tile.
        */
        bool checkPrimitives( unsigned int level, unsigned int levelRight, unsigned int levelBelow );
        
        /** 
        * Computes the resolution of a tile based on its mipmap level ID. 
        */
        inline unsigned int calcResolution( unsigned level, unsigned numLevels ) const {
            return level != (numLevels-1) ? 2 << (numLevels-(level+2) ) : 1;    
        }
        
        /** 
        * Convenience method for getting the index of an element within the vertex array.
        */
        inline unsigned int getIndex( unsigned c, unsigned r ){
            
            int test = c+r*(_maxResolution+1);
            assert( test < (int)((_maxResolution+1)*(_maxResolution+1)) );
            
            return c+r*(_maxResolution+1);
        }

	public:

        /** 
        * Rather than use standard computeBound() this computes the bounding box
        * using the offset and the worldSize.
        */
 
         void computeBoundingBox(Vec3f&, Vec3f&) const;
 
        /** 
        * Set the tile offset for shader positioning.
        * Calls dirtyBound() on its parents and recomputes recomputes
        * the bounding box using a custom computeBound(). 
        */
        inline void setOffset( const Vec3f& offset ){
            _offset = offset;
            if( getColorArray() ){
                Vec4Array* colors = static_cast<Vec4Array*>(getColorArray());
                colors->at(0) = Vec4f( _offset.x, _offset.y, _offset.z, 1.f );
            }
            else{
                Vec4Array* colors = new Vec4Array;
                colors->push_back( Vec4f( _offset.x, _offset.y, _offset.z, 1.f ) );
                setColorArray(colors, Array::BIND_OVERALL);
            }
            
            dirtyBound();
 
            ///setBound( computeBoundingBox() );
 
        }

        /** 
        * Sets the level of the mipmap tile. 
		* Automatically updates the resolution of the tile.
		*/
		inline void setLevel( unsigned int level )
		{
			_level = level;
			_resolution = calcResolution( level, _numLevels );
            _rowLen = _resolution+1;
		}

        inline int getLevel( void ) const { 
            return _level;
        }

        inline int getResolution( void ) const {
            return _resolution;
        }

        inline unsigned int getRowLen( void ) const {
            return _rowLen;
        }
	};
 