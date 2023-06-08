#include "Camera.h"
#include "glad/gl.h"

Camera::Camera(Float x,Float y,Float z):
    theta(90),phi(0),dirty(false),dir(0,0,-1),pos(x,y,z) { }

void Camera::MoveForward(Float dist)
{
	CalcDir();
	pos+=dir*dist;
}
void Camera::MoveBackward(Float dist)
{
	CalcDir();
	pos-=dir*dist;
}
void Camera::MoveLeft(Float dist)
{
	CalcDir();
	pos-=right*dist;
}
void Camera::MoveRight(Float dist)
{
	CalcDir();
	pos+=right*dist;
}
void Camera::MoveUp(Float dist)
{
	CalcDir();
	pos+=up*dist;
}
void Camera::MoveDown(Float dist)
{
	CalcDir();
	pos-=up*dist;
}

void Camera::RotateY(Float degrees)
{
	dirty=true;
	theta+=degrees;
}
void Camera::RotateX(Float degrees)
{
	dirty=true;
	phi+=degrees;
    phi=std::max(-85.0,phi);
    phi= std::min(85.0,phi);
}


Vec3d Camera::GetDir()
{
    CalcDir();
    return dir;
}

void Camera::Draw()
{
	CalcDir();
	Vec3d at = pos + dir*100;
	//gluLookAt(pos[0],pos[1],pos[2],at[0],at[1],at[2],up[0],up[1],up[2]);
}

Vec3d Camera::GetPos()
{
    CalcDir();
    return pos;
}

void Camera::CalcDir()
{
	if(!dirty) 
		return;
	Float rphi=Radians(phi);
	Float rtheta=Radians(theta);
    Float r=cos(rphi);

	dir[0]=r*cos(rtheta);
	dir[1]=sin(rphi);
	dir[2]=r*sin(rtheta);
    
    Vec3d vup(0,1,0);
	right=dir.cross(vup);

    dir.normalize();
    right.normalize();

    up=right.cross(dir);
    up.normalize();
    
	dirty=false;

} 