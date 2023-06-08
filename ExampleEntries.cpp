#include "ExampleEntries.h"

#include "Bullet3Common/b3AlignedObjectArray.h"
#include "Component\EmptyExample.h"
#include "TinyRendererSetup.h"
#include "TimeSeriesExample.h"
#include "RenderInstancingDemo.h"
#include "DynamicTexturedCubeDemo.h"

struct ExampleEntry
{
	int m_menuLevel;
	const char* m_name;
	const char* m_description;
	CommonExampleInterface::CreateFunc* m_createFunc;
	int m_option;

	ExampleEntry(int menuLevel, const char* name)
		: m_menuLevel(menuLevel), m_name(name), m_description(0), m_createFunc(0), m_option(0)
	{
	}

	ExampleEntry(int menuLevel, const char* name, const char* description, CommonExampleInterface::CreateFunc* createFunc, int option = 0)
		: m_menuLevel(menuLevel), m_name(name), m_description(description), m_createFunc(createFunc), m_option(option)
	{
	}
};

static ExampleEntry gDefaultExamples[] =
	{
 
		ExampleEntry(0, "Rendering"),
		ExampleEntry(1, "Instanced Rendering", "Simple example of fast instanced rendering, only active when using OpenGL3+.", RenderInstancingCreateFunc),
		ExampleEntry(1, "TinyRenderer", "Very small software renderer.", TinyRendererCreateFunc),
		ExampleEntry(1, "TimeSeries", "Time Series .", TimeSeriesCreateFunc),
		ExampleEntry(1, "DynamicTexturedCube", "Dynamic Textured Cube .", DynamicTexturedCubeDemoCreateFunc),

};


static b3AlignedObjectArray<ExampleEntry> gAdditionalRegisteredExamples;

struct ExampleEntriesInternalData
{
	b3AlignedObjectArray<ExampleEntry> m_allExamples;
};

static ExampleEntriesAll* gExampleAll;
ExampleEntriesAll* ExampleEntriesAll::instance()
{
    return  gExampleAll;
}

ExampleEntriesAll::ExampleEntriesAll()
{
    gExampleAll = this;
	m_data = new ExampleEntriesInternalData;
}

ExampleEntriesAll::~ExampleEntriesAll()
{
	delete m_data;
}

void ExampleEntriesAll::initOpenCLExampleEntries()
{

}

void ExampleEntriesAll::initExampleEntries()
{
	m_data->m_allExamples.clear();

	for (int i = 0; i < gAdditionalRegisteredExamples.size(); i++)
	{
		m_data->m_allExamples.push_back(gAdditionalRegisteredExamples[i]);
	}

	int numDefaultEntries = sizeof(gDefaultExamples) / sizeof(ExampleEntry);
	for (int i = 0; i < numDefaultEntries; i++)
	{
		m_data->m_allExamples.push_back(gDefaultExamples[i]);
	}

	if (m_data->m_allExamples.size() == 0)
	{
		{
			ExampleEntry e(0, "Empty");
			m_data->m_allExamples.push_back(e);
		}

		{
			ExampleEntry e(1, "Empty", "Empty Description", EmptyExample::CreateFunc);
			m_data->m_allExamples.push_back(e);
		}
	}
}

void ExampleEntriesAll::registerExampleEntry(int menuLevel, const char* name, const char* description, CommonExampleInterface::CreateFunc* createFunc, int option)
{
	ExampleEntry e(menuLevel, name, description, createFunc, option);
	gAdditionalRegisteredExamples.push_back(e);
}

int ExampleEntriesAll::getNumRegisteredExamples()
{
	return m_data->m_allExamples.size();
}

CommonExampleInterface::CreateFunc* ExampleEntriesAll::getExampleCreateFunc(int index)
{
	return m_data->m_allExamples[index].m_createFunc;
}

int ExampleEntriesAll::getExampleOption(int index)
{
	return m_data->m_allExamples[index].m_option;
}

const char* ExampleEntriesAll::getExampleName(int index)
{
	return m_data->m_allExamples[index].m_name;
}

const char* ExampleEntriesAll::getExampleDescription(int index)
{
	return m_data->m_allExamples[index].m_description;
}
