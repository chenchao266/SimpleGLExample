#pragma once

#include "CommonInterfaces/CommonGraphicsAppInterface.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "CommonInterfaces/CommonRenderInterface.h"
#include "CommonInterfaces/CommonExampleInterface.h"
#include "CommonInterfaces/CommonGUIHelperInterface.h"

#include "LevelSet3D.h"
#include "ParticleSet3D.h"
#include "StableFluid3D.h"
#include "MarchingCubes.h"
#include "Component/Timer.h"
//#include "PerspectiveCamera.h"

class WaterSimulator3D : public CommonExampleInterface
{
    CommonGraphicsApp* m_app;
      GUIHelperInterface* m_guiHelper;
	bool bfill;
	bool pause;
	bool step;
	int gridSizeX;
	int gridSizeY;
	int gridSizeZ;
	int particle_mode;
	
	bool debugMarchingCubes;
	bool render_pressure;
	bool render_surface;
	bool OUTPUT_FILE;
	bool writePOVRAYFile;

	const double TIMESTEP = 0.1;
     
	//PerspectiveCamera cam;
	int frame = 0;
	int v_height, v_width;


	LevelSet3D lset;
	ParticleSet3D pset;
	StableFluid3D grid0, grid1, grid2;
	MarchingCubes marchingCubes;
    float gridSizeXPara;
    float gridSizeYPara;
    float gridSizeZPara;
	friend double implicit(const Vec3d& o, WaterSimulator3D*);
public:
	WaterSimulator3D(GUIHelperInterface* app);
	~WaterSimulator3D();
    virtual void physicsDebugDraw(int debugDrawMode)
    {
    }
    virtual void initPhysics();

    virtual void exitPhysics()
    {
    }
    virtual void stepSimulation(float deltaTime);
 
    virtual void renderScene()
    {
        
    }
	// GLUT callbacks
 

    virtual bool keyboardCallback(int key, int state);

    virtual bool mouseButtonCallback(int button, int state, float x, float y);
   
    virtual bool mouseMoveCallback(float x, float y);
};

