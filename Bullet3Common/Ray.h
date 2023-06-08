#ifndef __RAY_H__
#define __RAY_H__

#include "Bullet3Common/geometry.h"
 
 
class Ray
{
public:
	Ray(void);
	Ray(const Vec3f& pos, const Vec3f& dir);
	~Ray(void);

	const Vec3f& GetOrigin(void) const { return origin; }
	const Vec3f& GetDirection(void) const { return direction; }

	void Transform(const Matrixf& matrix);

private:
	Vec3f origin;
	Vec3f direction;
};

#endif
