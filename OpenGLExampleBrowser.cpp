#include "OpenGLExampleBrowser.h"
//#include "LinearMath/btQuickprof.h"
#include "OpenGLWindow/OpenGLInclude.h"
//#include "OpenGLWindow/SimpleOpenGL2App.h"
#if 1
#include "OpenGLWindow/SimpleOpenGL3App.h"
#endif
#include "CommonInterfaces/CommonRenderInterface.h"
#ifdef __APPLE__
#include "OpenGLWindow/MacOpenGLWindow.h"
#else
#ifdef _WIN32
#include "OpenGLWindow/Win32OpenGLWindow.h"
#else
//let's cross the fingers it is Linux/X11
#ifdef BT_USE_EGL
#include "OpenGLWindow/EGLOpenGLWindow.h"
#else
#include "OpenGLWindow/X11OpenGLWindow.h"
#endif  //BT_USE_EGL
#endif  //_WIN32
#endif  //__APPLE__
//#include "Gwork/Renderers/OpenGL_DebugFont.h"
//#include "LinearMath/btThreads.h"
#include "Bullet3Common/b3Vector3.h"
#include "assert.h"
#include <stdio.h>
#include "OpenGLWindow/gwenInternalData.h"
#include "OpenGLWindow/gwenUserInterface.h"
#include "Component/b3Clock.h"
//#include "ChromeTraceUtil.h"
#include "OpenGLWindow/GwenParameterInterface.h"
#ifndef BT_NO_PROFILE
//#include "OpenGLWindow/GwenProfileWindow.h"
#endif
#include "OpenGLWindow/GwenTextureWindow.h"
#include "OpenGLWindow/GraphingTexture.h"
#include "CommonInterfaces/Common2dCanvasInterface.h"
#include "CommonInterfaces/CommonExampleInterface.h"
#include "Bullet3Common/b3CommandLineArgs.h"
#include "OpenGLWindow/SimpleCamera.h"
//#include "OpenGLWindow/SimpleOpenGL2Renderer.h"
#include "ExampleEntries.h"
#include "OpenGLWindow/OpenGLGuiHelper.h"
#include "Bullet3Common/b3FileUtils.h"

#include "OpenGLWindow/btIDebugDraw.h"
//quick test for file import, @todo(erwincoumans) make it more general and add other file formats
 
#include "Bullet3Common/b3HashMap.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"
#include "stb_image/stb_truetype.h"
extern bool useShadowMap;
struct GL3TexLoader : public MyTextureLoader
{
	b3HashMap<b3HashString, GLint> m_hashMap;

	virtual void LoadTexture(const Gwk::Texture* pTexture)
	{
		Gwk::String namestr = pTexture->name;
		const char* n = namestr.c_str();
		GLint* texIdPtr = m_hashMap[n];
		if (texIdPtr)
		{
			pTexture->m_intData = *texIdPtr;
		}
	}
	virtual void FreeTexture(const Gwk::Texture* pTexture)
	{
	}
};
#include <string.h>
struct FileImporterByExtension
{
    std::string m_extension;
    CommonExampleInterface::CreateFunc* m_createFunc;
};


struct OpenGLExampleBrowserInternalData
{
	Gwk::Renderer::Base* m_gwenRenderer;
	//CommonGraphicsApp* m_app;
#ifndef BT_NO_PROFILE
	//MyProfileWindow* m_profWindow;
#endif  //BT_NO_PROFILE
	b3AlignedObjectArray<Gwk::Controls::TreeNode*> m_nodes;
	//GwenUserInterface* m_gui;
	GL3TexLoader* m_myTexLoader;
	struct MyMenuItemHander* m_handler2;
	b3AlignedObjectArray<MyMenuItemHander*> m_handlers;
    OpenGLGuiHelper* guiHelper = 0;

    CommonExampleInterface* sCurrentDemo = 0;
    const char* gPngFileName = 0;

    const char* startFileName = "0_Bullet3Demo.txt";
    bool renderGui = true;
    float gFixedTimeStep = 0;
     int sCurrentDemoIndex = -1;
     int sCurrentHightlighted = 0;
     int gPngSkipFrames = 0;
     int gDebugDrawFlags = 0;
     int gRenderDevice = -1;
     int gWindowBackend = 0;

     bool gAllowRetina = true;
     bool gDisableDemoSelection = false;
     bool visualWireframe = false;
     bool gBlockGuiMessages = false;

     bool renderVisualGeometry = true;
     bool renderGrid = true;
     bool gEnableRenderLoop = true;
     bool gEnableDefaultKeyboardShortcuts = true;
     bool gEnableDefaultMousePicking = true;
     bool pauseSimulation = false;
     bool singleStepSimulation = false;
     int mouseOldX, mouseOldY;
     int mouseButtons = 0;
     bool mouseMoved = false;
     bool mousePressed = false;
     Vec3f rotate;
     Vec3f translate;

    b3AlignedObjectArray<FileImporterByExtension> gFileImporterByExtension;
	OpenGLExampleBrowserInternalData()
		: m_gwenRenderer(0),
		guiHelper(0),
		sCurrentDemo(0),
		  //m_gui(0),
		  m_myTexLoader(0),
		  m_handler2(0)
	{
	}
};
 

 
#ifndef BT_NO_PROFILE
//MyProfileWindow* s_profWindow = 0;
#endif  //BT_NO_PROFILE
 

char staticPngFileName[1024];
  
/*extern*/ bool gDisableDeactivation;
  
//#include <float.h>
//unsigned int fp_control_state = _controlfp(_EM_INEXACT, _MCW_EM);

void deleteDemo(OpenGLExampleBrowserInternalData* internalData)
{
	if (internalData->sCurrentDemo)
	{
		internalData->sCurrentDemo->exitPhysics();
		SimpleOpenGL3App::instance()->m_renderer->removeAllInstances();
		delete internalData->sCurrentDemo;
        internalData->sCurrentDemo = 0;
		delete internalData->guiHelper;
		internalData->guiHelper = 0;

		//		CProfileManager::CleanupMemory();
	}
}



b3KeyboardCallback prevKeyboardCallback = 0;

void MyKeyboardCallback(int key, int state)
{
	//b3Printf("key=%d, state=%d", key, state);
	bool handled = false;
    OpenGLExampleBrowserInternalData* internalData = OpenGLExampleBrowser::instance()->m_data;
	if (internalData->renderGui)
	{
		if (GwenUserInterface::instance() && !handled)
		{
			handled = GwenUserInterface::instance()->keyboardCallback(key, state);
		}
	}

	if (!handled && internalData->sCurrentDemo)
	{
		handled = internalData->sCurrentDemo->keyboardCallback(key, state);
	}

	//checkout: is it desired to ignore keys, if the demo already handles them?
	//if (handled)
	//	return;
 

	if (internalData->gEnableDefaultKeyboardShortcuts)
	{
		if (key == 'a' && state)
		{
            internalData->gDebugDrawFlags ^= btIDebugDraw::DBG_DrawAabb;
		}
		if (key == 'c' && state)
		{
            internalData->gDebugDrawFlags ^= btIDebugDraw::DBG_DrawContactPoints;
		}
		if (key == 'd' && state)
		{
            internalData->gDebugDrawFlags ^= btIDebugDraw::DBG_NoDeactivation;
			gDisableDeactivation = ((internalData->gDebugDrawFlags & btIDebugDraw::DBG_NoDeactivation) != 0);
		}
		if (key == 'j' && state)
		{
            internalData->gDebugDrawFlags ^= btIDebugDraw::DBG_DrawFrames;
		}

		if (key == 'k' && state)
		{
            internalData->gDebugDrawFlags ^= btIDebugDraw::DBG_DrawConstraints;
		}

		if (key == 'l' && state)
		{
            internalData->gDebugDrawFlags ^= btIDebugDraw::DBG_DrawConstraintLimits;
		}
		if (key == 'w' && state)
		{
            internalData->visualWireframe = !internalData->visualWireframe;
            internalData->gDebugDrawFlags ^= btIDebugDraw::DBG_DrawWireframe;
		}

		if (key == 'v' && state)
		{
            internalData->renderVisualGeometry = !internalData->renderVisualGeometry;
		}
		if (key == 'g' && state)
		{
            internalData->renderGrid = !internalData->renderGrid;
            internalData->renderGui = !internalData->renderGui;
		}

		if (key == 'i' && state)
		{
            internalData->pauseSimulation = !internalData->pauseSimulation;
		}
		if (key == 'o' && state)
		{
            internalData->singleStepSimulation = true;
		}

		if (key == 'p')
		{
			if (state)
			{
				//b3ChromeUtilsStartTimings();
			}
			else
			{
#ifdef _WIN32
				//b3ChromeUtilsStopTimingsAndWriteJsonFile("timings");
#else
				//b3ChromeUtilsStopTimingsAndWriteJsonFile("/tmp/timings");
#endif
			}
		}

#if 1
		if (key == 's' && state)
		{
			useShadowMap = !useShadowMap;
		}
#endif
		if (key == B3G_F1)
		{
			static int count = 0;
			if (state)
			{
				b3Printf("F1 pressed %d", count++);

				if (internalData->gPngFileName)
				{
					b3Printf("disable image dump");

                    internalData->gPngFileName = 0;
				}
				else
				{
                    internalData->gPngFileName = ExampleEntriesAll::instance()->getExampleName(internalData->sCurrentDemoIndex);
					b3Printf("enable image dump %s", internalData->gPngFileName);
				}
			}
			else
			{
				b3Printf("F1 released %d", count++);
			}
		}
	}
	if (key == B3G_ESCAPE && SimpleOpenGL3App::instance()->m_window)
	{
		SimpleOpenGL3App::instance()->m_window->setRequestExit();
	}

	if (prevKeyboardCallback)
		prevKeyboardCallback(key, state);
}

b3MouseMoveCallback prevMouseMoveCallback = 0;
  void MyMouseMoveCallback(float x, float y)
{
	bool handled = false;
    OpenGLExampleBrowserInternalData*  internalData = OpenGLExampleBrowser::instance()->m_data;
	if (internalData->sCurrentDemo)
		handled = internalData->sCurrentDemo->mouseMoveCallback(x, y);
	if (internalData->renderGui)
	{
		if (!handled && GwenUserInterface::instance())
			handled = GwenUserInterface::instance()->mouseMoveCallback(x, y);
	}
	if (!handled)
    {
        float dx, dy;
        dx = (float)(x - internalData->mouseOldX);
        dy = (float)(y - internalData->mouseOldY);

        if (internalData->mouseButtons == (1 << MOUSE_LEFT))
        {
            internalData->rotate.x += dy * 0.2f;
            internalData->rotate.y += dx * 0.2f;
            internalData->rotate.z;
        }
        else if (internalData->mouseButtons == (1 << MOUSE_MIDDLE))
        {
            internalData->translate.x += dx * 0.01f;
            internalData->translate.y -= dy * 0.01f;
        }
        else if (internalData->mouseButtons == (1 << MOUSE_RIGHT))
        {
            internalData->translate.z += dy * 0.01f;
        }
        internalData->mouseMoved = true;
        internalData->mouseOldX = x;
        internalData->mouseOldY = y;
		if (prevMouseMoveCallback)
			prevMouseMoveCallback(x, y);
	}
}

b3MouseButtonCallback prevMouseButtonCallback = 0;

  void MyMouseButtonCallback(int button, int state, float x, float y)
{
	bool handled = false;

    OpenGLExampleBrowserInternalData* internalData = OpenGLExampleBrowser::instance()->m_data;
	//try picking first
	if (internalData->sCurrentDemo)
		handled = internalData->sCurrentDemo->mouseButtonCallback(button, state, x, y);

	if (internalData->renderGui)
	{
		if (!handled && GwenUserInterface::instance())
			handled = GwenUserInterface::instance()->mouseButtonCallback(button, state, x, y);
	}
	if (!handled)
	{
        if (state == MOUSE_DOWN)
        {
            internalData->mouseButtons |= (1 << button);
            internalData->mousePressed = true;
        }
        else if (state == MOUSE_UP)
        {
            internalData->mouseButtons = 0;
            if (internalData->mouseMoved)
            {
            }
            else
            {

            }
            internalData->mousePressed = false;
        }
        internalData->mouseMoved = false;
        internalData->mouseOldX = x;
        internalData->mouseOldY = y;
		if (prevMouseButtonCallback)
			prevMouseButtonCallback(button, state, x, y);
	}
	//	b3DefaultMouseButtonCallback(button,state,x,y);
}



void OpenGLExampleBrowser::registerFileImporter(const char* extension, CommonExampleInterface::CreateFunc* createFunc)
{
	FileImporterByExtension fi;
	fi.m_extension = extension;
	fi.m_createFunc = createFunc;
    m_data->gFileImporterByExtension.push_back(fi);
}
 
void OpenGLExampleBrowserVisualizerFlagCallback(int flag, bool enable)
{
    OpenGLExampleBrowserInternalData* internalData = OpenGLExampleBrowser::instance()->m_data;

	if (flag == COV_ENABLE_Y_AXIS_UP)
	{
		//either Y = up or Z
		int upAxis = enable ? 1 : 2;
		SimpleOpenGL3App::instance()->setUpAxis(upAxis);
	}

	if (flag == COV_ENABLE_RENDERING)
	{
        internalData->gEnableRenderLoop = (enable != 0);
	}

	if (flag == COV_ENABLE_SINGLE_STEP_RENDERING)
	{
		if (enable)
		{
            internalData->gEnableRenderLoop = false;
            internalData->singleStepSimulation = true;
		}
		else
		{
            internalData->gEnableRenderLoop = true;
            internalData->singleStepSimulation = false;
		}
	}
 
	if (flag == COV_ENABLE_SHADOWS)
	{
		useShadowMap = enable;
	}
	if (flag == COV_ENABLE_GUI)
	{
        internalData->renderGui = enable;
        internalData->renderGrid = enable;
	}

	if (flag == COV_ENABLE_KEYBOARD_SHORTCUTS)
	{
        internalData->gEnableDefaultKeyboardShortcuts = enable;
	}
	if (flag == COV_ENABLE_MOUSE_PICKING)
	{
        internalData->gEnableDefaultMousePicking = enable;
	}

	if (flag == COV_ENABLE_WIREFRAME)
    {
 

        internalData->visualWireframe = enable;
		if (internalData->visualWireframe)
		{
            internalData->gDebugDrawFlags |= btIDebugDraw::DBG_DrawWireframe;
		}
		else
		{
            internalData->gDebugDrawFlags &= ~btIDebugDraw::DBG_DrawWireframe;
		}
	}
}

void openFileDemo(OpenGLExampleBrowserInternalData* internalData, const char* filename)
{
	deleteDemo(internalData);

	internalData->guiHelper = new OpenGLGuiHelper(SimpleOpenGL3App::instance(), false);
	internalData->guiHelper->setVisualizerFlagCallback(OpenGLExampleBrowserVisualizerFlagCallback);

	SimpleOpenGL3App::instance()->m_parameterInterface->removeAllParameters();

	CommonExampleOptions options(internalData->guiHelper, 1);
	options.m_fileName = filename;
	char fullPath[1024];
	sprintf(fullPath, "%s", filename);
	b3FileUtils::toLower(fullPath);

	for (int i = 0; i < internalData->gFileImporterByExtension.size(); i++)
	{
		if (strstr(fullPath, internalData->gFileImporterByExtension[i].m_extension.c_str()))
		{
			internalData->sCurrentDemo = internalData->gFileImporterByExtension[i].m_createFunc(options);
		}
	}

	if (internalData->sCurrentDemo)
	{
		internalData->sCurrentDemo->initPhysics();
		internalData->sCurrentDemo->resetCamera();
	}
}

void selectDemo(OpenGLExampleBrowserInternalData* internalData)
{
	bool resetCamera = (internalData->sCurrentDemoIndex != internalData->sCurrentHightlighted);
	internalData->sCurrentDemoIndex = internalData->sCurrentHightlighted;
 
	int numDemos = ExampleEntriesAll::instance()->getNumRegisteredExamples();

	if (internalData->sCurrentHightlighted > numDemos)
	{
        internalData->sCurrentHightlighted = 0;
	}
	deleteDemo(internalData);

	CommonExampleInterface::CreateFunc* func = ExampleEntriesAll::instance()->getExampleCreateFunc(internalData->sCurrentHightlighted);
	if (func)
	{
		if (SimpleOpenGL3App::instance()->m_parameterInterface)
		{
			SimpleOpenGL3App::instance()->m_parameterInterface->removeAllParameters();
		}
		int option = ExampleEntriesAll::instance()->getExampleOption(internalData->sCurrentHightlighted);
		internalData->guiHelper = new OpenGLGuiHelper(SimpleOpenGL3App::instance(), false);
		internalData->guiHelper->setVisualizerFlagCallback(OpenGLExampleBrowserVisualizerFlagCallback);

		CommonExampleOptions options(internalData->guiHelper, option);
		//options.m_sharedMem = sSharedMem;
		internalData->sCurrentDemo = (*func)(options);
		if (internalData->sCurrentDemo)
		{
			if (GwenUserInterface::instance())
			{
				GwenUserInterface::instance()->setStatusBarMessage("Status: OK", false);
			}
			b3Printf("Selected demo: %s", ExampleEntriesAll::instance()->getExampleName(internalData->sCurrentHightlighted));
			if (GwenUserInterface::instance())
			{
				GwenUserInterface::instance()->setExampleDescription(ExampleEntriesAll::instance()->getExampleDescription(internalData->sCurrentHightlighted));
			}

			internalData->sCurrentDemo->initPhysics();
			if (resetCamera)
			{
				internalData->sCurrentDemo->resetCamera();
			}
		}
	}
}

#include <stdio.h>

 void saveCurrentSettings(int currentEntry, const char* startFileName)
{
	FILE* f = fopen(startFileName, "w");
	if (f)
    {
        OpenGLExampleBrowserInternalData* internalData = OpenGLExampleBrowser::instance()->m_data;

		fprintf(f, "--start_demo_name=%s\n", ExampleEntriesAll::instance()->getExampleName(internalData->sCurrentDemoIndex));
		fprintf(f, "--mouse_move_multiplier=%f\n", SimpleOpenGL3App::instance()->getMouseMoveMultiplier());
		fprintf(f, "--mouse_wheel_multiplier=%f\n", SimpleOpenGL3App::instance()->getMouseWheelMultiplier());
		float red, green, blue;
		SimpleOpenGL3App::instance()->getBackgroundColor(&red, &green, &blue);
		fprintf(f, "--background_color_red= %f\n", red);
		fprintf(f, "--background_color_green= %f\n", green);
		fprintf(f, "--background_color_blue= %f\n", blue);
		fprintf(f, "--fixed_timestep= %f\n", internalData->gFixedTimeStep);
		if (!internalData->gAllowRetina)
		{
			fprintf(f, "--disable_retina");
		}
 

		fclose(f);
	}
};

 void loadCurrentSettings(const char* startFileName, b3CommandLineArgs& args)
{
	//int currentEntry= 0;
	FILE* f = fopen(startFileName, "r");
	if (f)
	{
		char oneline[1024];
		char* argv[] = {0, &oneline[0]};

		while (fgets(oneline, 1024, f) != NULL)
		{
			char* pos;
			if ((pos = strchr(oneline, '\n')) != NULL)
				*pos = '\0';
			args.addArgs(2, argv);
		}
		fclose(f);
	}
};
 
//in case of multi-threading, don't submit messages while the GUI is rendering (causing crashes)

void MyGuiPrintf(const char* msg)
{
    OpenGLExampleBrowserInternalData* internalData = OpenGLExampleBrowser::instance()->m_data;

	printf("b3Printf: %s\n", msg);
	if (!internalData->gDisableDemoSelection && !internalData->gBlockGuiMessages)
	{
		GwenUserInterface::instance()->textOutput(msg);
		GwenUserInterface::instance()->forceUpdateScrollBars();
	}
}

void MyStatusBarPrintf(const char* msg)
{
    OpenGLExampleBrowserInternalData* internalData = OpenGLExampleBrowser::instance()->m_data;

	printf("b3Printf: %s\n", msg);
	if (!internalData->gDisableDemoSelection && !internalData->gBlockGuiMessages)
	{
		bool isLeft = true;
		GwenUserInterface::instance()->setStatusBarMessage(msg, isLeft);
	}
}

void MyStatusBarError(const char* msg)
{
    OpenGLExampleBrowserInternalData* internalData = OpenGLExampleBrowser::instance()->m_data;

	printf("Warning: %s\n", msg);
	if (!internalData->gDisableDemoSelection && !internalData->gBlockGuiMessages)
	{
		bool isLeft = false;
		GwenUserInterface::instance()->setStatusBarMessage(msg, isLeft);
		GwenUserInterface::instance()->textOutput(msg);
		GwenUserInterface::instance()->forceUpdateScrollBars();
	}
	b3Assert(0);
}

struct MyMenuItemHander : public Gwk::Event::Handler
{
	int m_buttonId;
    OpenGLExampleBrowserInternalData* internalData;
	MyMenuItemHander(int buttonId)
		: m_buttonId(buttonId)
    {
        internalData = OpenGLExampleBrowser::instance()->m_data;
	}

	void onButtonA(Gwk::Controls::Base* pControl)
	{
		//const Gwk::String& name = pControl->GetName();
		Gwk::Controls::TreeNode* node = (Gwk::Controls::TreeNode*)pControl;
		//	Gwk::Controls::Label* l = node->GetButton();

		Gwk::String la = node->GetButton()->GetText();  // node->GetButton()->GetName();// GetText();
		Gwk::String laa = (la);
		//	const char* ha = laa.c_str();

		//printf("selected %s\n", ha);
		//int dep = but->IsDepressed();
		//int tog = but->GetToggleState();
		//		if (m_data->m_toggleButtonCallback)
		//		(*m_data->m_toggleButtonCallback)(m_buttonId, tog);
	}
	void onButtonB(Gwk::Controls::Base* pControl)
	{
		Gwk::Controls::Label* label = (Gwk::Controls::Label*)pControl;
		Gwk::String la = label->GetText();  // node->GetButton()->GetName();// GetText();
		Gwk::String laa = (la);
  
		if (!internalData->gDisableDemoSelection)
        {
     
			selectDemo(internalData);
			saveCurrentSettings(internalData->sCurrentDemoIndex, internalData->startFileName);
		}
	}
	void onButtonC(Gwk::Controls::Base* pControl)
	{
		/*Gwk::Controls::Label* label = (Gwk::Controls::Label*) pControl;
		Gwk::String la = label->GetText();// node->GetButton()->GetName();// GetText();
		Gwk::String laa = (la);
		const char* ha = laa.c_str();
         
		*/
	}
	void onButtonD(Gwk::Controls::Base* pControl)
	{
		/*		Gwk::Controls::Label* label = (Gwk::Controls::Label*) pControl;
		Gwk::String la = label->GetText();// node->GetButton()->GetName();// GetText();
		Gwk::String laa = (la);
		const char* ha = laa.c_str();
		*/
     
		if (!internalData->gDisableDemoSelection)
        {
           
			selectDemo(internalData);
			saveCurrentSettings(internalData->sCurrentDemoIndex, internalData->startFileName);
		}
	}

	void onButtonE(Gwk::Controls::Base* pControl)
	{
		//	printf("select %d\n",m_buttonId);
        internalData->sCurrentHightlighted = m_buttonId;
		GwenUserInterface::instance()->setExampleDescription(ExampleEntriesAll::instance()->getExampleDescription(m_buttonId));
	}

	void onButtonF(Gwk::Controls::Base* pControl)
	{
		//printf("selection changed!\n");
	}

	void onButtonG(Gwk::Controls::Base* pControl)
	{
		//printf("onButtonG !\n");
	}
};

void quitCallback()
{
	SimpleOpenGL3App::instance()->m_window->setRequestExit();
}

void fileOpenCallback()
{
	char filename[1024];
	int len = SimpleOpenGL3App::instance()->m_window->fileOpenDialog(filename, 1024);
	if (len)
	{
		//todo(erwincoumans) check if it is actually URDF
		//printf("file open:%s\n", filename);
		openFileDemo(OpenGLExampleBrowser::instance()->m_data, filename);
	}
}

#define MAX_GRAPH_WINDOWS 5

struct QuickCanvas : public Common2dCanvasInterface
{
	GL3TexLoader* m_myTexLoader;

	MyGraphWindow* m_gw[MAX_GRAPH_WINDOWS];
	GraphingTexture* m_gt[MAX_GRAPH_WINDOWS];
	int m_curNumGraphWindows;

	QuickCanvas(GL3TexLoader* myTexLoader)
		: m_myTexLoader(myTexLoader),
		  m_curNumGraphWindows(0)
	{
		for (int i = 0; i < MAX_GRAPH_WINDOWS; i++)
		{
			m_gw[i] = 0;
			m_gt[i] = 0;
		}
	}
	virtual ~QuickCanvas() {}
	virtual int createCanvas(const char* canvasName, int width, int height, int xPos, int yPos)
	{
		if (m_curNumGraphWindows < MAX_GRAPH_WINDOWS)
		{
			//find a slot
			int slot = m_curNumGraphWindows;
			b3Assert(slot < MAX_GRAPH_WINDOWS);
			if (slot >= MAX_GRAPH_WINDOWS)
				return 0;  //don't crash

			m_curNumGraphWindows++;

			MyGraphInput input(GwenUserInterface::instance()->getInternalData());
			input.m_width = width;
			input.m_height = height;
			input.m_xPos = xPos;
			input.m_yPos = yPos;
			input.m_name = canvasName;
			input.m_texName = canvasName;
			m_gt[slot] = new GraphingTexture;
			m_gt[slot]->create(width, height);
			int texId = m_gt[slot]->getTextureId();
			m_myTexLoader->m_hashMap.insert(canvasName, texId);
			m_gw[slot] = setupTextureWindow(input);

			return slot;
		}
		return -1;
	}
	virtual void destroyCanvas(int canvasId)
	{
		b3Assert(canvasId >= 0);
		delete m_gt[canvasId];
		m_gt[canvasId] = 0;
		destroyTextureWindow(m_gw[canvasId]);
		m_gw[canvasId] = 0;
		m_curNumGraphWindows--;
	}
	virtual void setPixel(int canvasId, int x, int y, const Colorb& clr)
	{
		b3Assert(canvasId >= 0);
		b3Assert(canvasId < m_curNumGraphWindows);
		m_gt[canvasId]->setPixel(x, y, clr);
	}

	virtual void getPixel(int canvasId, int x, int y, Colorb& clr)
	{
		b3Assert(canvasId >= 0);
		b3Assert(canvasId < m_curNumGraphWindows);
		m_gt[canvasId]->getPixel(x, y, clr);
	}

	virtual void refreshImageData(int canvasId)
	{
		m_gt[canvasId]->uploadImageData();
	}
};

static OpenGLExampleBrowser* gBrowser;
OpenGLExampleBrowser* OpenGLExampleBrowser::instance()
{ 
    return gBrowser;
}

OpenGLExampleBrowser::OpenGLExampleBrowser( )
{
    gBrowser = this;
	m_data = new OpenGLExampleBrowserInternalData;

}

OpenGLExampleBrowser::~OpenGLExampleBrowser()
{
	deleteDemo(m_data);
	for (int i = 0; i < m_data->m_nodes.size(); i++)
	{
		delete m_data->m_nodes[i];
	}
	delete m_data->m_handler2;
	for (int i = 0; i < m_data->m_handlers.size(); i++)
	{
		delete m_data->m_handlers[i];
	}
	m_data->m_handlers.clear();
	m_data->m_nodes.clear();
	delete SimpleOpenGL3App::instance()->m_parameterInterface;
	SimpleOpenGL3App::instance()->m_parameterInterface = 0;
	delete SimpleOpenGL3App::instance()->m_2dCanvasInterface;
	SimpleOpenGL3App::instance()->m_2dCanvasInterface = 0;

#ifndef BT_NO_PROFILE
	//destroyProfileWindow(internalData->m_profWindow);
#endif

    GwenUserInterface::instance()->exit();

	delete GwenUserInterface::instance();
	delete m_data->m_gwenRenderer;
	delete m_data->m_myTexLoader;
     
    m_data->gFileImporterByExtension.clear();
    delete SimpleOpenGL3App::instance();
	delete m_data;


 
}

#include "Component/EmptyExample.h"

bool OpenGLExampleBrowser::init(int argc, char* argv[])
{
	b3CommandLineArgs args(argc, argv);

	loadCurrentSettings(m_data->startFileName, args);
	if (args.CheckCmdLineFlag("nogui"))
	{
        m_data->renderGrid = false;
        m_data->renderGui = false;
	}
 
	args.GetCmdLineArgument("fixed_timestep", m_data->gFixedTimeStep);
	args.GetCmdLineArgument("png_skip_frames", m_data->gPngSkipFrames);
 
	if (args.CheckCmdLineFlag("disable_retina"))
	{
        m_data->gAllowRetina = false;
	}

	int width = 1024;
	int height = 768;

	if (args.CheckCmdLineFlag("width"))
	{
		args.GetCmdLineArgument("width", width);
	}
	if (args.CheckCmdLineFlag("height"))
	{
		args.GetCmdLineArgument("height", height);
	}

#if 1
 
	args.GetCmdLineArgument("render_device", m_data->gRenderDevice);
	args.GetCmdLineArgument("window_backend", m_data->gWindowBackend);
  
#endif
	const char* appTitle = "Bullet Physics ExampleBrowser";
#if defined(_DEBUG) || defined(DEBUG)
	const char* optMode = "Debug build (slow)";
#else
	const char* optMode = "Release build";
#endif

 
	const char* glContext = "[btgl]";
 
 
		char title[1024];
		sprintf(title, "%s using OpenGL3+ %s %s", appTitle, glContext, optMode);
        SimpleOpenGL3App*  s_app = new SimpleOpenGL3App(title, width, height, m_data->gAllowRetina, m_data->gWindowBackend, m_data->gRenderDevice);
		 
	 
 
	//internalData->m_app = SimpleOpenGL3App::instance();
	char* gVideoFileName = 0;
	args.GetCmdLineArgument("mp4", gVideoFileName);
#if 1
	if (gVideoFileName)
        s_app->dumpFramesToVideo(gVideoFileName);
#endif

 
	width = s_app->m_window->getWidth();
	height = s_app->m_window->getHeight();

	prevMouseMoveCallback = s_app->m_window->getMouseMoveCallback();
	s_app->m_window->setMouseMoveCallback(MyMouseMoveCallback);

	prevMouseButtonCallback = s_app->m_window->getMouseButtonCallback();
	s_app->m_window->setMouseButtonCallback(MyMouseButtonCallback);
	prevKeyboardCallback = s_app->m_window->getKeyboardCallback();
	s_app->m_window->setKeyboardCallback(MyKeyboardCallback);

	s_app->m_renderer->getActiveCamera()->setCameraDistance(13);
	s_app->m_renderer->getActiveCamera()->setCameraPitch(0);
	s_app->m_renderer->getActiveCamera()->setCameraTargetPosition(0, 0, 0);

	float mouseMoveMult = s_app->getMouseMoveMultiplier();
	if (args.GetCmdLineArgument("mouse_move_multiplier", mouseMoveMult))
	{
		s_app->setMouseMoveMultiplier(mouseMoveMult);
	}

	float mouseWheelMult = s_app->getMouseWheelMultiplier();
	if (args.GetCmdLineArgument("mouse_wheel_multiplier", mouseWheelMult))
	{
		s_app->setMouseWheelMultiplier(mouseWheelMult);
	}
     
	float red, green, blue;
	s_app->getBackgroundColor(&red, &green, &blue);
	args.GetCmdLineArgument("background_color_red", red);
	args.GetCmdLineArgument("background_color_green", green);
	args.GetCmdLineArgument("background_color_blue", blue);
	s_app->setBackgroundColor(red, green, blue);

	//b3SetCustomWarningMessageFunc(MyGuiPrintf);
	//b3SetCustomPrintfFunc(MyGuiPrintf);
	//b3SetCustomErrorMessageFunc(MyStatusBarError);

	assert(glGetError() == GL_NO_ERROR);

 
		GL3TexLoader* myTexLoader = new GL3TexLoader;
		m_data->m_myTexLoader = myTexLoader;

 
#if 1
	 
		{
			sth_stash* fontstash = s_app->getFontStash();
            Gwk::Platform::RelativeToExecutablePaths paths("");
			m_data->m_gwenRenderer = new GwenOpenGL3CoreRenderer(paths, s_app->m_primRenderer, fontstash, width, height, s_app->m_window->getRetinaScale(), myTexLoader);
		}
#endif

        GwenUserInterface* gui2 = new GwenUserInterface;

		//internalData->m_gui = gui2;

		m_data->m_myTexLoader = myTexLoader;

		gui2->init(width, height, m_data->m_gwenRenderer, s_app->m_window->getRetinaScale());
 

 
	{
		//	gui->getInternalData()->m_explorerPage
		Gwk::Controls::TreeControl* tree = gui2->getInternalData()->m_explorerTreeCtrl;

		//gui->getInternalData()->pRenderer->setTextureLoader(myTexLoader);

#ifndef BT_NO_PROFILE
		//s_profWindow = setupProfileWindow(gui2->getInternalData());
		//internalData->m_profWindow = s_profWindow;
		//profileWindowSetVisible(s_profWindow, false);
#endif  //BT_NO_PROFILE
		gui2->setFocus();

		s_app->m_parameterInterface = s_app->m_parameterInterface = new GwenParameterInterface(gui2->getInternalData());
		s_app->m_2dCanvasInterface = new QuickCanvas(myTexLoader);

		///add some demos to the gAllExamples

		int numDemos = ExampleEntriesAll::instance()->getNumRegisteredExamples();

		//char nodeText[1024];
		//int curDemo = 0;
		int selectedDemo = 0;
		Gwk::Controls::TreeNode* curNode = tree;
		m_data->m_handler2 = new MyMenuItemHander(-1);

		char* demoNameFromCommandOption = 0;
		args.GetCmdLineArgument("start_demo_name", demoNameFromCommandOption);
		if (demoNameFromCommandOption)
		{
			selectedDemo = -1;
		}

		tree->onReturnKeyDown.Add(m_data->m_handler2, &MyMenuItemHander::onButtonD);
		int firstAvailableDemoIndex = -1;
		Gwk::Controls::TreeNode* firstNode = 0;

		for (int d = 0; d < numDemos; d++)
		{
			//		sprintf(nodeText, "Node %d", i);
            Gwk::String nodeUText = (ExampleEntriesAll::instance()->getExampleName(d));
			if (ExampleEntriesAll::instance()->getExampleCreateFunc(d))  //was test for gAllExamples[d].m_menuLevel==1
			{
				Gwk::Controls::TreeNode* pNode = curNode->AddNode(nodeUText);

				if (firstAvailableDemoIndex < 0)
				{
					firstAvailableDemoIndex = d;
					firstNode = pNode;
				}

				if (d == selectedDemo)
				{
					firstAvailableDemoIndex = d;
					firstNode = pNode;
					//pNode->SetSelected(true);
					//tree->ExpandAll();
					//	tree->ForceUpdateScrollBars();
					//tree->OnKeyLeft(true);
					//	tree->OnKeyRight(true);

					//tree->ExpandAll();

					//	selectDemo(internalData, d);
				}

				if (demoNameFromCommandOption)
				{
					const char* demoName = ExampleEntriesAll::instance()->getExampleName(d);
					int res = strcmp(demoName, demoNameFromCommandOption);
					if (res == 0)
					{
						firstAvailableDemoIndex = d;
						firstNode = pNode;
					}
				}

#if 1
				MyMenuItemHander* handler = new MyMenuItemHander(d);
				m_data->m_handlers.push_back(handler);

				pNode->onNamePress.Add(handler, &MyMenuItemHander::onButtonA);
				pNode->GetButton()->onDoubleClick.Add(handler, &MyMenuItemHander::onButtonB);
				pNode->GetButton()->onDown.Add(handler, &MyMenuItemHander::onButtonC);
				pNode->onSelect.Add(handler, &MyMenuItemHander::onButtonE);
				pNode->onReturnKeyDown.Add(handler, &MyMenuItemHander::onButtonG);
				pNode->onSelectChange.Add(handler, &MyMenuItemHander::onButtonF);

#endif
				//			pNode->onKeyReturn.Add(handler, &MyMenuItemHander::onButtonD);
				//			pNode->GetButton()->onKeyboardReturn.Add(handler, &MyMenuItemHander::onButtonD);
				//		pNode->onNamePress.Add(handler, &MyMenuItemHander::onButtonD);
				//			pNode->onKeyboardPressed.Add(handler, &MyMenuItemHander::onButtonD);
				//			pNode->OnKeyPress
			}
			else
			{
				curNode = tree->AddNode(nodeUText);
				m_data->m_nodes.push_back(curNode);
			}
		}

		if (m_data->sCurrentDemo == 0)
		{
			if (firstAvailableDemoIndex >= 0)
			{
				firstNode->SetSelected(true);
				while (firstNode != tree)
				{
					firstNode->ExpandAll();
					firstNode = (Gwk::Controls::TreeNode*)firstNode->GetParent();
				}
                m_data->sCurrentHightlighted = firstAvailableDemoIndex;
				selectDemo(m_data);
			}
		}
		free(demoNameFromCommandOption);
		demoNameFromCommandOption = 0;

		b3Assert(m_data->sCurrentDemo != 0);
		if (m_data->sCurrentDemo == 0)
		{
			printf("Error, no demo/example\n");
			exit(0);
		}

		gui2->registerFileOpenCallback(fileOpenCallback);
		gui2->registerQuitCallback(quitCallback);
	}

	return true;
}

CommonExampleInterface* OpenGLExampleBrowser::getCurrentExample()
{
	b3Assert(m_data->sCurrentDemo);
	return m_data->sCurrentDemo;
}

bool OpenGLExampleBrowser::requestedExit()
{
	return SimpleOpenGL3App::instance()->m_window->requestedExit();
}

void OpenGLExampleBrowser::updateGraphics()
{
	if (m_data->sCurrentDemo)
	{
		if (!m_data->pauseSimulation || m_data->singleStepSimulation)
		{
			//B3_PROFILE("internalData->sCurrentDemo->updateGraphics");
			m_data->sCurrentDemo->updateGraphics();
		}
	}
}

void OpenGLExampleBrowser::update(float deltaTime)
{
    SimpleOpenGL3App*  s_app = SimpleOpenGL3App::instance();
    GwenUserInterface*  gui2 = GwenUserInterface::instance();
	//b3ChromeUtilsEnableProfiling();

	if (!m_data->gEnableRenderLoop && !m_data->singleStepSimulation)
	{
		B3_PROFILE("updateGraphics");
		m_data->sCurrentDemo->updateGraphics();
		return;
	}

	B3_PROFILE("OpenGLExampleBrowser::update");
	//assert(glGetError() == GL_NO_ERROR);
	{
		B3_PROFILE("s_app->m_renderer");
		s_app->m_renderer->init();
	}
	DrawGridData dg;
	dg.upAxis = s_app->getUpAxis();

	{
		B3_PROFILE("Update Camera and Light");

		s_app->m_renderer->updateCamera(dg.upAxis);
	}

	static int frameCount = 0;
	frameCount++;
  
	if (m_data->gPngFileName)
	{
		static int skip = 0;
		skip--;
		if (skip < 0)
		{
			skip = m_data->gPngSkipFrames;
			//printf("gPngFileName=%s\n",internalData->gPngFileName);
			static int s_frameCount = 0;

			sprintf(staticPngFileName, "%s%d.png", m_data->gPngFileName, s_frameCount++);
			//b3Printf("Made screenshot %s",staticPngFileName);
			s_app->dumpNextFrameToPng(staticPngFileName);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	if (m_data->sCurrentDemo)
	{
		if (!m_data->pauseSimulation || m_data->singleStepSimulation)
		{
			//printf("---------------------------------------------------\n");
			//printf("Framecount = %d\n",frameCount);
			B3_PROFILE("internalData->sCurrentDemo->stepSimulation");

			if (m_data->gFixedTimeStep > 0)
			{
				
				m_data->sCurrentDemo->stepSimulation(m_data->gFixedTimeStep);
			}
			else
			{
				m_data->sCurrentDemo->stepSimulation(deltaTime);  //1./60.f);
			}
		}
       
		if (m_data->renderGrid)
		{
			B3_PROFILE("Draw Grid");
			//glPolygonOffset(3.0, 3);
			//glEnable(GL_POLYGON_OFFSET_FILL);
			s_app->drawGrid(dg);
		}
		if (m_data->renderVisualGeometry && ((m_data->gDebugDrawFlags & btIDebugDraw::DBG_DrawWireframe) == 0))
		{
			if (m_data->visualWireframe)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			B3_PROFILE("Render Scene");
			m_data->sCurrentDemo->renderScene();
		}
		else
		{
			B3_PROFILE("physicsDebugDraw");
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_data->sCurrentDemo->physicsDebugDraw(m_data->gDebugDrawFlags);
		}
	}

	{
		if (gui2 && m_data->guiHelper && m_data->guiHelper->getRenderInterface() && m_data->guiHelper->getRenderInterface()->getActiveCamera())
		{
			B3_PROFILE("setStatusBarMessage");
			char msg[1024];
			float camDist = m_data->guiHelper->getRenderInterface()->getActiveCamera()->getCameraDistance();
			float pitch = m_data->guiHelper->getRenderInterface()->getActiveCamera()->getCameraPitch();
			float yaw = m_data->guiHelper->getRenderInterface()->getActiveCamera()->getCameraYaw();
			float camTarget[3];
			float camPos[3];
			m_data->guiHelper->getRenderInterface()->getActiveCamera()->getCameraPosition(camPos);
			m_data->guiHelper->getRenderInterface()->getActiveCamera()->getCameraTargetPosition(camTarget);
			sprintf(msg, "camTargetPos=%2.2f,%2.2f,%2.2f, dist=%2.2f, pitch=%2.2f, yaw=%2.2f", camTarget[0], camTarget[1], camTarget[2], camDist, pitch, yaw);
            gui2->setStatusBarMessage(msg, true);
		}
	}

	static int toggle = 1;
	if (m_data->renderGui)
	{
		B3_PROFILE("renderGui");
#ifndef BT_NO_PROFILE

		if (!m_data->pauseSimulation || m_data->singleStepSimulation)
		{
			//if (isProfileWindowVisible(s_profWindow))
			//{
			//	processProfileData(s_profWindow, false);
			//}
		}
#endif  //#ifndef BT_NO_PROFILE

		{
			B3_PROFILE("updateOpenGL");
	 

			if (gui2)
			{
                m_data->gBlockGuiMessages = true;
                gui2->draw(s_app->m_renderer->getScreenWidth(), s_app->m_renderer->getScreenHeight());

                m_data->gBlockGuiMessages = false;
			}

		 
		}
	}

    m_data->singleStepSimulation = false;

	toggle = 1 - toggle;
	{
		B3_PROFILE("Sync Parameters");
		if (s_app->m_parameterInterface)
		{
			s_app->m_parameterInterface->syncParameters();
		}
	}
	{
		B3_PROFILE("Swap Buffers");
		s_app->swapBuffer();
	}

	if (gui2)
	{
		B3_PROFILE("forceUpdateScrollBars");
        gui2->forceUpdateScrollBars();
	}
}
 
