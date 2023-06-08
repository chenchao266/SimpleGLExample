
#include "TimeSeriesExample.h"

#include "CommonInterfaces/CommonGraphicsAppInterface.h"
#include "CommonInterfaces/CommonRenderInterface.h"

#include "CommonInterfaces/Common2dCanvasInterface.h"
#include "CommonInterfaces/CommonExampleInterface.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
//#include "btBulletCollisionCommon.h"
#include "CommonInterfaces/CommonGUIHelperInterface.h"

#include "Component/TimeSeriesCanvas.h"
struct TimeSeriesExampleInternalData;
struct TimeSeriesExample : public CommonExampleInterface
{
	  CommonGraphicsApp* m_app;
	  TimeSeriesExampleInternalData* m_data;

	TimeSeriesExample(  CommonGraphicsApp* app);

	virtual ~TimeSeriesExample();

	virtual void initPhysics();

	virtual void exitPhysics();

	virtual void stepSimulation(float deltaTime);

	virtual void physicsDebugDraw(int debugFlags);

	virtual void syncPhysicsToGraphics(const btDynamicsWorld* gfxBridge);

	virtual bool mouseMoveCallback(float x, float y);

	virtual bool mouseButtonCallback(int button, int state, float x, float y);

	virtual bool keyboardCallback(int key, int state);

	virtual void renderScene()
	{
	}
};

struct TimeSeriesExampleInternalData
{
	TimeSeriesCanvas* m_timeSeriesCanvas;

	TimeSeriesExampleInternalData()
		: m_timeSeriesCanvas(0)
	{
	}
};

TimeSeriesExample::TimeSeriesExample(  CommonGraphicsApp* app)
{
	m_app = app;
	m_data = new TimeSeriesExampleInternalData;
}

TimeSeriesExample::~TimeSeriesExample()
{
	delete m_data->m_timeSeriesCanvas;
	delete m_data;
}

void TimeSeriesExample::initPhysics()
{
	//request a visual bitma/texture we can render to

	m_data->m_timeSeriesCanvas = new TimeSeriesCanvas(m_app->m_2dCanvasInterface, 512, 512, "Test");
	m_data->m_timeSeriesCanvas->setupTimeSeries(3, 100, 0);
	m_data->m_timeSeriesCanvas->addDataSource("Some sine wave", Colorb(255, 0, 0, 255));
	m_data->m_timeSeriesCanvas->addDataSource("Some cosine wave", Colorb(0, 255, 0, 255));
	m_data->m_timeSeriesCanvas->addDataSource("Delta Time (*10)", Colorb(0, 0, 255, 255));
	m_data->m_timeSeriesCanvas->addDataSource("Tan", Colorb(255, 0, 255, 255));
	m_data->m_timeSeriesCanvas->addDataSource("Some cosine wave2", Colorb(255, 255, 0, 255));
	m_data->m_timeSeriesCanvas->addDataSource("Empty source2", Colorb(255, 0, 255, 255));
}

void TimeSeriesExample::exitPhysics()
{
}

void TimeSeriesExample::stepSimulation(float deltaTime)
{
	float time = m_data->m_timeSeriesCanvas->getCurrentTime();
	float v = sinf(time);
	m_data->m_timeSeriesCanvas->insertDataAtCurrentTime(v, 0, true);
	v = cosf(time);
	m_data->m_timeSeriesCanvas->insertDataAtCurrentTime(v, 1, true);
	v = tanf(time);
	m_data->m_timeSeriesCanvas->insertDataAtCurrentTime(v, 3, true);
	m_data->m_timeSeriesCanvas->insertDataAtCurrentTime(deltaTime * 10, 2, true);

	m_data->m_timeSeriesCanvas->nextTick();
}

void TimeSeriesExample::physicsDebugDraw(int debugDrawFlags)
{
}

bool TimeSeriesExample::mouseMoveCallback(float x, float y)
{
	return false;
}

bool TimeSeriesExample::mouseButtonCallback(int button, int state, float x, float y)
{
	return false;
}

bool TimeSeriesExample::keyboardCallback(int key, int state)
{
	return false;
}

void TimeSeriesExample::syncPhysicsToGraphics(const btDynamicsWorld* gfxBridge)
{
}

CommonExampleInterface* TimeSeriesCreateFunc(  CommonExampleOptions& options)
{
	return new TimeSeriesExample(options.m_guiHelper->getAppInterface());
}
