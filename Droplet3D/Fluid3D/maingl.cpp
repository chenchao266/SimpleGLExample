#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <glad/gl.h>
//#include "gluvi.h"
   
#include "CommonInterfaces/CommonGraphicsAppInterface.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "CommonInterfaces/CommonRenderInterface.h"
#include "CommonInterfaces/CommonExampleInterface.h"
#include "CommonInterfaces/CommonGUIHelperInterface.h"
#include "../UnifiedPhysics/render_particles.h"
//Simple viewer for liquid simulator data
//Hold shift and use the mouse buttons to manipulate the camera

//using namespace std;
class Fluid3D : public CommonExampleInterface
{
    CommonGraphicsApp* m_app;
    GUIHelperInterface* m_guiHelper;
    std::string frame_number = "frame 0";

    std::string file_path;

    unsigned int frame = 0;
    unsigned int highest_frame = 0;

    bool filming = false;
    bool running = false;

    char * ppmfileformat;

    std::vector<Vec3f> particles;
    float particle_radius;
    ParticleRenderer::DisplayMode displayMode = ParticleRenderer::PARTICLE_SPHERES;
    ParticleRenderer *renderer = 0;
    GLuint g_vbo = 0;

public:
    Fluid3D(GUIHelperInterface* app);
    ~Fluid3D();
    virtual void physicsDebugDraw(int debugDrawMode)
    {
    }
    virtual void initPhysics();

    virtual void exitPhysics()
    {
    }
    virtual void stepSimulation(float deltaTime);

    virtual void renderScene();
    // GLUT callbacks


    virtual bool keyboardCallback(int key, int state);

    virtual bool mouseButtonCallback(int button, int state, float x, float y);

    virtual bool mouseMoveCallback(float x, float y);
    void createVBO(GLuint & vbo, std::vector<Vec3f> & particle_data);
    bool read_frame(int newframe);
    void set_lights_and_material(int  );

};
void Fluid3D::createVBO(GLuint & vbo, std::vector<Vec3f> & particle_data)
{
	if(vbo)
		glDeleteBuffers(1, &vbo);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	float *data = new float[4*particle_data.size()];
	for (int i=0;i<particle_data.size();i++)
	{
		data[i*4]   =particle_data[i][0];
		data[i*4+1] =particle_data[i][1];
		data[i*4+2] =particle_data[i][2];
		data[i*4+3] =1.0;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*particle_data.size(), 
		data, GL_STATIC_DRAW);
	delete []data;


}
bool Fluid3D::read_frame(int newframe)
{
   if(newframe<0) newframe = highest_frame;

   std::ostringstream strout;
   
   strout << file_path << "particles_" << newframe << ".txt";
   printf("File path %s\n", strout.str().c_str());
   std::ifstream particles_in(strout.str().c_str());
   if(!particles_in.good()) {
      printf("Failed to open particles!\n");
      return false;
   }
   unsigned int p_count;
   particles_in >> p_count >> particle_radius;
   particles.resize(p_count);
   for(unsigned int p = 0; p < p_count; ++p) {
      Vec3f pos;
      particles_in >> pos[0] >> pos[1] >> pos[2];
      particles[p] = pos;
   }
   printf("Particle count: %d\n", p_count);
   printf("Particle radius: %f\n", particle_radius);

   if(newframe > (int)highest_frame)
      highest_frame = newframe;

   strout.str("");

   frame=newframe;

   strout.str("");
   strout << "frame " << frame;
   frame_number = strout.str();

   return true;
}
 
void Fluid3D::set_lights_and_material(int object)
{
   glEnable(GL_LIGHTING);
   GLfloat global_ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
   glShadeModel(GL_SMOOTH);

   //Light #1
   GLfloat color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
   GLfloat position[3] = {1.0f, 1.0f, 1.0f};
   glLightfv(GL_LIGHT0, GL_SPECULAR, color);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
   glLightfv(GL_LIGHT0, GL_POSITION, position);

   //Light #2
   GLfloat color2[4] = {1.0f, 1.0f, 1.0f, 1.0f};
   GLfloat position2[3] = {-1.0f, -1.0f, 1.0f};
   glLightfv(GL_LIGHT1, GL_SPECULAR, color2);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, color2);
   glLightfv(GL_LIGHT1, GL_POSITION, position2);

   GLfloat obj_color[4] = {.2, .3, .7};
   glMaterialfv (GL_FRONT, GL_AMBIENT, obj_color);
   glMaterialfv (GL_FRONT, GL_DIFFUSE, obj_color);

   GLfloat specular[4] = {.4, .2, .8};
   glMaterialf (GL_FRONT, GL_SHININESS, 32);
   glMaterialfv (GL_FRONT, GL_SPECULAR, specular);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);

}

void Fluid3D::stepSimulation(float deltaTime)
{
   if(filming) {
      //Gluvi::ppm_screenshot(ppmfileformat, frame);
      if(read_frame(frame+1)) {
         if(frame == 0) {
            filming = false;
         }
      }
      //glutPostRedisplay();
      //glutTimerFunc(200, timer, 0);
   }


   if(running) {
      read_frame(frame+1);
      //glutTimerFunc(1, timer, 0);
      //glutPostRedisplay();
   }

}


void Fluid3D::renderScene( )
{
   glClearColor(0.6f, 0.7f, 0.9f, 1);

   //Coordinate system
   glDisable(GL_LIGHTING);
   glBegin(GL_LINES);
   glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(0.1,0,0);
   glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,0.1,0);
   glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,0.1);
   glEnd();

   //glEnable(GL_LIGHTING);
   //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   //set_lights_and_material(1); 
   //glDisable(GL_DEPTH_TEST);
   //glEnable(GL_BLEND);
   //glBlendFunc(GL_ONE, GL_ONE);
   //Draw the liquid particles as simple spheres for now.
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
   //GLUquadric* particle_sphere;
   //particle_sphere = gluNewQuadric();
   //gluQuadricDrawStyle(particle_sphere, GLU_FILL );
   //for(unsigned int p = 0; p < particles.size(); ++p) {
   //   glPushMatrix();
   //   Vec3f pos = particles[p].v;
   //   glTranslatef(pos[0], pos[1], pos[2]);
   //   gluSphere(particle_sphere, particle_radius, 20, 20);
   //   glPopMatrix();   
   //}
   renderer->setWindowSize(720, 720);
   renderer->setFOV(45.0);
   renderer->setParticleRadius(particle_radius);
   createVBO(g_vbo, particles);
   renderer->setVertexBuffer(g_vbo, particles.size());
   renderer->display(ParticleRenderer::PARTICLE_SPHERES);

   //glBegin(GL_POINTS);
   //for(unsigned int p = 0; p < particles.size(); ++p)
   //{
	  // glColor4f(0.1,0.1,0.8,0.01);
	  // glVertex3f(particles[p][0],particles[p][1],particles[p][2]);
   //}
   //glEnd();
 
   //glEnable(GL_DEPTH_TEST);
   //Draw the bound box for good measure
   //glDisable(GL_LIGHTING);
   //glColor3f(0,0,0);
   //glBegin(GL_LINES);
   //glVertex3f(0,0,0);
   //glVertex3f(0,0,1);

   //glVertex3f(0,0,0);
   //glVertex3f(0,0.5,0);

   //glVertex3f(0,0,0);
   //glVertex3f(1,0,0);

   //glVertex3f(0,0.5,0);
   //glVertex3f(1,0.5,0);

   //glVertex3f(1,0.5,0);
   //glVertex3f(1,0,0);

   //glVertex3f(1,0,0);
   //glVertex3f(1,0,0.5);

   //glVertex3f(0,0.5,0);
   //glVertex3f(0,0.5,0.5);

   //glVertex3f(1,0.5,0);
   //glVertex3f(1,0.5,0.5);

   //glVertex3f(0,0.5,0.5);
   //glVertex3f(1,0.5,0.5);

   //glVertex3f(1,0,0.5);
   //glVertex3f(1,0.5,0.5);

   //glVertex3f(0,0,0.5);
   //glVertex3f(1,0,0.5);

   //glVertex3f(0,0,0.5);
   //glVertex3f(0,0.5,0.5);

   //glEnd();
   
   //Draw wireframe sphere geometry (specific to this scene).
   //glColor3f(0,0,0);
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
   //GLUquadric* sphere;
   //sphere = gluNewQuadric();
   //gluQuadricDrawStyle(sphere, GLU_LINE );
   //glPushMatrix();
   //glTranslatef(0.5f, 0.5f,0.5f);
   //gluSphere(sphere, 0.35, 20, 20);
   //glPopMatrix();


} 

bool Fluid3D::keyboardCallback(int key, int state) {

   if(key == 'r') {
      if(!filming) {
         if(!running) {
            running = true;
            //glutTimerFunc(1000, timer, 0);
         }
         else {
            running = false;
         }
      }
   }
   else if(key == 'f') {
      if(!running) {
         if(!filming) {
            filming = true;
         }
         else {
            filming = false;
         }
      }
   }
   //glutPostRedisplay();
   return true;
}

Fluid3D::Fluid3D(GUIHelperInterface* gui)
{
    m_guiHelper = gui;
    m_app = gui->getAppInterface();



    //Gluvi::init("Liquid Data Viewer", &argc, argv);
    renderer = new ParticleRenderer;
    renderer->setParticleRadius(0.01);

    file_path = "";

    //read grid dimensions
    char buffer[100];
    sprintf(buffer, "%s/simdata.txt", file_path.c_str());

    std::ostringstream strout;
    strout << file_path << "liquidmesh_" << 0 << ".obj";

    ppmfileformat = new char[strlen(file_path.c_str()) + 100];
    sprintf(ppmfileformat, "%sscreenshot%%04d.ppm", file_path.c_str());
    printf("%s\n", ppmfileformat);
}
   //read input mesh here


Fluid3D::~Fluid3D()
{
 
}

void Fluid3D::initPhysics()
{
    if (!read_frame(0))
        return;
}


 