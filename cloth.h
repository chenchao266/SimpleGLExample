//////////////////////////////////////////////////////////////////////////////////////////
//	BALL.h
//	Class for a ball in our "ball and spring" cloth model
//	Downloaded from: www.paulsprojects.net
//	Created:	12th January 2003
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef cloth_H
#define cloth_H
#include "Bullet3Common/geometry.h"
//Make sure these structures are packed to a multiple of 4 bytes (sizeof(float)),
//since glMap2 takes strides as integer multiples of 4 bytes.
//How consistent..
#pragma pack(push)
#pragma pack(4)

class BALL
{
public:
	Vec3f position;
	Vec3f velocity;

	float mass;

	//Is this ball held in position?
	bool fixed;

	//Vertex normal for this ball
	Vec3f normal;
};

#pragma pack(pop)

class SPRING
{
public:
	//Indices of the balls at either end of the spring
	int ball1;
	int ball2;

	//Tension in the spring
	float tension;

	float springConstant;
	float naturalLength;

	SPRING()	:	ball1(-1), ball2(-1)
	{}
	~SPRING()
	{}
};

#endif	//BALL_H
