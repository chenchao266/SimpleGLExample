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

#include <Cylinder.h>


Cylinder::Cylinder( void ):
    _radius     (1.f),
    _height     (1.f),
    _steps      (16),
    _hasTop     (true),
    _hasBottom  (true)
{
    build();
}

Cylinder::Cylinder( float radius, float height, unsigned int steps, bool hasTop, bool hasBottom  ) :
	_radius     (radius),
	_height     (height),
	_steps      (steps), 
	_hasTop     (hasTop),
	_hasBottom  (hasBottom)
{
	build();
}

Cylinder::Cylinder( const Cylinder& copy ):
    GeometryShape  (copy),
    _radius        (copy._radius),
    _height        (copy._height),
    _steps         (copy._steps),
    _hasTop        (copy._hasTop),
    _hasBottom     (copy._hasBottom)
{
}

Cylinder::~Cylinder(void)
{
}

void Cylinder::build( float radius, float height, unsigned int steps, bool top, bool bottom )
{
    _radius = radius;
    _height = height;
    _steps = steps;
    _hasTop = top;
    _hasBottom = bottom;

	build();
}

void Cylinder::build( void )
{
    // clear primitives if there are any
    if(getNumPrimitiveSets() > 0)
    {
        removePrimitiveSet(0,getNumPrimitiveSets());
 
    }

	const float twoPI = M_PI * 2.f;
	const float angleInc = twoPI / (float)_steps;

	Vec3Array* vertices = new Vec3Array();

	vertices->push_back( Vec3f() );	// bottom centre

	for ( float angle = 0.f; angle <= twoPI; angle += angleInc )
	{
		float x1 = _radius * cos(angle);
		float y1 = _radius * sin(angle);

		vertices->push_back( Vec3f( x1, y1, 0.f ) );
		vertices->push_back( Vec3f( x1, y1, _height ) );
	}

	vertices->push_back( Vec3f(0.f, 0.f, _height) );	// top centre

	DrawElementsUInt* indices = 
		new DrawElementsUInt( DrawElements::TRIANGLE_STRIP, 0 );

	for(unsigned int c = 1; c < _steps*2+1; c += 2 )
	{	
		indices->push_back( c   );
		indices->push_back( c+1 );
	}

	indices->push_back(1);
	indices->push_back(2);

	addPrimitiveSet( indices );

	if( _hasBottom )
	{
		DrawElementsUInt* fanIndices = 
			new DrawElementsUInt( DrawElements::TRIANGLE_FAN, 0 );

		fanIndices->push_back( 0 );

		for(int c = _steps*2-1; c >= 1; c -= 2 )
		{	
			fanIndices->push_back( c );
		}

		fanIndices->push_back( _steps*2-1 );

		addPrimitiveSet( fanIndices );
	}

	if( _hasTop )
	{
		DrawElementsUInt* fanIndices = 
			new DrawElementsUInt( DrawElements::TRIANGLE_FAN, 0 );

		fanIndices->push_back( vertices->size()-1 );

		for(int c = _steps*2; c >= 1; c -= 2 )
		{	
			fanIndices->push_back( c );
		}

		fanIndices->push_back( _steps*2 );

		addPrimitiveSet( fanIndices );
	}

	_colorArray.reset(new Vec4Array(1));
	(*_colorArray)[0]=Vec4f( 0.f, 0.f, 0.f, 1.0f );

	setVertexArray( vertices );
	setColorArray( _colorArray.get(), Array::BIND_OVERALL );
 
}

void Cylinder::setColor(const Vec4f& color)
{
	if(getColorArray())
	{
		Vec4Array* colors = static_cast<Vec4Array*>( getColorArray() );
		(*_colorArray)[0] = color;
		//setColorBinding( Array::BIND_OVERALL );
 
	}
}
