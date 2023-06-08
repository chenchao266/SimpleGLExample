#ifndef OPENGL_BROWSER_GUI_H
#define OPENGL_BROWSER_GUI_H

#include "Component/ExampleBrowserInterface.h"
enum b3ConfigureDebugVisualizerEnum
{
    COV_ENABLE_GUI = 1,
    COV_ENABLE_SHADOWS,
    COV_ENABLE_WIREFRAME,
    COV_ENABLE_VR_TELEPORTING,
    COV_ENABLE_VR_PICKING,
    COV_ENABLE_VR_RENDER_CONTROLLERS,
    COV_ENABLE_RENDERING,
    COV_ENABLE_SYNC_RENDERING_INTERNAL,
    COV_ENABLE_KEYBOARD_SHORTCUTS,
    COV_ENABLE_MOUSE_PICKING,
    COV_ENABLE_Y_AXIS_UP,
    COV_ENABLE_TINY_RENDERER,
    COV_ENABLE_RGB_BUFFER_PREVIEW,
    COV_ENABLE_DEPTH_BUFFER_PREVIEW,
    COV_ENABLE_SEGMENTATION_MARK_PREVIEW,
    COV_ENABLE_PLANAR_REFLECTION,
    COV_ENABLE_SINGLE_STEP_RENDERING,
};
class OpenGLExampleBrowser : public ExampleBrowserInterface
{
     
public:	
    struct OpenGLExampleBrowserInternalData* m_data;
    static OpenGLExampleBrowser* instance();
	OpenGLExampleBrowser( );
	virtual ~OpenGLExampleBrowser();

	virtual CommonExampleInterface* getCurrentExample();

	virtual bool init(int argc, char* argv[]);

	virtual void update(float deltaTime);

	virtual void updateGraphics();

	virtual bool requestedExit();
     
	 void registerFileImporter(const char* extension, CommonExampleInterface::CreateFunc* createFunc);
};

#endif  //OPENGL_BROWSER_GUI_H
