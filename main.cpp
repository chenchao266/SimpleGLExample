#include "OpenGLWindow/SimpleOpenGL3App.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3CommandLineArgs.h"
#include "Bullet3Common/b3Transform.h"

#include "assert.h"
#include <stdio.h>
#include "Component/b3Clock.h"
#include "Component/TinyRendererGUIHelper.h"
#include "Component/SW_And_OpenGLGuiHelper.h"
#include "OpenGLWindow/GwenParameterInterface.h"
#include "OpenGLWindow/gwenInternalData.h"
#include "OpenGLWindow/btIDebugDraw.h"
#include "OpenGLExampleBrowser.h"
#include "ExampleEntries.h"
 
char* gVideoFileName = 0;
char* gPngFileName = 0;
 
static b3WheelCallback sOldWheelCB = 0;
static b3ResizeCallback sOldResizeCB = 0;
static b3MouseMoveCallback sOldMouseMoveCB = 0;
static b3MouseButtonCallback sOldMouseButtonCB = 0;
static b3KeyboardCallback sOldKeyboardCB = 0;

CommonExampleInterface* example;
static CommonWindowInterface* s_window = 0;
  
float gWidth = 0;
float gHeight = 0;

void MyWheelCallback(float deltax, float deltay)
{
	if (sOldWheelCB)
		sOldWheelCB(deltax, deltay);
}
void MyResizeCallback(float width, float height)
{
	gWidth = width;
	gHeight = height;

	if (sOldResizeCB)
		sOldResizeCB(width, height);
}
 
extern void MyMouseMoveCallback(float x, float y);
extern void MyMouseButtonCallback(int button, int state, float x, float y);

extern void MyKeyboardCallback(int key, int state);
 
int main1(int argc, char* argv[])
{
	b3CommandLineArgs myArgs(argc, argv);

	SimpleOpenGL3App* app = new SimpleOpenGL3App("SimpleOpenGL3App", 640, 480, true);
    GwenInternalData data;
    app->m_parameterInterface = new GwenParameterInterface(&data);
    s_window = app->m_window;
	app->m_instancingRenderer->getActiveCamera()->setCameraDistance(13);
	app->m_instancingRenderer->getActiveCamera()->setCameraPitch(0);
	app->m_instancingRenderer->getActiveCamera()->setCameraTargetPosition(0, 0, 0);
	sOldKeyboardCB = app->m_window->getKeyboardCallback();
	app->m_window->setKeyboardCallback(MyKeyboardCallback);
	sOldMouseMoveCB = app->m_window->getMouseMoveCallback();
	app->m_window->setMouseMoveCallback(MyMouseMoveCallback);
	sOldMouseButtonCB = app->m_window->getMouseButtonCallback();
	app->m_window->setMouseButtonCallback(MyMouseButtonCallback);
	sOldWheelCB = app->m_window->getWheelCallback();
	app->m_window->setWheelCallback(MyWheelCallback);
	sOldResizeCB = app->m_window->getResizeCallback();
	app->m_window->setResizeCallback(MyResizeCallback);

	int textureWidth = gWidth;
	int textureHeight = gHeight;
	TGAImage rgbColorBuffer(gWidth, gHeight, TGAImage::RGB);
	b3AlignedObjectArray<float> depthBuffer;
	depthBuffer.resize(gWidth * gHeight);

	TinyRenderObjectData renderData(rgbColorBuffer, depthBuffer);  //, "african_head/african_head.obj");//floor.obj");

	//renderData.loadModel("african_head/african_head.obj");
	renderData.loadModel("floor.obj");

	//renderData.createCube(1,1,1);

	myArgs.GetCmdLineArgument("mp4_file", gVideoFileName);
	if (gVideoFileName)
		app->dumpFramesToVideo(gVideoFileName);

	myArgs.GetCmdLineArgument("png_file", gPngFileName);
	char fileName[1024];

	unsigned char* image = new unsigned char[textureWidth * textureHeight * 4];

	int textureHandle = app->m_renderer->registerTexture(image, textureWidth, textureHeight);

	int cubeIndex = app->registerCubeShape(1, 1, 1);

	b3Vector3 pos = b3MakeVector3(0, 0, 0);
	b3Quaternion orn(0, 0, 0, 1);
	b3Vector3 color = b3MakeVector3(1, 0, 0);
	b3Vector3 scaling = b3MakeVector3(1, 1, 1);
	app->m_renderer->registerGraphicsInstance(cubeIndex, pos, orn, color, scaling);
	app->m_renderer->writeTransforms();

	do
	{
		static int frameCount = 0;
		frameCount++;
		if (gPngFileName)
		{
			printf("gPngFileName=%s\n", gPngFileName);

			sprintf(fileName, "%s%d.png", gPngFileName, frameCount++);
			app->dumpNextFrameToPng(fileName);
		}

		app->m_instancingRenderer->init();
		app->m_instancingRenderer->updateCamera();

		///clear the color and z (depth) buffer
		for (int y = 0; y < textureHeight; ++y)
		{
			unsigned char* pi = image + (y)*textureWidth * 3;
			for (int x = 0; x < textureWidth; ++x)
			{
				Colorb color;
				color.c[0] = 255;
				color.c[1] = 255;
				color.c[2] = 255;
				color.c[3] = 255;

				renderData.m_rgbColorBuffer.set(x, y, color);
				renderData.m_depthBuffer[x + y * textureWidth] = -1e30f;
			}
		}

		float projMat[16];
		app->m_instancingRenderer->getActiveCamera()->getCameraProjectionMatrix(projMat);
		float viewMat[16];
		app->m_instancingRenderer->getActiveCamera()->getCameraViewMatrix(viewMat);
		B3_ATTRIBUTE_ALIGNED16(float modelMat[16]);

		//sync the object transform
		b3Transform tr;
		tr.setIdentity();
		static float posUp = 0.f;
		// posUp += 0.001;
		b3Vector3 org = b3MakeVector3(0, posUp, 0);
		tr.setOrigin(org);
		tr.getOpenGLMatrix(modelMat);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				renderData.m_viewMatrix[i][j] = viewMat[i + 4 * j];
				renderData.m_modelMatrix[i][j] = modelMat[i + 4 * j];
			}
		}

		//render the object
		TinyRenderer::renderObject(renderData);

#if 1
		//update the texels of the texture using a simple pattern, animated using frame index
		for (int y = 0; y < textureHeight; ++y)
		{
			unsigned char* pi = image + (y)*textureWidth * 3;
			for (int x = 0; x < textureWidth; ++x)
			{
				Colorb color = renderData.m_rgbColorBuffer.get(x, y);
				pi[0] = color.c[2];
				pi[1] = color.c[1];
				pi[2] = color.c[0];
				pi[3] = 255;
				pi += 3;
			}
		}
#else

		//update the texels of the texture using a simple pattern, animated using frame index
		for (int y = 0; y < textureHeight; ++y)
		{
			const int t = (y + frameCount) >> 4;
			unsigned char* pi = image + y * textureWidth * 3;
			for (int x = 0; x < textureWidth; ++x)
			{
				Colorb color = renderData.m_rgbColorBuffer.get(x, y);

				const int s = x >> 4;
				const unsigned char b = 180;
				unsigned char c = b + ((s + (t & 1)) & 1) * (255 - b);
				pi[0] = pi[1] = pi[2] = pi[3] = c;
				pi += 3;
			}
		}
#endif

		app->m_renderer->activateTexture(textureHandle);
		app->m_renderer->updateTexture(textureHandle, image);

        Colorf color(1, 1, 1, 1);
		app->m_primRenderer->drawTexturedRect(0, 0, gWidth / 3, gHeight / 3, color, 0, 0, 1, 1, true);

		app->m_renderer->renderScene();
		app->drawGrid();
		char bla[1024];
		sprintf(bla, "Simple test frame %d", frameCount);

		app->drawText(bla, 10, 10, 8, color);
		app->swapBuffer();
	} while (!app->m_window->requestedExit());

	delete app;
	return 0;
}
 
static double gMinUpdateTimeMicroSecs = 1000.;

static bool interrupted = false;
static OpenGLExampleBrowser* sExampleBrowser = 0;

int main(int argc, char* argv[])
{

    b3CommandLineArgs args(argc, argv);
    b3Clock clock;
    args.GetCmdLineArgument("minUpdateTimeMicroSecs", gMinUpdateTimeMicroSecs);

    ExampleEntriesAll* examples = new ExampleEntriesAll;
    examples->initExampleEntries();

    OpenGLExampleBrowser* exampleBrowser = new OpenGLExampleBrowser();
    sExampleBrowser = exampleBrowser;  //for <CTRL-C> etc, cleanup shared memory
    bool init = exampleBrowser->init(argc, argv);
    //exampleBrowser->registerFileImporter(".urdf", ImportURDFCreateFunc);
    //exampleBrowser->registerFileImporter(".sdf", ImportSDFCreateFunc);
    //exampleBrowser->registerFileImporter(".obj", ImportObjCreateFunc);
    //exampleBrowser->registerFileImporter(".stl", ImportSTLCreateFunc);
    //exampleBrowser->registerFileImporter(".bullet", SerializeBulletCreateFunc);

    clock.reset();
    if (init)
    {
        do
        {
            float deltaTimeInSeconds = clock.getTimeMicroseconds() / 1000000.f;
            if (deltaTimeInSeconds > 0.1)
            {
                deltaTimeInSeconds = 0.1;
            }
            if (deltaTimeInSeconds < (gMinUpdateTimeMicroSecs / 1e6))
            {
                b3Clock::usleep(gMinUpdateTimeMicroSecs / 10.);
            }
            else
            {
                clock.reset();
                exampleBrowser->update(deltaTimeInSeconds);
            }
        } while (!exampleBrowser->requestedExit() && !interrupted);
    }
    delete exampleBrowser;
    delete examples;
}
 