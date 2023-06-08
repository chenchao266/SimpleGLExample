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
#include <Component/TextureRectangle.h>

 
	class   ScreenAlignedQuad : public GeometryShape
	{
	public:
		ScreenAlignedQuad( void );
		ScreenAlignedQuad( const Vec3f& corner, const Vec2f& dims, const Vec2s& textureSize );
		ScreenAlignedQuad( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture );
		ScreenAlignedQuad( const ScreenAlignedQuad &copy );
		
		virtual void build( const Vec3f& corner, const Vec2f& dims, const Vec2s& textureSize );
		virtual void build( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture );
         
		virtual ~ScreenAlignedQuad(void){};
	};
 
