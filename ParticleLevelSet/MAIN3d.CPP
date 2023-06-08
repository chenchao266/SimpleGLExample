#include "glad/gl.h"
//#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib,"glu32.lib")
//#pragma comment(lib,"glut32.lib")

#include "main.h"
#include "Particle.h"
#include "ParticleSet.h"
#include "container.h"
#include "MarchCubes.h"
#include "ImpSurface.h"
#include "Camera.h"
#include "ppm.h"
#include "Component/Timer.h"

bool pause = true;
bool bfill = true;
int particle_mode = 0;
bool OUTPUT_FILE = false;
bool writePOVRAYFile = false;
int count = 0;

Container contain(NX,NY,NZ,HH);

MarchCube* marchCube = 0;
IsoSurface* surface = 0;

Timer timer;
Camera cam(0,0,5);
int frame = 0;
int v_height,v_width;

void DisplayFunc()
{
	frame++;

	static Double time = 0.0;
	static int frames = 0;
	
	time += timer.time_s();
	timer.setStartTick();
	frames++;

	if(time >= 1.0)
	{
        std::cout<<"Framerate:"<<(frames/time)<< std::endl;
		frames = 0;
		time = 0;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(bfill) glPolygonMode(GL_FRONT,GL_FILL);
	else	  glPolygonMode(GL_FRONT,GL_LINE);
	
    cam.Draw();
	int verts[8][3];

	for(int i=0; i < 8;i++)
	{
		verts[i][0] = i & 1 ? 1 : -1;
		verts[i][1] = i & 2 ? 1 : -1;
		verts[i][2] = i & 4 ? 1 : -1;
	}

	glBegin(GL_LINES);
	for(int i=0; i < 8;i++)
	{
		for(int j=0;j<3;j++)
		{
			glVertex3iv(verts[i]);
			glVertex3iv(verts[i^(1<<j)]);
		}
	}
	glEnd();

	if(!pause) contain.Update();

	glEnable(GL_LIGHTING);
	

	marchCube->march(*surface);
	surface->glDraw();
	if(writePOVRAYFile)
	{
        std::ostringstream outs;
		outs << "povray/contain"<<frame<<".pov";

        std::ofstream out(outs.str().c_str());;
		out<<*surface<< std::endl;
		out.close();

		/*
		ostringstream cmds;
		cmds << "\"C:\\Program Files\\POV-Ray for Windows v3.5\\bin\\pvengine.exe\" -w1600 -h1200 +a0.3 -d +Ipovray/contain";
		cmds << frame;
		cmds << ".pov +f +opovray/contain";
		cmds << frame;
		cmds << ".bmp -p";
		system(cmds.str().c_str());
		*/
	}

	
	if(particle_mode)
	{
		glDisable(GL_LIGHTING);
		glBegin(GL_POINTS);
		
		ParticleSet::cIterator it = contain.pset.begin();
		while(it != contain.pset.end())
		{
			Vec3d pos;
			
			(*it)->GetPosition(pos);
			float x = pos[0] * 2 / NX - 1;
			float y = pos[1] * 2 / NY - 1;
			float z = pos[2] * 2 / NZ - 1;

			if((*it)->Sign() < 0)
			{
				glColor3f(1,0,0);
				if(particle_mode & 1)
					glVertex3d(x,y,z);
			}
			else
			{
				glColor3f(0,0,1);
				if(particle_mode & 2)
					glVertex3d(x,y,z);
			}

			
			it++;
		}
		glEnd();
	}

	glColor3f(1,1,1);
	//glutSwapBuffers();

	if(OUTPUT_FILE && count <= 1000)
	{
		static int frame = 0;
        std::ostringstream out;
		out << "movie/contain" << frame << ".ppm";
		frame++;
		FILE* fp = fopen(out.str().c_str(),"w");
		
		DumpPPM(fp,0,v_width,v_height);
		fclose(fp);
	}
}

void IdleFunc()
{
	//glutPostRedisplay();
}

void ReshapeFunc(int width, int height)
{
	if(height==0)
		height=1;

	v_height=height;
	v_width=width;

	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45,Float(width)/Float(height),0.01,1000);
	glMatrixMode(GL_MODELVIEW);
}

void KeyboardFunc(unsigned char key, int x, int y) 
{
 	switch(key) 
	{
	case 'a':
		cam.MoveLeft(0.2f);
		break;
	case 'd':
		cam.MoveRight(0.2f);
		break;
	case 'w':
		cam.MoveForward(0.2f);
		break;
	case 's':
		cam.MoveBackward(0.2f);
		break;
	case 'r':
		cam.MoveUp(0.2f);
		break;
	case 'f':
		cam.MoveDown(0.2f);
		break;
	case 'q':
		cam.RotateY(-3);
		break;
	case 'e':
		cam.RotateY(3);
		break;
	case 't':
		cam.RotateX(3);
		break;
	case 'g':
		cam.RotateX(-3);
		break;
	case ' ':
		pause = !pause;
		break;
	case 'p':
		particle_mode++;
		particle_mode %= 4;
		break;
	case 'z':
		bfill = !bfill;
		break;
	default:
		break;
	}

}

void MouseFunc(int button, int state, int x, int y)
{
	y=v_height-y;
		
	//if (state == 0)  //button down
	//{
	//	if (button == GLUT_LEFT_BUTTON) 
	//	{
	//	
	//	}
	//	else if (button == GLUT_MIDDLE_BUTTON) 
	//	{
	//	
	//	}
	//	else if (button == GLUT_RIGHT_BUTTON) 
	//	{

	//	}
	//}
	//else if (state == 1) //button up
	//{
	//	if (button == GLUT_LEFT_BUTTON) 
	//	{
	//		
	//	}
	//	else if(button==GLUT_MIDDLE_BUTTON)
	//	{

	//	}
	//	else if(button==GLUT_RIGHT_BUTTON)
	//	{
	//	
	//	}
	//}
}

int main3d(int argc,char **argv)
{
	//glutInit(&argc,argv);

	printf ( "\n\nSpacebar starts and stops the demo.\n\n" );

	//open_glut_window ();

	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // set display mode
	//glutInitWindowSize(600,600); // set window size
	//glutInitWindowPosition(0,0); // set window position on screen
	//glutCreateWindow("Glut Window"); // set window title

	//glutMouseFunc(MouseFunc); // register the mouse action function
	//glutKeyboardFunc(KeyboardFunc); // register the keyboard action function
	//glutDisplayFunc(DisplayFunc); //register the redraw function
	//glutReshapeFunc(ReshapeFunc);
	//glutIdleFunc(IdleFunc);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	cam.RotateY(180);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	marchCube = new MarchCube();
	marchCube->setThreshold(0);
	marchCube->setSize(2,2,2);
	marchCube->setRes(100,100,100);
	marchCube->setCenter(0,0,0);
	
	surface = new IsoSurface(&contain.lset);

	timer.setStartTick();
	//glutMainLoop();
		
	delete surface;
	delete marchCube;
	return 0;
}