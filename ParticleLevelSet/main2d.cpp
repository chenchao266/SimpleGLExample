#include "glad/gl.h"
#include "CommonInterfaces/CommonGraphicsAppInterface.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "CommonInterfaces/CommonRenderInterface.h"
#include "CommonInterfaces/CommonExampleInterface.h"
#include "CommonInterfaces/CommonGUIHelperInterface.h"

#include "main.h"
#include "container2d.h"
#include "Component/timer.h"
#include "particle2d.h"
#include "particleset2d.h"
#include "Bullet3Common/geometry.h"

/* global variables */
class ParticleLevelSet2D : public CommonExampleInterface
{
    CommonGraphicsApp* m_app;
    GUIHelperInterface* m_guiHelper;
    Container2D container;
     
    int dvel = 3, pause = 1, area = 0, rate = 0;
       
    Double dt = 0;

    Double rot_time = 0.0;
    Double fps_time = 0.0;
    int frames = 0;
    Timer timer;

public:
    ParticleLevelSet2D(GUIHelperInterface* app);
    ~ParticleLevelSet2D();

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
protected:
    void draw_velocity();
    void draw_density();
    void draw_points();
    void FindArea();
    void draw_particles();
};

 
void ParticleLevelSet2D::draw_velocity ( void )
{
	Float x, y, u, v;

	glColor3f ( 1.0f, 1.0f, 1.0f );
	glLineWidth ( 1.0f );

	glBegin ( GL_LINES );

		for (int i=1 ; i<=NX ; i++ ) {
			x = (i-0.5f)*HH;
			for (int j=1 ; j<=NY ; j++ ) {
				y = (j-0.5f)*HH;
				container.GetVelocity(i,j,u,v);
				glVertex2f ( x, y );
				glVertex2f ( x+u, y+v );
			}
		}

	glEnd ();
}

void ParticleLevelSet2D::draw_density ( void )
{
	Float x, y, d00, d01, d10, d11;
	glBegin ( GL_QUADS );

		for (int i=1 ; i<=NX ; i++ ) {
			x = (i-0.5)*HH;
			for (int j=1 ; j<=NY ; j++ ) {
				y = (j-0.5)*HH;

                d00 = container(i,j)     > 0. ? 0. : 200;
				d01 = container(i,j+1)   > 0. ? 0. : 200;
				d10 = container(i+1,j)   > 0. ? 0. : 200;
				d11 = container(i+1,j+1) > 0. ? 0. : 200;

				glColor3f ( d00, d00, d00 ); glVertex2f ( x, y );
				glColor3f ( d10, d10, d10 ); glVertex2f ( x+HH, y );
				glColor3f ( d11, d11, d11 ); glVertex2f ( x+HH, y+HH );
				glColor3f ( d01, d01, d01 ); glVertex2f ( x, y+HH );
			}
		}

	glEnd ();
}

void ParticleLevelSet2D::draw_points ( void )
{
	Float x, y, d00;
	glBegin ( GL_POINTS );

		for (int i=1 ; i<=NX ; i++ ) {
			x = (i-0.5)*HH;
			for (int j=1 ; j<=NY ; j++ ) {
				y = (j-0.5)*HH;
                d00 = container(i,j)     > 0. ? 0. : 1.0;
				glColor3f ( d00, d00, d00 ); glVertex2f ( x, y );
			}
		}

	glEnd ();
}

void ParticleLevelSet2D::FindArea()
{
    int a = 0;
	for (int i=1 ; i<=NX ; i++ ) 
		for (int j=1 ; j<=NY ; j++ ) 
            a += container(i,j) > 0. ? 0 : 1;
    std::cout << std::endl << std::endl << "Area: " << a << std::endl << std::endl;
    area = 0;
}

void ParticleLevelSet2D::draw_particles (void)
{
    Float x,y;
    glBegin ( GL_POINTS );

        ParticleSet2D::cIterator pit, end = container.pset.end();
        for(pit = container.pset.begin(); pit != end; ++pit) {
            Vec2d pos;
            Particle2D& particle = *(*pit);
		    particle.GetPosition(pos);
		    Double phi = container.lset.LinearSample(pos);
		    Double sign = particle.Sign();

            x = (pos[0] - .5) * HH;
            y = (pos[1] - .5) * HH;

            if(sign * phi < 0) {
                if(sign > 0) glColor3f ( 0.0, 1.0, 0.0 );
                else glColor3f ( 1.0, 1.0, 0.0 );
            }
            else {
                if(sign > 0) glColor3f ( 1.0, 0.0, 0.0 );
                else glColor3f ( 0.0, 0.0, 1.0 );
            }
            glVertex2f ( x, y );
        }

    glEnd();
}

ParticleLevelSet2D::ParticleLevelSet2D(GUIHelperInterface* gui) :container(NX, NY, HH, DT2D)
{
    m_guiHelper = gui;
    m_app = gui->getAppInterface();
     
    timer.setStartTick();
}


ParticleLevelSet2D::~ParticleLevelSet2D()
{

 
}
bool ParticleLevelSet2D::keyboardCallback(int key, int state)
{
	switch ( key )
	{
		case 'c':
		case 'C':
			container.Clear();
			break;

		case 'q':
		case 'Q':
			exit ( 0 );
			break;

		case 'x':
		case 'X':
			dvel = dvel++ % 4;
			break;
        case 'p':
        case 'P':
            pause = pause ? 0 : 1;
            break;
        case 'a':
        case 'A':
            area = 1;
            break;
        case 'f':
        case 'F':
            rate = rate ? 0 : 1;
            break;
	}
    return true;
}
 
void ParticleLevelSet2D::stepSimulation(float deltaTime)
{
    if(!pause) { container.Update(); pause = 0; dt += DT2D; }
    if(dt > (628 * HH)) { pause = 0; dt = 0; std::cout << rot_time << std::endl; rot_time = 0; }
    if(area) FindArea();

 
}

void ParticleLevelSet2D::renderScene( )
{	
	fps_time += timer.time_s();
    rot_time += timer.time_s();
	timer.setStartTick();
	frames++;

	if(fps_time >= 10.0 && rate)
	{
		std::cout<<"Framerate:"<<(frames/fps_time)<<std::endl;
		frames = 0;
		fps_time = 0;
	}
 
		if      ( dvel==0 ) draw_velocity ();
		else if	( dvel==1 )	draw_density ();
        else if ( dvel==2 ) draw_points ();
        else if ( dvel==3 ) draw_particles ();

 
}

 