
#include "TinyRendererSetup.h"

#include "CommonInterfaces/CommonGraphicsAppInterface.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "CommonInterfaces/CommonRenderInterface.h"
#include "OpenGLWindow/TinyRenderer.h"
#include "CommonInterfaces/Common2dCanvasInterface.h"
 
#include "CommonInterfaces/CommonExampleInterface.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
//#include "btBulletCollisionCommon.h"
#include "CommonInterfaces/CommonGUIHelperInterface.h"
#include "Bullet3Common/CollisionShape2TriangleMesh.h"
#include "Component/b3ImportMeshUtility.h"
#include "OpenGLWindow/GLInstanceGraphicsShape.h"
#include "CommonInterfaces/CommonParameterInterface.h"
#include "Component/b3BulletDefaultFileIO.h"
struct TinyRendererSetupInternalData
{
	TGAImage m_rgbColorBuffer;
	b3AlignedObjectArray<float> m_depthBuffer;
	b3AlignedObjectArray<float> m_shadowBuffer;
	b3AlignedObjectArray<int> m_segmentationMaskBuffer;

	int m_width;
	int m_height;

	b3AlignedObjectArray<btCollisionShape*> m_shapePtr;
	b3AlignedObjectArray<b3Transform> m_transforms;
	b3AlignedObjectArray<TinyRenderObjectData*> m_renderObjects;

	//btVoronoiSimplexSolver m_simplexSolver;
	b3Scalar m_pitch;
	b3Scalar m_roll;
	b3Scalar m_yaw;

	int m_textureHandle;
	int m_animateRenderer;

	b3Vector3 m_lightPos;

	TinyRendererSetupInternalData(int width, int height)
		: m_rgbColorBuffer(width, height, TGAImage::RGB),
		  m_width(width),
		  m_height(height),
		  m_pitch(0),
		  m_roll(0),
		  m_yaw(0),
		  m_textureHandle(0),
		  m_animateRenderer(0)
	{
		m_lightPos.setValue(-3, 15, 15);

		m_depthBuffer.resize(m_width * m_height);
		m_shadowBuffer.resize(m_width * m_height);
		//        m_segmentationMaskBuffer.resize(m_width*m_height);
	}
	void updateTransforms()
	{
		int numObjects = m_shapePtr.size();
		m_transforms.resize(numObjects);
		for (int i = 0; i < numObjects; i++)
		{
			m_transforms[i].setIdentity();
			//b3Vector3	pos(0.f,-(2.5* numObjects * 0.5)+i*2.5f, 0.f);
			b3Vector3 pos(0.f, +i * 2.5f, 0.f);
			m_transforms[i].setIdentity();
			m_transforms[i].setOrigin(pos);
			b3Quaternion orn;
			if (i < 2)
			{
				orn.setEuler(m_yaw, m_pitch, m_roll);
				m_transforms[i].setRotation(orn);
			}
		}
		if (m_animateRenderer)
		{
			m_pitch += 0.005f;
			m_yaw += 0.01f;
		}
	}
};

struct TinyRendererSetup : public CommonExampleInterface
{
	 GUIHelperInterface* m_guiHelper;
	 CommonGraphicsApp* m_app;
	 TinyRendererSetupInternalData* m_data;
	bool m_useSoftware;

	TinyRendererSetup(  GUIHelperInterface* guiHelper);

	virtual ~TinyRendererSetup();

	virtual void initPhysics();

	virtual void exitPhysics();

	virtual void stepSimulation(float deltaTime);

	virtual void physicsDebugDraw(int debugFlags);
     
	virtual bool mouseMoveCallback(float x, float y);

	virtual bool mouseButtonCallback(int button, int state, float x, float y);

	virtual bool keyboardCallback(int key, int state);

	virtual void renderScene();

	void animateRenderer(int animateRendererIndex)
	{
		m_data->m_animateRenderer = animateRendererIndex;
	}

	void selectRenderer(int rendererIndex)
	{
		m_useSoftware = (rendererIndex == 0);
	}

	void resetCamera()
	{
		float dist = 11;
		float pitch = -35;
		float yaw = 52;
		float targetPos[3] = {0, 0.46, 0};
		m_guiHelper->resetCamera(dist, yaw, pitch, targetPos[0], targetPos[1], targetPos[2]);
	}
};

TinyRendererSetup::TinyRendererSetup( GUIHelperInterface* gui)
{
	m_useSoftware = false;

	m_guiHelper = gui;
	m_app = gui->getAppInterface();
	m_data = new TinyRendererSetupInternalData(gui->getAppInterface()->m_window->getWidth(), gui->getAppInterface()->m_window->getHeight());

	const char* fileName = "textured_sphere_smooth.obj";
	fileName = "cube.obj";
	fileName = "torus/torus_with_plane.obj";

	{
		{
			int shapeId = -1;

			b3ImportMeshData meshData;
			b3BulletDefaultFileIO fileIO;
			if (b3ImportMeshUtility::loadAndRegisterMeshFromFileInternal(fileName, meshData,&fileIO))
			{
				int textureIndex = -1;

				if (meshData.m_texture)
				{
					textureIndex = m_guiHelper->getRenderInterface()->registerTexture(meshData.m_texture->buffer(), meshData.m_texture->get_width(), meshData.m_texture->get_height());
				}

				shapeId = m_guiHelper->getRenderInterface()->registerShape(&meshData.m_gfxShape->m_vertices->at(0).xyzw[0],
																		   meshData.m_gfxShape->m_numvertices,
																		   &meshData.m_gfxShape->m_indices->at(0),
																		   meshData.m_gfxShape->m_numIndices,
																		   B3_GL_TRIANGLES,
																		   textureIndex);

				float position[4] = {0, 0, 0, 1};
				float orn[4] = {0, 0, 0, 1};
                Colorf color(1, 1, 1, 1);
				float scaling[4] = {1, 1, 1, 1};

				m_guiHelper->getRenderInterface()->registerGraphicsInstance(shapeId, position, orn, color.c, scaling);
				m_guiHelper->getRenderInterface()->writeTransforms();

				m_data->m_shapePtr.push_back(0);
				TinyRenderObjectData* ob = new TinyRenderObjectData(
					m_data->m_rgbColorBuffer,
					m_data->m_depthBuffer,
					&m_data->m_shadowBuffer,
					&m_data->m_segmentationMaskBuffer,
					m_data->m_renderObjects.size(), -1);

				meshData.m_gfxShape->m_scaling[0] = scaling[0];
				meshData.m_gfxShape->m_scaling[1] = scaling[1];
				meshData.m_gfxShape->m_scaling[2] = scaling[2];

				const int* indices = &meshData.m_gfxShape->m_indices->at(0);
				ob->registerMeshShape(&meshData.m_gfxShape->m_vertices->at(0).xyzw[0],
									  meshData.m_gfxShape->m_numvertices,
									  indices,
									  meshData.m_gfxShape->m_numIndices, color, meshData.m_texture);

				ob->m_localScaling.setValue(scaling[0], scaling[1], scaling[2]);

				m_data->m_renderObjects.push_back(ob);

				delete meshData.m_gfxShape;
				if (!meshData.m_isCached)
				{
					delete meshData.m_texture;
				}
			}
		}
	}
}

TinyRendererSetup::~TinyRendererSetup()
{
	delete m_data;
}

const char* itemsanimate[] = {"Fixed", "Rotate"};
void TinyRendererComboCallbackAnimate(int combobox, const char* item, void* userPointer)
{
	TinyRendererSetup* cl = (TinyRendererSetup*)userPointer;
	b3Assert(cl);
	int index = -1;
	int numItems = sizeof(itemsanimate) / sizeof(char*);
	for (int i = 0; i < numItems; i++)
	{
		if (!strcmp(item, itemsanimate[i]))
		{
			index = i;
		}
	}
	cl->animateRenderer(index);
}

const char* items[] = {"Software", "OpenGL"};

void TinyRendererComboCallback(int combobox, const char* item, void* userPointer)
{
	TinyRendererSetup* cl = (TinyRendererSetup*)userPointer;
	b3Assert(cl);
	int index = -1;
	int numItems = sizeof(items) / sizeof(char*);
	for (int i = 0; i < numItems; i++)
	{
		if (!strcmp(item, items[i]))
		{
			index = i;
		}
	}
	cl->selectRenderer(index);
}

void TinyRendererSetup::initPhysics()
{
	//request a visual bitma/texture we can render to

	m_app->setUpAxis(2);

	CommonRenderInterface* render = m_app->m_renderer;

	m_data->m_textureHandle = render->registerTexture(m_data->m_rgbColorBuffer.buffer(), m_data->m_width, m_data->m_height);

	{
		ComboBoxParams comboParams;
		comboParams.m_userPointer = this;
		comboParams.m_numItems = sizeof(items) / sizeof(char*);
		comboParams.m_startItem = 1;
		comboParams.m_items = items;
		comboParams.m_callback = TinyRendererComboCallback;
		m_guiHelper->getParameterInterface()->registerComboBox(comboParams);
	}

	{
		ComboBoxParams comboParams;
		comboParams.m_userPointer = this;
		comboParams.m_numItems = sizeof(itemsanimate) / sizeof(char*);
		comboParams.m_startItem = 0;
		comboParams.m_items = itemsanimate;
		comboParams.m_callback = TinyRendererComboCallbackAnimate;
		m_guiHelper->getParameterInterface()->registerComboBox(comboParams);
	}

	{
		SliderParams slider("LightPosX", &m_data->m_lightPos[0]);
		slider.m_minVal = -10;
		slider.m_maxVal = 10;
		if (m_guiHelper->getParameterInterface())
			m_guiHelper->getParameterInterface()->registerSliderFloatParameter(slider);
	}
	{
		SliderParams slider("LightPosY", &m_data->m_lightPos[1]);
		slider.m_minVal = -10;
		slider.m_maxVal = 10;
		if (m_guiHelper->getParameterInterface())
			m_guiHelper->getParameterInterface()->registerSliderFloatParameter(slider);
	}
	{
		SliderParams slider("LightPosZ", &m_data->m_lightPos[2]);
		slider.m_minVal = -10;
		slider.m_maxVal = 10;
		if (m_guiHelper->getParameterInterface())
			m_guiHelper->getParameterInterface()->registerSliderFloatParameter(slider);
	}
}

void TinyRendererSetup::exitPhysics()
{
}

void TinyRendererSetup::stepSimulation(float deltaTime)
{
	m_data->updateTransforms();
}

void TinyRendererSetup::renderScene()
{
	m_data->updateTransforms();

	b3Vector4 from(m_data->m_lightPos[0], m_data->m_lightPos[1], m_data->m_lightPos[2], 1);
	b3Vector4 toX(m_data->m_lightPos[0] + 0.1, m_data->m_lightPos[1], m_data->m_lightPos[2], 1);
	b3Vector4 toY(m_data->m_lightPos[0], m_data->m_lightPos[1] + 0.1, m_data->m_lightPos[2], 1);
	b3Vector4 toZ(m_data->m_lightPos[0], m_data->m_lightPos[1], m_data->m_lightPos[2] + 0.1, 1);
    Colorf colorX(1, 0, 0, 1);
    Colorf colorY(0, 1, 0, 1);
    Colorf colorZ(0, 0, 1, 1);
	int width = 2;
	m_guiHelper->getRenderInterface()->drawLine(from, toX, colorX, width);
	m_guiHelper->getRenderInterface()->drawLine(from, toY, colorY, width);
	m_guiHelper->getRenderInterface()->drawLine(from, toZ, colorZ, width);

	if (!m_useSoftware)
	{
		b3Vector3 lightPos(m_data->m_lightPos[0], m_data->m_lightPos[1], m_data->m_lightPos[2]);
		m_guiHelper->getRenderInterface()->setLightPosition(lightPos);

		for (int i = 0; i < m_data->m_transforms.size(); i++)
		{
			m_guiHelper->getRenderInterface()->writeSingleInstanceTransformToCPU(m_data->m_transforms[i].getOrigin(), m_data->m_transforms[i].getRotation(), i);
		}
		m_guiHelper->getRenderInterface()->writeTransforms();
		m_guiHelper->getRenderInterface()->renderScene();
	}
	else
	{
		Colorb clearColor;
		clearColor.c[0] = 200;
		clearColor.c[1] = 200;
		clearColor.c[2] = 200;
		clearColor.c[3] = 255;
		for (int y = 0; y < m_data->m_height; ++y)
		{
			for (int x = 0; x < m_data->m_width; ++x)
			{
				m_data->m_rgbColorBuffer.set(x, y, clearColor);
				m_data->m_depthBuffer[x + y * m_data->m_width] = -1e30f;
				m_data->m_shadowBuffer[x + y * m_data->m_width] = -1e30f;
			}
		}

        B3_ATTRIBUTE_ALIGNED16(b3Scalar modelMat2[16]);
        B3_ATTRIBUTE_ALIGNED16(float viewMat[16]);
        B3_ATTRIBUTE_ALIGNED16(float projMat[16]);
		CommonRenderInterface* render = this->m_app->m_renderer;
		render->getActiveCamera()->getCameraViewMatrix(viewMat);
		render->getActiveCamera()->getCameraProjectionMatrix(projMat);

		for (int o = 0; o < this->m_data->m_renderObjects.size(); o++)
		{
			const b3Transform& tr = m_data->m_transforms[o];
			tr.getOpenGLMatrix(modelMat2);

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					m_data->m_renderObjects[o]->m_modelMatrix[i][j] = float(modelMat2[i + 4 * j]);
					m_data->m_renderObjects[o]->m_viewMatrix[i][j] = viewMat[i + 4 * j];
					m_data->m_renderObjects[o]->m_projectionMatrix[i][j] = projMat[i + 4 * j];

					b3Vector3 lightDirWorld = b3Vector3(m_data->m_lightPos[0], m_data->m_lightPos[1], m_data->m_lightPos[2]);

					m_data->m_renderObjects[o]->m_lightDirWorld = lightDirWorld.normalized();

					b3Vector3 lightColor(1.0, 1.0, 1.0);
					m_data->m_renderObjects[o]->m_lightColor = lightColor;

					m_data->m_renderObjects[o]->m_lightDistance = 10.0;
					m_data->m_renderObjects[o]->m_lightAmbientCoeff = 0.6;
					m_data->m_renderObjects[o]->m_lightDiffuseCoeff = 0.35;
					m_data->m_renderObjects[o]->m_lightSpecularCoeff = 0.05;
				}
			}
			TinyRenderer::renderObjectDepth(*m_data->m_renderObjects[o]);
		}

		for (int o = 0; o < this->m_data->m_renderObjects.size(); o++)
		{
			const b3Transform& tr = m_data->m_transforms[o];
			tr.getOpenGLMatrix(modelMat2);

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					m_data->m_renderObjects[o]->m_modelMatrix[i][j] = float(modelMat2[i + 4 * j]);
					m_data->m_renderObjects[o]->m_viewMatrix[i][j] = viewMat[i + 4 * j];
					m_data->m_renderObjects[o]->m_projectionMatrix[i][j] = projMat[i + 4 * j];

					b3Vector3 lightDirWorld = b3Vector3(m_data->m_lightPos[0], m_data->m_lightPos[1], m_data->m_lightPos[2]);

					m_data->m_renderObjects[o]->m_lightDirWorld = lightDirWorld.normalized();

					b3Vector3 lightColor(1.0, 1.0, 1.0);
					m_data->m_renderObjects[o]->m_lightColor = lightColor;

					m_data->m_renderObjects[o]->m_lightDistance = 10.0;
					m_data->m_renderObjects[o]->m_lightAmbientCoeff = 0.6;
					m_data->m_renderObjects[o]->m_lightDiffuseCoeff = 0.35;
					m_data->m_renderObjects[o]->m_lightSpecularCoeff = 0.05;
				}
			}
			TinyRenderer::renderObject(*m_data->m_renderObjects[o]);
		}
		//m_app->drawText("hello",500,500);
		render->activateTexture(m_data->m_textureHandle);
		render->updateTexture(m_data->m_textureHandle, m_data->m_rgbColorBuffer.buffer());
        Colorf color(1, 1, 1, 1);
		m_app->drawTexturedRect(0, 0, m_app->m_window->getWidth(), m_app->m_window->getHeight(), color, 0, 0, 1, 1, true);
	}
}

void TinyRendererSetup::physicsDebugDraw(int debugDrawFlags)
{
}

bool TinyRendererSetup::mouseMoveCallback(float x, float y)
{
	return false;
}

bool TinyRendererSetup::mouseButtonCallback(int button, int state, float x, float y)
{
	return false;
}

bool TinyRendererSetup::keyboardCallback(int key, int state)
{
	return false;
}
 
CommonExampleInterface* TinyRendererCreateFunc(  CommonExampleOptions& options)
{
	return new TinyRendererSetup(options.m_guiHelper);
}
//B3_STANDALONE_EXAMPLE(TinyRendererCreateFunc);