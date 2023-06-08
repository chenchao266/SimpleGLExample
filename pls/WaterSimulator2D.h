#pragma once
#include "CommonInterfaces/CommonGraphicsAppInterface.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "CommonInterfaces/CommonRenderInterface.h"
#include "CommonInterfaces/CommonExampleInterface.h"
#include "CommonInterfaces/CommonGUIHelperInterface.h"

#include "LevelSet2D.h"
#include "ParticleSet2D.h"
#include "StableFluid2D.h"
#include "MarchingSquares.h"
 
class WaterSimulator2D : public CommonExampleInterface
{
    CommonGraphicsApp* m_app;
      GUIHelperInterface* m_guiHelper;
	bool bfill;
	bool pause;
	bool step;
	int gridSizeX;
	int gridSizeY;
	int particle_mode;

	bool debugMarchingCubes;
	bool render_pressure;
	bool render_surface;
	bool OUTPUT_FILE;
	bool writePOVRAYFile;

	const double TIMESTEP = 0.1;
     

	int frame = 0;
	int v_height, v_width;


	LevelSet2D lset;
	ParticleSet2D pset;
	StableFluid2D grid0, grid1, grid2;
	MarchingSquares marchingSquares;
    float gridSizeXPara;
    float gridSizeYPara;
	friend double implicit(const Vec2d& o, WaterSimulator2D*);
public:
	WaterSimulator2D(GUIHelperInterface* app);
	~WaterSimulator2D();

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

