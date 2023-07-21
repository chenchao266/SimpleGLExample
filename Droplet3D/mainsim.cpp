#include <iostream>
#include <sstream>

#include <glad/gl.h>
 
#include "CommonInterfaces/CommonGraphicsAppInterface.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "CommonInterfaces/CommonRenderInterface.h"
#include "CommonInterfaces/CommonExampleInterface.h"
#include "CommonInterfaces/CommonGUIHelperInterface.h"
//#include "../UnifiedPhysics/render_particles.h"

#include "Sim.h"
#include "MeshIO.h"
#include <iomanip>
//#include "YImage.h"
//#include "TextRenderer.h"
#include "Component/Timer.h"
#include "OpenGLWindow/tgaimage.h"



class BPSSim3D : public CommonExampleInterface
{
    CommonGraphicsApp* m_app;
    GUIHelperInterface* m_guiHelper;
    bool headless;
    
    Sim g_sim;
    int win_w;
    int win_h;
    
    bool step;
    bool run;
    int substep_id;
    bool autoload;
    bool finished;
    
    double view_theta;
    double view_alpha;
    double view_dist;
    EigenVec3d view_center;
    double view_field_scale;
    
    double mouse_x;
    double mouse_y;
    
    bool ldrag;
    bool sldrag;
    double ldrag_start_x;
    double ldrag_start_y;
    bool rdrag;
    double rdrag_start_x;
    double rdrag_start_y;
    Sim::RenderMode render_mode;
    
    int selection_mode;

public:
    BPSSim3D(GUIHelperInterface* app);
    ~BPSSim3D();
    virtual void physicsDebugDraw(int debugDrawMode)
    {
    }
    virtual void initPhysics();

    virtual void exitPhysics();
    virtual void stepSimulation(float deltaTime);

    virtual void renderScene();
    // GLUT callbacks


    virtual bool keyboardCallback(int key, int state);

    virtual bool mouseButtonCallback(int button, int state, float x, float y);

    virtual bool mouseMoveCallback(float x, float y);
    void clear();
    void motion(double x, double y);
};

void BPSSim3D::clear()
{
    view_theta = 0;
    view_alpha = 0;
    view_dist = 4;
    view_center.Zero();
    view_field_scale = 1;
    
    mouse_x = 0;
    mouse_y = 0;
    
    ldrag = false;
    sldrag = false;
    ldrag_start_x = 0;
    ldrag_start_y = 0;
    rdrag = false;
    rdrag_start_x = 0;
    rdrag_start_y = 0;
}

void BPSSim3D::renderScene()
{
    // object center and zoom
    EigenVec3d center(0, 0, 0);
    for (size_t i = 0; i < g_sim.bps()->mesh().nv(); i++)
        center += g_sim.bps()->pos(i);
    center /= g_sim.bps()->mesh().nv();
    Vec3d radius(0, 0, 0);
    for (size_t i = 0; i < g_sim.bps()->mesh().nv(); i++)
        for (size_t j = 0; j < 3; j++)
            radius[0] = max(radius[0], (g_sim.bps()->pos(i) - center)[0]);
    double min_d = max(max(radius[0], radius[1]), radius[2]) * 2.2;
//    view_dist = max(min_d, view_dist);
    
    glClearColor(1, 1, 1, 1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double ar = (double)win_w / win_h;
    double vfh = 0.01 * 0.4;
    glFrustum(-vfh * ar, vfh * ar, -vfh, vfh, 0.01, 500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(0, 0, -view_dist);
    glRotated(-90, 1, 0, 0);
    glRotated(view_alpha, 1, 0, 0);
    glRotated(view_theta, 0, 0, 1);
    glTranslated(-view_center.x(), -view_center.y(), -view_center.z());
    
    glBegin(GL_LINES);
    glColor3d(1, 0, 0);     glVertex3d(0, 0, 0);    glVertex3d(2, 0, 0);
    glColor3d(0, 1, 0);     glVertex3d(0, 0, 0);    glVertex3d(0, 2, 0);
    glColor3d(0, 0, 1);     glVertex3d(0, 0, 0);    glVertex3d(0, 0, 2);
    glEnd();
    //press 'g'
    g_sim.render(render_mode, view_field_scale, false, EigenVec2d((double)mouse_x / win_w * 2 - 1, 1 - (double)mouse_y / win_h * 2), selection_mode);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, win_w, 0, win_h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    std::stringstream ss;
    ss << "T = " << g_sim.time();
    std::string s = ss.str();
    glColor4f(0.0, 0.0, 0.0, 1.0);
    //TextRenderer::renderString(s, 32, win_h - 32, 0, 32);
    
    ss.str("");
    if (g_sim.nearestVertex() >= 0)
        ss << "Picked: Vertex " << g_sim.nearestVertex();
    else if (g_sim.nearestEdge() >= 0)
        ss << "Picked: Edge " << g_sim.nearestEdge();
    else if (g_sim.nearestFace() >= 0)
        ss << "Picked: Face " << g_sim.nearestFace();
    s = ss.str();
    if (s.size() > 0)
        //TextRenderer::renderString(s, 32, win_h - 32 * 2, 0, 32);
    
    ss.str("");
    double influx = g_sim.currentInflux();
    ss << "Influx: " << influx;
    s = ss.str();
    //TextRenderer::renderString(s, 32, win_h - 32 * 3, 0, 32);
}

void BPSSim3D::stepSimulation(float)
{
    if (run || step)
    {
        g_sim.step(substep_id);
        step = false;
        substep_id = 0;
        std::cout << "Finished step: T = " << g_sim.time() << std::endl;
        g_sim.stepOutput();
        
        if (g_sim.isFinished())
        {
            finished = true;
            if (!headless)
            {
                ;
            }
        }
    }
    
    if (autoload)
    {
        if (!g_sim.incrementTime(10, true))
        {
            ;
        }
        g_sim.stepOutput();
    }
}

bool  BPSSim3D::keyboardCallback(int k, int sc)
{
    bool bshift = m_app->m_window->isModifierKeyPressed(B3G_SHIFT);
    bool balt = m_app->m_window->isModifierKeyPressed(B3G_ALT);
    bool bctrl = m_app->m_window->isModifierKeyPressed(B3G_CONTROL);
    {
        if (k == 'q' || k == B3G_ESCAPE)
        {
            ;
        } else if (k == ' ')
        {
            run = !run;
        } else if (k == 's')
        {
            step = true;
        } else if (k >= '0' && k <= '9')
        {
            step = true;
            substep_id = k - '0';
        } else if (k == 'm')
        {
            render_mode = (Sim::RenderMode)(((int)render_mode + (bshift ? -1 : 1)) % ((int)Sim::RM_COUNT));
            std::cout << "Render mode: " << (int)render_mode << std::endl;
        } else if (k == 'v')
        {
            selection_mode = (!(bshift) ? (selection_mode | Sim::SM_VERTEX) : (selection_mode & ~Sim::SM_VERTEX));
            std::cout << "Mouse cursor selecting" << ((selection_mode & Sim::SM_VERTEX) ? " vertices" : "") << ((selection_mode & Sim::SM_EDGE) ? " edges" : "") << ((selection_mode & Sim::SM_FACE) ? " faces" : "") << "." << std::endl;
        } else if (k == 'e')
        {
            selection_mode = (!(bshift) ? (selection_mode | Sim::SM_EDGE) : (selection_mode & ~Sim::SM_EDGE));
            std::cout << "Mouse cursor selecting" << ((selection_mode & Sim::SM_VERTEX) ? " vertices" : "") << ((selection_mode & Sim::SM_EDGE) ? " edges" : "") << ((selection_mode & Sim::SM_FACE) ? " faces" : "") << "." << std::endl;
        } else if (k == 'f')
        {
            selection_mode = (!(bshift) ? (selection_mode | Sim::SM_FACE) : (selection_mode & ~Sim::SM_FACE));
            std::cout << "Mouse cursor selecting" << ((selection_mode & Sim::SM_VERTEX) ? " vertices" : "") << ((selection_mode & Sim::SM_EDGE) ? " edges" : "") << ((selection_mode & Sim::SM_FACE) ? " faces" : "") << "." << std::endl;
        } else if (k == 'n')
        {
            g_sim.showPrimitiveInfo();
        } else if (k == '[')      // [
        {
            int inc = -1;
            if (bshift) inc *= 10;
            if (balt) inc *= 100;
            g_sim.incrementTime(inc, !(bctrl));   // hold down control to just decrement time, without loading REC files
        } else if (k == ']')     // ]
        {
            int inc = 1;
            if (bshift) inc *= 10;
            if (balt) inc *= 100;
            g_sim.incrementTime(inc, !(bctrl));   // hold down control to just increment time, without loading REC files
        } else if (k == ',')             // ,
        {
            g_sim.bps()->intermediate_v_select(-1);
            std::cout << "Intermediate velocity [" << g_sim.bps()->intermediate_v_selector() << "]: " << g_sim.bps()->intermediate_v().second << std::endl;
        } else if (k == '.')            // .
        {
            g_sim.bps()->intermediate_v_select(1);
            std::cout << "Intermediate velocity [" << g_sim.bps()->intermediate_v_selector() << "]: " << g_sim.bps()->intermediate_v().second << std::endl;
        } else if (k == 'o')
        {
            std::stringstream obj_ss;
            obj_ss << "mesh_" << ::time(NULL) << ".obj";
            MeshIO::saveOBJ(*(g_sim.bps()), obj_ss.str().c_str());
        } else if (k == 'p')
        {
            std::stringstream png_ss;
            png_ss << "screenshot_" << ::time(NULL) << ".png";
            
            int w = m_app->m_window->getWidth(), h = m_app->m_window->getHeight();
            //glfwGetFramebufferSize(window, &w, &h);
            
            TGAImage img(w, h, 4);
            glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)(img.buffer()));
            img.flip_vertically();
            img.write_tga_file(png_ss.str().c_str());
        } else if (k == 'r')
        {
            std::stringstream rec_ss;
            rec_ss << "mesh_" << ::time(NULL) << ".rec";
            MeshIO::save(*(g_sim.bps()), rec_ss.str().c_str());
        } else if (k == 'l')
        {
            autoload = true;
        } else if (k == 't')
        {
            if (bshift)
                g_sim.showSelected();
            else
                g_sim.selectPrimitive();
        } else if (k == 'h')
        {
            initPhysics();
        } else if (k == 'c')
        {
            if (g_sim.nearestVertex() >= 0)
            {
                view_center = g_sim.bps()->pos(g_sim.nearestVertex());
            } else if (g_sim.nearestEdge() >= 0)
            {
                Vec2st e = g_sim.bps()->mesh().m_edges[g_sim.nearestEdge()];
                view_center = (g_sim.bps()->pos(e[0]) + g_sim.bps()->pos(e[1])) / 2;
            } else if (g_sim.nearestFace() >= 0)
            {
                Vec3st f = g_sim.bps()->mesh().m_tris[g_sim.nearestFace()];
                view_center = (g_sim.bps()->pos(f[0]) + g_sim.bps()->pos(f[1]) + g_sim.bps()->pos(f[2])) / 3;
            }
        } else if (k == 'b')
        {
            g_sim.bps()->setVerbose(!g_sim.bps()->verbose());
        } else if (k == '=')
        {
            if (bshift)
                view_field_scale = std::abs(view_field_scale);    // turn the field display on (e.g. velocity field)
            else
                view_field_scale *= 1.5;
        } else if (k == '-')
        {
            if (bshift)
                view_field_scale = -std::abs(view_field_scale);   // turn the field display off (e.g. velocity field)
            else
                view_field_scale /= 1.5;
        }
    }
    return true;
}

bool  BPSSim3D::mouseButtonCallback(int button, int action, float x, float y)
{
    bool bshift = m_app->m_window->isModifierKeyPressed(B3G_SHIFT);
    if (button == MOUSE_LEFT && action == MOUSE_DOWN)
    {
        if (bshift)
            sldrag = true;
        else
            ldrag = true;
        ldrag_start_x = x;  ldrag_start_y = y;
    } else if (button == MOUSE_LEFT && action == MOUSE_UP)
    {
        ldrag = false;
        sldrag = false;
    } else if (button == MOUSE_RIGHT && action == MOUSE_DOWN)
    {
        rdrag = true;
        rdrag_start_x = x;  rdrag_start_y = y;
    } else if (button == MOUSE_RIGHT && action == MOUSE_UP)
    {
        rdrag = false;
    }
    motion(x, y);
    return true;
}

void  BPSSim3D::motion( double x, double y)
{
    if (ldrag || sldrag)
    {
        if (ldrag)
        {
            view_theta += (x - ldrag_start_x) * 1.0;
            view_alpha += (y - ldrag_start_y) * 1.0;
            if (view_alpha >  90) view_alpha =  90;
            if (view_alpha < -90) view_alpha = -90;
        } else
        {
            double a = view_alpha * M_PI / 180;
            double t = view_theta * M_PI / 180;
            EigenVec3d right(cos(-t), sin(-t), 0);
            EigenVec3d up(-sin(a) * sin(-t), sin(a) * cos(-t), cos(a));
            view_center -= (right * (x - ldrag_start_x) - up * (y - ldrag_start_y)) * view_dist * 0.001;
        }
        
        ldrag_start_x = x;
        ldrag_start_y = y;
    }
    if (rdrag)
    {
        view_dist *= pow(2.0, (y - rdrag_start_y) * 0.01);
        
        rdrag_start_x = x;
        rdrag_start_y = y;
    }
    
    mouse_x = x;
    mouse_y = y;
}
 
 
BPSSim3D::BPSSim3D(GUIHelperInterface* gui):g_sim(false)
{
    m_guiHelper = gui;
    m_app = gui->getAppInterface();

    {
        //std::cout << "Usage:\n\tDroplet3D option_file [param1 override_value1 [param2 override_value2 [...]]].\n" << std::endl;

    }

    // simulation setup
    // CSim::TimerMan::setReport(false);
    clear();
    run = false;
    step = false;
    substep_id = 0;
    autoload = false;
    finished = false;

    win_w = 1000;
    win_h = 1000;


    render_mode = Sim::RM_TRANSPARENT;
}
void BPSSim3D::initPhysics()
{
    selection_mode = Sim::SM_VERTEX | Sim::SM_EDGE | Sim::SM_FACE;
    std::string option_file;
    std::vector<std::pair<std::string, std::string> > option_overrides;

    {

        std::string f;
        std::string s;
        option_overrides.push_back(std::make_pair(f, s));
    }

    if (!g_sim.loadOptions(option_file, option_overrides))
        return;

    headless = g_sim.headless();
    if (!headless)
    {
        //TextRenderer::initialize();
    }


    if (!g_sim.init())
        return ;

    std::cout << "Initialization complete. Starting the simulation..." << std::endl;

    // main loop
    if (headless)
    {
        run = true;
    }

}

void BPSSim3D::exitPhysics()
{
    g_sim.statsReport();
     
}

