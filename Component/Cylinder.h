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
 

class Cylinder : public GeometryShape
{
protected:
	std::shared_ptr<Vec4Array> _colorArray;
    float _radius;
    float _height;
    unsigned _steps;
    bool _hasTop;
    bool _hasBottom;

public:
	Cylinder( void );

	Cylinder( float radius, float height, unsigned int steps, bool hasTop, bool hasBottom );

	Cylinder( const Cylinder& copy );
     
	virtual ~Cylinder(void);

public:
	virtual void build( float radius, float height, unsigned int steps, bool top, bool bottom  );
	virtual void build( void );

	void setColor(const Vec4f& color);

    inline const float getRadius( void ) const{
        return _radius;
    }

    inline const float getHeight( void ) const{
        return _height;
    }

    inline const unsigned getSteps( void ) const{
        return _steps;
    }

    inline const bool hasTop( void ) const{
        return _hasTop;
    }

    inline const bool hasBottom( void ) const{
        return _hasBottom;
    }
};


