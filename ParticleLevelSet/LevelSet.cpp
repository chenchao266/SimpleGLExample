﻿#include "LevelSet.h"
#include "ParticleSet.h"
#include "Particle.h"

void LevelSet::Update(const Velocity& grid, const Double &dt)
{
	//First Order time integration
    for (int k = 1; k <= NZ; k++) {
        for (int j = 1; j <= NY; j++) {
            for (int i = 1; i <= NX; i++) {//FOR_LS
                SemiLagrangianStep(i, j, k, grid, dt);
            }
        }
    }//END_FOR_THREE

        std::swap(gridPhi, gridTmp);
	gridPhi.SetBoundarySignedDist();
}

void LevelSet::SemiLagrangianStep(int x, int y, int z, const Velocity &grid, const Double &dt)
{
    static int r,s,t;
    static Double a,b,c;
	if(gridPhi(x,y,z) > SEMILAGRA_LIMIT) {
		gridTmp(x,y,z) = gridPhi(x,y,z);
		return;
	}
	static Vec3d u; //obtain from velocity grid
	grid.GetVelocity(Vec3d(x,y,z), u);

    r = x - int(ceil(u[0] * dt * hInv));
	s = y - int(ceil(u[1] * dt * hInv));
    t = z - int(ceil(u[2] * dt * hInv));

    //doesn't get from boundary
    r = Clamp(r,0,Nx);
    s = Clamp(s,0,Ny);
    t = Clamp(t,0,Nz);

	a = (Double(x - r) * h - u[0] * dt) * hInv;
	b = (Double(y - s) * h - u[1] * dt) * hInv;
    c = (Double(z - t) * h - u[2] * dt) * hInv;

	gridTmp(x,y,z) =    a  *    b  *    c  * gridPhi(r+1, s+1, t+1) +
				     (1-a) *    b  *    c  * gridPhi(r  , s+1, t+1) +
				        a  * (1-b) *    c  * gridPhi(r+1, s  , t+1) +
                        a  *    b  * (1-c) * gridPhi(r+1, s+1, t  ) +
                     (1-a) * (1-b) *    c  * gridPhi(r  , s  , t+1) +
                     (1-a) *    b  * (1-c) * gridPhi(r  , s+1, t  ) +
                        a  * (1-b) * (1-c) * gridPhi(r+1, s  , t  ) +
				     (1-a) * (1-b) * (1-c) * gridPhi(r  , s  , t  );
}

void LevelSet::ReInitialize(FastMarch &gridFM) {
    gridFM.Reinitialize(gridPhi);
    gridPhi.SetBoundarySignedDist();
}
    
void LevelSet::Fix(const ParticleSet& particleSet)
{	
	static ParticleSet::cIterator pit, end;
	static Vec3d pos;
    static Double phi;
    static int sign;

    end = particleSet.end();
	gridPos = gridPhi;
	gridNeg = gridPhi;
	for(pit = particleSet.begin(); pit != end; ++pit)
	{
		Particle& particle = *(*pit);
		particle.GetPosition(pos);
        phi = SAMPLEPHI(pos);
		sign = particle.Sign();

		if(phi * sign < 0.)
		{
			//particle has crossed the boundary
			if(sign < 0.) FixNeg(particle, int(pos[0]), int(pos[1]), int(pos[2]));
			else		  FixPos(particle, int(pos[0]), int(pos[1]), int(pos[2]));
		}
	}
	//Merge gridPos & gridNeg
	static Double phiPos, phiNeg;
    for (int k = 0; k < (NZ + 2); k++) {
        for (int j = 0; j < (NY + 2); j++) {
            for (int i = 0; i < (NX + 2); i++) {//FOR_ALL_LS
                phiPos = gridPos(i, j, k); phiNeg = gridNeg(i, j, k);
                gridPhi(i, j, k) = abs(phiPos) < abs(phiNeg) ? phiPos : phiNeg;
            }
        }
    }//END_FOR_THREE
}

inline void LevelSet::FixNeg(const Particle &particle, int i, int j, int k)
{
	static Double particlePhi;
	for(int dx = 0; dx < 2; dx++) {
		for(int dy = 0; dy < 2; dy++) {
            for(int dz = 0; dz < 2; dz++) {
			    particlePhi = particle.phi(Vec3d(i+dx,j+dy,k+dz), h);
                gridNeg(i+dx,j+dy,k+dz) = min(particlePhi, gridNeg(i+dx,j+dy,k+dz));
            }
		}
	}
}

inline void LevelSet::FixPos(const Particle &particle, int i, int j, int k)
{
	static Double particlePhi;
	for(int dx = 0; dx < 2; dx++) {
		for(int dy = 0; dy < 2; dy++) {
            for(int dz = 0; dz < 2; dz++) {
			    particlePhi = particle.phi(Vec3d(i+dx,j+dy,k+dz), h);
                gridPos(i+dx,j+dy,k+dz) = max(particlePhi, gridPos(i+dx,j+dy,k+dz));
            }
		}
    }
}

inline Double LevelSet::LinearSample(const Vec3d &pos) const
{
	static Double xlerp,ylerp,zlerp;
    static int i0, i1, j0, j1, k0, k1;

    i0 = int(pos[0]); i1 = i0 + 1;
    j0 = int(pos[1]); j1 = j0 + 1;
    k0 = int(pos[2]); k1 = k0 + 1;
    xlerp = pos[0]-i0;
	ylerp = pos[1]-j0;
    zlerp = pos[2]-k0;

    return Lerp(zlerp,
                Lerp(ylerp,
                     Lerp( xlerp, gridPhi(i0, j0, k0), gridPhi(i1, j0, k0) ),
			         Lerp( xlerp, gridPhi(i0, j1, k0), gridPhi(i1, j1, k0) )),
                Lerp(ylerp,
                     Lerp( xlerp, gridPhi(i0, j0, k1), gridPhi(i1, j0, k1) ),
                     Lerp( xlerp, gridPhi(i0, j1, k1), gridPhi(i1, j1, k1) )) );
}

Double LevelSet::CubicSample(const Vec3d &pos) const
{
    static Double r, s, t;
    static int i0,i1,i2,i3,j0,j1,j2,j3,k0,k1,k2,k3;
    
    i0 = int(pos[0]) - 1; i1 = i0+1; i2 = i1+1; i3 = i2+1;
    j0 = int(pos[1]) - 1; j1 = j0+1; j2 = j1+1; j3 = j2+1;
    k0 = int(pos[2]) - 1; k1 = k0+1; k2 = k1+1; k3 = k2+1;
    r = pos[0] - i1;
    s = pos[1] - j1;
    t = pos[2] - k1;
    i0 = i0 < 0 ? 0 : i0; i3 = i3 > Nx+1 ? Nx+1 : i3;
    j0 = j0 < 0 ? 0 : j0; j3 = j3 > Ny+1 ? Ny+1 : j3;
    k0 = k0 < 0 ? 0 : k0; k3 = k3 > Nz+1 ? Nz+1 : k3;

	return  MCerp(t, MCerp(r, MCerp(s, gridPhi(i0,j0,k0),gridPhi(i0,j1,k0),
                              gridPhi(i0,j2,k0),gridPhi(i0,j3,k0)),
                              MCerp(s, gridPhi(i1,j0,k0),gridPhi(i1,j1,k0),
                              gridPhi(i1,j2,k0),gridPhi(i1,j3,k0)),
                              MCerp(s, gridPhi(i2,j0,k0),gridPhi(i2,j1,k0),
                              gridPhi(i2,j2,k0),gridPhi(i2,j3,k0)),
                              MCerp(s, gridPhi(i3,j0,k0),gridPhi(i3,j1,k0),
                              gridPhi(i3,j2,k0),gridPhi(i3,j3,k0)) ),
                     MCerp(r, MCerp(s, gridPhi(i0,j0,k1),gridPhi(i0,j1,k1),
                              gridPhi(i0,j2,k1),gridPhi(i0,j3,k1)),
                              MCerp(s, gridPhi(i1,j0,k1),gridPhi(i1,j1,k1),
                              gridPhi(i1,j2,k1),gridPhi(i1,j3,k1)),
                              MCerp(s, gridPhi(i2,j0,k1),gridPhi(i2,j1,k1),
                              gridPhi(i2,j2,k1),gridPhi(i2,j3,k1)),
                              MCerp(s, gridPhi(i3,j0,k1),gridPhi(i3,j1,k1),
                              gridPhi(i3,j2,k1),gridPhi(i3,j3,k1)) ),
                     MCerp(r, MCerp(s, gridPhi(i0,j0,k2),gridPhi(i0,j1,k2),
                              gridPhi(i0,j2,k2),gridPhi(i0,j3,k2)),
                              MCerp(s, gridPhi(i1,j0,k2),gridPhi(i1,j1,k2),
                              gridPhi(i1,j2,k2),gridPhi(i1,j3,k2)),
                              MCerp(s, gridPhi(i2,j0,k2),gridPhi(i2,j1,k2),
                              gridPhi(i2,j2,k2),gridPhi(i2,j3,k2)),
                              MCerp(s, gridPhi(i3,j0,k2),gridPhi(i3,j1,k2),
                              gridPhi(i3,j2,k2),gridPhi(i3,j3,k2)) ),
                     MCerp(r, MCerp(s, gridPhi(i0,j0,k3),gridPhi(i0,j1,k3),
                              gridPhi(i0,j2,k3),gridPhi(i0,j3,k3)),
                              MCerp(s, gridPhi(i1,j0,k3),gridPhi(i1,j1,k3),
                              gridPhi(i1,j2,k3),gridPhi(i1,j3,k3)),
                              MCerp(s, gridPhi(i2,j0,k3),gridPhi(i2,j1,k3),
                              gridPhi(i2,j2,k3),gridPhi(i2,j3,k3)),
                              MCerp(s, gridPhi(i3,j0,k3),gridPhi(i3,j1,k3),
                              gridPhi(i3,j2,k3),gridPhi(i3,j3,k3)) ) );
}

void LevelSet::normal(const Vec3d &pos, Vec3d &n) const
{
	gradient(pos, n);
	n.normalize();
}

void LevelSet::gradient(const Vec3d &pos, Vec3d &g) const
{
	static Vec3d dx = Vec3d(0.25 , 0.  , 0.  );
	static Vec3d dy = Vec3d(0.   , 0.25, 0.  );
    static Vec3d dz = Vec3d(0.   , 0.  , 0.25);
	g[0] = SAMPLEPHI(pos+dx) - SAMPLEPHI(pos-dx);
	g[1] = SAMPLEPHI(pos+dy) - SAMPLEPHI(pos-dy);
    g[2] = SAMPLEPHI(pos+dz) - SAMPLEPHI(pos-dz);
	g *= 2. * hInv;
}

void LevelSet::gradient(const Vec3d &pos, const Vec3d &u, Vec3d &g)
{
	static Vec3d dx = Vec3d(0.25 , 0.  , 0.  );
	static Vec3d dy = Vec3d(0.   , 0.25, 0.  );
    static Vec3d dz = Vec3d(0.   , 0.  , 0.25);
    static Double center;
	center = SAMPLEPHI(pos);
	if(u[0] < 0)	g[0] = (SAMPLEPHI(pos + dx) - center);
	else			g[0] = (center - SAMPLEPHI(pos - dx));
	if(u[1] < 0)	g[1] = (SAMPLEPHI(pos + dy) - center);
	else			g[1] = (center - SAMPLEPHI(pos - dy));
    if(u[2] < 0)	g[2] = (SAMPLEPHI(pos + dz) - center);
	else			g[2] = (center - SAMPLEPHI(pos - dz));
    g *= 4 * hInv;
}