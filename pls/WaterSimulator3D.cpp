#include "mathfun.h"
#include "WaterSimulator3D.h"

 
#include "LevelSet3D.h"
#include "Particle3D.h"
#include "ParticleSet3D.h"
#include "StableFluid3D.h"
#include "MarchingCubes.h"
//#include "PerspectiveCamera.h"
#include "Component/Timer.h"
#include "OpenGLWindow/OpenGLInclude.h"
#include "CommonInterfaces/CommonCallbacks.h"
#include "CommonInterfaces/CommonParameterInterface.h"
#include "Component/b3BulletDefaultFileIO.h"

WaterSimulator3D::WaterSimulator3D(GUIHelperInterface* gui)//int gridSizeX, int gridSizeY, int gridSizeZ
	:  gridSizeX(100), gridSizeY(100), gridSizeZ(100),
    lset(gridSizeX, gridSizeY, gridSizeZ),
	pset(gridSizeX, gridSizeY, gridSizeZ), 
	grid0(gridSizeX, gridSizeY, gridSizeZ, gridSizeX, gridSizeY, gridSizeZ),
	grid1(gridSizeX, gridSizeY, gridSizeZ, gridSizeX, gridSizeY, gridSizeZ),
	grid2(gridSizeX, gridSizeY, gridSizeZ, gridSizeX, gridSizeY, gridSizeZ),
	//cam(0,0,5),
	frame(0),
	particle_mode(0),
	bfill(true),
	pause(true),
	step(false),
	debugMarchingCubes(false),
	render_pressure(true),
	render_surface(true),
	OUTPUT_FILE(false),
	writePOVRAYFile(false)
{
    m_guiHelper = gui;
    m_app = gui->getAppInterface();
}


WaterSimulator3D::~WaterSimulator3D()
{
}


void WaterSimulator3D::stepSimulation(float deltaTime)
{
	frame++;

	static double time = 0.0;
	static int frames = 0;

#ifdef OSGOCEAN_TIMING
    Timer_t startTime;
    Timer_t endTime;
    startTime = Timer::instance()->tick();
#endif /*OSTOCEAN_TIMING*/
	frames++;

	if (time >= 1.0)
	{
        std::cout << "Framerate:" << (frames / time) << std::endl;
		frames = 0;
		time = 0;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (bfill)
	{
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	else
	{
		glPolygonMode(GL_FRONT, GL_LINE);
	}

	//cam.Draw();
	int verts[8][3];

	for (int i = 0; i < 8; i++)
	{
		verts[i][0] = i & 1 ? 1 : -1;
		verts[i][1] = i & 2 ? 1 : -1;
		verts[i][2] = i & 4 ? 1 : -1;
	}

	glBegin(GL_LINES);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j<3; j++)
		{
			glVertex3iv(verts[i]);
			glVertex3iv(verts[i ^ (1 << j)]);
		}
	}
	glEnd();

	if (!pause || step)
	{
		
		//lset.Update(grid0, grid1, grid2, TIMESTEP);
		lset.UpdateThreaded(grid0, grid1, grid2, TIMESTEP);
		//pset.Update(grid0, grid1, grid2, TIMESTEP);
		pset.UpdateThreaded(grid0, grid1, grid2, TIMESTEP);
		
		//		lset.Fix(pset);
		lset.Reinitialize();
		//	lset.Reinitialize();
		//		lset.CheckGrid();

		static int count = 0;
		count++;
		
		grid0 = grid2;
		grid1.step(TIMESTEP);
		grid2.step(TIMESTEP);
		
		if (count % 20 == 0)
		{
			//			pset.Reseed(lset);
		}
		if (count % 10 == 0)
		{
			//	lset.Reinitialize();
		}
		step = false;
	}

    if (render_surface)
    { 
        marchingCubes.Update(); 
    }

#ifdef OSGOCEAN_TIMING
    endTime = Timer::instance()->tick();
    double dt = Timer::instance()->delta_m(startTime, endTime);
    time += dt;
    fprintf(stderr, "updateVertices() time = %lfms\n", dt);
#endif /*OSGOCEAN_TIMING*/

}
 
void WaterSimulator3D::renderScene()
{
    glEnable(GL_LIGHTING);
    if (render_surface)
    { 
        marchingCubes.Draw();
    }

    if (writePOVRAYFile)
    {
        std::ostringstream outs;
        outs << "povray/water" << frame << ".pov";

        std::ofstream out(outs.str().c_str());;
        //	out << *surface << endl;
        out.close();

        //std::ostringstream cmds;
        //cmds << "\"C:\\Users\\alanga\\AppData\\Roaming\\POV-Ray\\v3.6\\bin\\pvengine64.exe\" /exit -w1600 -h1200 +a0.3 -d +Ipovray/water";
        //cmds << frame;
        //cmds << ".pov +f +opovray/water";
        //cmds << frame;
        //cmds << ".bmp -p";
        //system(cmds.str().c_str());
    }

    if (particle_mode)
    {
        glDisable(GL_LIGHTING);
        glBegin(GL_POINTS);
        for (int i = 0; i < gridSizeX; i++)
        {
            for (int j = 0; j < gridSizeY; j++)
            {
                for (int k = 0; k < gridSizeZ; k++)
                {
                    ParticleSet3D::Iterator it = pset.begin(i, j, k);
                    while (it != pset.end(i, j, k))
                    {
                        double x, y, z;

                        (*it)->GetPosition(x, y, z);
                        x = x * 2 / gridSizeX - 1;
                        y = y * 2 / gridSizeY - 1;
                        z = z * 2 / gridSizeZ - 1;

                        if ((*it)->Sign() < 0)
                        {
                            glColor3f(1, 0, 0);
                            if (particle_mode & 1)
                                glVertex3d(x, y, z);
                        }
                        else
                        {
                            glColor3f(0, 0, 1);
                            if (particle_mode & 2)
                                glVertex3d(x, y, z);
                        }


                        it++;
                    }
                }
            }
        }
        glEnd();
    }
    if (render_pressure)
    {
        glDisable(GL_LIGHTING);
        glBegin(GL_POINTS);
        for (int i = 0; i < gridSizeX; i++)
        {
            for (int j = 0; j < gridSizeY; j++)
            {
                for (int k = 0; k < gridSizeZ; k++)
                {
                    double x = i, y = j, z = k;
                    x = x * 2 / gridSizeX - 1;
                    y = y * 2 / gridSizeY - 1;
                    z = z * 2 / gridSizeZ - 1;

                    double pressureVal = grid0.pressure[grid0.makeIndex(i, j, k)];
                    //cout << i << " " << j << " " << k << " " << pressureVal << endl;
                    double r = pressureVal / 20;
                    double g = 0;
                    double b = (20 - pressureVal) / 20;
                    glColor3f(r, g, b);
                    glVertex3d(x, y, z);
                }
            }

        }
        glEnd();
    }

    glColor3f(1, 1, 1);
    //glutSwapBuffers();
  
    //if (OUTPUT_FILE)
    //{
    //    static int frame = 0;
    //    ostringstream out;
    //    out << "movie/water" << frame << ".ppm";
    //    frame++;
    //    FILE* fp;
    //    fopen_s(&fp, out.str().c_str(), "w");
    //    DumpPPM(fp, 0, v_width, v_height);
    //    fclose(fp);
    //}
   
}

bool WaterSimulator3D::keyboardCallback(int key, int state)
{
	switch (key)
	{
	case 'n':
		step = true;
		break;
	case ' ':
		pause = !pause;
		break;
	case 'p':
		particle_mode++;
		particle_mode %= 4;
		break;
	case 'o':
		marchingCubes.level++;
        std::cout << "level is " << marchingCubes.level << std::endl;
		break;
	case 'i':
		marchingCubes.level--;
        std::cout << "level is " << marchingCubes.level << std::endl;
		break;
	case 'z':
		bfill = !bfill;
		break;

	default:
		break;
	}
    return true;
}


bool WaterSimulator3D::mouseButtonCallback(int button, int state, float x, float y)
{
	y = v_height - y;

	if (state == 0)  //button down
	{
		if (button == MOUSE_LEFT)
		{

		}
		else if (button == MOUSE_MIDDLE)
		{

		}
		else if (button == MOUSE_RIGHT)
		{

		}
	}
	else if (state == 1) //button up
	{
		if (button == MOUSE_LEFT)
		{

		}
		else if (button == MOUSE_MIDDLE)
		{

		}
		else if (button == MOUSE_RIGHT)
		{

		}
	}
    return true;
}
 
bool WaterSimulator3D::mouseMoveCallback(float x, float y)
{
    return false;
}

double implicit(const Vec3d& o, WaterSimulator3D* simulator)
{
	double x = (o[0] + 1) * simulator->gridSizeX / 2.0;
	double y = (o[1] + 1) * simulator->gridSizeY / 2.0;
	double z = (o[2] + 1) * simulator->gridSizeZ / 2.0;
	double d = simulator->lset.LinearSample(x, y, z);

	double scale = double(simulator->gridSizeX) / 2.0;
	return d / scale;
}

void WaterSimulator3D::initPhysics( )
{
    {
        SliderParams slider("GridSizeX", &gridSizeXPara);
        slider.m_minVal = 10;
        slider.m_maxVal = 1000;
        if (m_guiHelper->getParameterInterface())
            m_guiHelper->getParameterInterface()->registerSliderFloatParameter(slider);
    }
    {
        SliderParams slider("GridSizeY", &gridSizeYPara);
        slider.m_minVal = 10;
        slider.m_maxVal = 1000;
        if (m_guiHelper->getParameterInterface())
            m_guiHelper->getParameterInterface()->registerSliderFloatParameter(slider);
    }
    {
        SliderParams slider("GridSizeZ", &gridSizeZPara);
        slider.m_minVal = 10;
        slider.m_maxVal = 1000;
        if (m_guiHelper->getParameterInterface())
            m_guiHelper->getParameterInterface()->registerSliderFloatParameter(slider);
    }


	lset.MakeSphere(.5*gridSizeX, .9*gridSizeY, .5*gridSizeZ, .1*gridSizeX);

	//lset.MakeSphere(.65*grid_sizex,.65*grid_sizey,.65*grid_sizez,.15*grid_sizex );
	lset.Reinitialize();
	//lset.CheckGrid();
	for (int i = 0; i<gridSizeX; i++)
	{
		for (int j = 0; j<gridSizeY; j++)
		{
			for (int k = 0; k<gridSizeZ; k++)
			{
				Vec3d center(.45*gridSizeX, .45*gridSizeY, .45*gridSizeZ);
				Vec3d here(i, j, k);

				if ((center - here).norm() < .45*gridSizeX)
				{
					int index = grid0.makeIndex(i, j, k);
					grid0.pressure[index] = 1.0;
					grid1.pressure[index] = 1.0;
					grid2.pressure[index] = 1.0;
				}
			}
		}
	}

	
	grid1.step(TIMESTEP * 0.5);
	grid2.step(TIMESTEP);
	
	//surface = new IsoSurface(&lset);
    implicitFuncType fa = std::bind(&implicit, std::placeholders::_1, this);
	marchingCubes.setImplicitFunction(fa);
	marchingCubes.createTable();

	pset.Reseed(lset);
	 
	//glutMainLoop();
}
