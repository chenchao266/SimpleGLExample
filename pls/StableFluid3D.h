#pragma once
#include "mathfun.h"
#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/geometry.h"
#include "Bullet3Common/SparseVector.h"

#include "Bullet3Common/SparseMatrix.h"

static int xOffset3[3] = { 1, 0, 0 };
static int yOffset3[3] = { 0, 1, 0 };
static int zOffset3[3] = { 0, 0, 1 };

class StableFluid3D
{
	const double GRAV_ACCEL = 9.8;
	const double PCG_EPS = 1e-10;
	const double PCG_MAXITER = 500;
public:
	StableFluid3D();
	~StableFluid3D();
	StableFluid3D(double width_, double height_, double depth_, int resX_, int resY_, int resZ_);
	StableFluid3D(const StableFluid3D& toCopy);

	StableFluid3D& operator =(const StableFluid3D& toCopy);

	void step(double dt);

	void getvalue(double i, double j, double k, double& ux, double& uy, double& uz) const;
	Vec3d linearSamp(const Vec3d& pos, double***const v[3]) const;
	Vec3d linearSamp(double x, double y, double z, double***const v[3]) const;

	double linearSamp(const Vec3d& pos, VecXd& v) const;
	double linearSamp(double x, double y, double z, VecXd& v) const;

	void setVisc(double viscosity_);
	void setDensity(double density_);
	void setSize(int width_, int height_, int depth_);
	void setRes(int resX_, int resY_, int resZ_);

protected:
	void initGrids();
	void clearGrids();
	void initPoisson();
	void clearPoisson();
public:
	int makeIndex(int x, int y, int z) const;
	void splitIndex(int index, int& x, int& y, int& z);
	void setBoundary();
	void addForcesToVelocity(double dt);
	void advectVelocity(double dt);
	void advectPressure(double dt);
	void projectVelocity(double dt);

	SparseMatrixd laplacian;
	SparseMatrixd preconditioner;

    VecXd vel;
    VecXd	pressure, tempPressure;

	std::vector<VecXd>	r;
	std::vector<VecXd>	z;
    VecXd	p;

    VecXd	ones;

	double***  velocities[3];
	double***  tempVelocities[3];

	double viscosity;
	double density;
	double width;
	double height;
	double depth;
	int	resX;
	int resY;
	int resZ;
};
