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

 
	/** 
	* Stores mipmap geometry and associated mipmap level data. 
	*/
	class   MipmapGeometry : public GeometryShape
	{
	public:
		enum BORDER_TYPE{ BORDER_X, BORDER_Y, BORDER_XY, BORDER_NONE };

	private:
		unsigned int _level;				/**< Mipmap level 0 is highest level. */
		unsigned int _numLevels;		/**< Number of mipmap levels in use. */
		unsigned int _resolution;		/**< Tile size (n^2). */
		unsigned int _rowLen;			/**< Length of rows. */
		unsigned int _colLen;			/**< Length of cols. */
		unsigned int _startIdx;			/**< Start position in vertex array. */
		
		BORDER_TYPE	 _border;			/**< is the patch a border piece. */
	
	public:
		/** 
		* Default constructor. 
		* Initialises all values to 0.
		*/
		MipmapGeometry( void );
	
		/** 
		* Constructor. 
		* @param level Tile level.
		* @param numLevels Number of levels in the mipmap sequence.
		* @param startIdx The position of the first vertex in the main vertex array.
		* @param border Tiles border type.
		*/
		MipmapGeometry( unsigned int level,
							 unsigned int numLevels, 
							 unsigned int startIdx,
							 BORDER_TYPE border ); 

		MipmapGeometry( const MipmapGeometry& copy );
  
		virtual ~MipmapGeometry( void );

	public:
		inline unsigned int getLevel( void ) const { 
			return _level;
		}

		inline unsigned int getResolution( void ) const {
			return _resolution;
		}

		inline unsigned int getRowLen( void ) const {
			return _rowLen;
		}

		inline unsigned int getColLen( void ) const {
			return _colLen;
		}
		
		inline BORDER_TYPE getBorder( void ) const {
			return _border;
		}

		/** 
		* Sets the level of the mipmap tile. 
		* Automatically updates the resolution, row and column lengths of the tile.
		*/
		inline void setLevel( unsigned int level )
		{
			_level = level;
			_resolution = level != (_numLevels-1) ? 2 << (_numLevels-(level+2) ) : 1;
			_rowLen = _border==BORDER_X || _border==BORDER_XY ? _resolution+1 : _resolution;
			_colLen = _border==BORDER_Y || _border==BORDER_XY ? _resolution+1 : _resolution;
		}

		inline const PrimitiveSetList& getPrimitives( void ) const {
			return _primitives;
		}

		inline unsigned int getIdx( void ) const {
			return _startIdx;
		}

		inline void setIdx( unsigned int idx ) {
			_startIdx = idx;
		}

		inline unsigned int getIndex( unsigned int c, unsigned r )
		{
			return _startIdx + (c + r * _rowLen);
		}
	};
 
