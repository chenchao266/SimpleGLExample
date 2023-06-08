#pragma once
#include "mathfun.h"
#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/geometry.h"
#include "Bullet3Common/SparseVector.h"
#include "Bullet3Common/SparseMatrix.h"
#include <vector>
static int xOffset2[2] = { 1, 0 };
static int yOffset2[2] = { 0, 1 };


class StableFluid2D
{
	const double GRAV_ACCEL = 9.8;
	const double PCG_EPS = 1e-10;
	const double PCG_MAXITER = 500;
public:
	StableFluid2D();
	~StableFluid2D();
	StableFluid2D(double width_, double height_, int resX_, int resY_);
	StableFluid2D(const StableFluid2D& toCopy);

	StableFluid2D& operator =(const StableFluid2D& toCopy);

	void step(double dt);

	void getvalue(double i, double j, double& ux, double& uy) const;
	Vec2d linearSamp(const Vec2d& pos, double**const v[3]) const;
	Vec2d linearSamp(double x, double y, double**const v[3]) const;

	double linearSamp(const Vec2d& pos, VecXd& v) const;
	double linearSamp(double x, double y, VecXd& v) const;

	void setVisc(double viscosity_);
	void setDensity(double density_);
	void setSize(int width_, int height_);
	void setRes(int resX_, int resY_);

protected:
	void initGrids();
	void clearGrids();
	void initPoisson();
	void clearPoisson();
public:
	int makeIndex(int x, int y) const;
	void splitIndex(int index, int& x, int& y);
	void setBoundary();
	void addForcesToVelocity(double dt);
	void advectVelocity(double dt);
	void projectVelocity(double dt);

	SparseMatrixd laplacian;
	SparseMatrixd preconditioner;

    VecXd vel;
    VecXd	pressure, tempPressure;

	std::vector<VecXd>	r;
	std::vector<VecXd>	z;
    VecXd	p;

    VecXd	ones;

    double** velocities[3];
	double** tempVelocities[3];

	double viscosity;
	double density;
	double width;
	double height;
	int	resX;
	int resY;
};
