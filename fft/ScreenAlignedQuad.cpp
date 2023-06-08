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

#include "ScreenAlignedQuad.h"
 
ScreenAlignedQuad::ScreenAlignedQuad(void)
{
}

ScreenAlignedQuad::ScreenAlignedQuad( const Vec3f& corner, const Vec2f& dims, const Vec2s& textureSize )
{
	build( corner, dims, textureSize );
}

ScreenAlignedQuad::ScreenAlignedQuad( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture )
{
	if(texture)
		build( corner, dims, texture );
}

ScreenAlignedQuad::ScreenAlignedQuad( const ScreenAlignedQuad &copy):
	GeometryShape(copy)
{
}

void ScreenAlignedQuad::build( const Vec3f& corner, const Vec2f& dims, const Vec2s& textureSize )
{
	Vec3Array* coords = new Vec3Array(4);
	(*coords)[0] = corner+Vec3f(0.f, dims.y, 0.f);
	(*coords)[1] = corner;
	(*coords)[2] = corner+Vec3f(dims.x, 0.f, 0.f);
	(*coords)[3] = corner+Vec3f(dims.x, dims.y, 0.f);
	setVertexArray(coords);

	Vec2Array* tcoords = new Vec2Array(4);
	(*tcoords)[0] = Vec2f(0.f,             textureSize.y);
	(*tcoords)[1] = Vec2f(0.f,             0.f);
	(*tcoords)[2] = Vec2f(textureSize.x, 0.f);
	(*tcoords)[3] = Vec2f(textureSize.x, textureSize.y);
	setTexCoordArray(0,tcoords, Array::BIND_PER_VERTEX);

	Vec4Array* colours = new Vec4Array(1);
	(*colours)[0]=Vec4f(1.0f,1.0f,1.0,1.0f);
	setColorArray(colours, Array::BIND_OVERALL);

	Vec3Array* normals = new Vec3Array(1);
	(*normals)[0] = Vec3f(0.f, -1.f, 0.f);
	(*normals)[0].normalize();
	setNormalArray(normals, Array::BIND_OVERALL);
     
	addPrimitiveSet(new DrawElementsUShort(DrawElements::QUADS,0,4));

	//setMode(GL_LIGHTING, OFF);
	//setMode(GL_DEPTH_TEST, OFF );
}

void ScreenAlignedQuad::build( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture )
{
	if(texture)
	{
		Vec2s texSize( texture->getTextureWidth(), texture->getTextureHeight() );
		build(corner, dims, texSize);
	}
}
