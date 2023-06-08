#include "OpenGLGuiHelper.h"

#include "Bullet3Common/btDynamicsWorld.h"
#include "btIDebugDraw.h"
#include "CommonInterfaces/CommonGraphicsAppInterface.h"
#include "CommonInterfaces/CommonRenderInterface.h"
#include "Bullet3Common/b3Scalar.h"
#include "Bullet3Common/b3HashMap.h"
#include "Bullet3Common/CollisionShape2TriangleMesh.h"
#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/btCollisionShape.h"
#include "Bullet3Common/btCollisionObject.h"
#include "Bullet3Common/btTriangleCallback.h"
#include "OpenGLWindow/ShapeData.h"

#include "OpenGLWindow/SimpleCamera.h"

#define BT_LINE_BATCH_SIZE 512

 
 
    inline Vec3f fromB3(const b3Vector3& org) 
    {
       return Vec3f(org.x, org.y, org.z);
    }
     
 

B3_ATTRIBUTE_ALIGNED16(class)
MyDebugDrawer : public btIDebugDraw
{
	CommonGraphicsApp* m_glApp;
	int m_debugMode;

	b3AlignedObjectArray<Vec3f> m_linePoints;
	b3AlignedObjectArray<unsigned int> m_lineIndices;

    Colorf m_currentLineColor;
	DefaultColors m_ourColors;

public:
	B3_DECLARE_ALIGNED_ALLOCATOR();

	MyDebugDrawer(CommonGraphicsApp * app)
		: m_glApp(app), m_debugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb), m_currentLineColor(-1, -1, -1)
	{
	}

	virtual ~MyDebugDrawer()
	{
	}
	virtual DefaultColors getDefaultColors() const
	{
		return m_ourColors;
	}
	///the default implementation for setDefaultColors has no effect. A derived class can implement it and store the colors.
	virtual void setDefaultColors(const DefaultColors& colors)
	{
		m_ourColors = colors;
	}

	virtual void drawLine(const b3Vector3& from1, const b3Vector3& to1, const Colorf& color1)
	{
		//float from[4] = {from1[0],from1[1],from1[2],from1[3]};
		//float to[4] = {to1[0],to1[1],to1[2],to1[3]};
		//float color[4] = {color1[0],color1[1],color1[2],color1[3]};
		//m_glApp->m_instancingRenderer->drawLine(from,to,color);
		if (m_currentLineColor != color1 || m_linePoints.size() >= BT_LINE_BATCH_SIZE)
		{
			flushLines();
			m_currentLineColor = color1;
		}
	 
		m_linePoints.push_back(fromB3(from1));
		m_linePoints.push_back(fromB3(to1));

		m_lineIndices.push_back(m_lineIndices.size());
		m_lineIndices.push_back(m_lineIndices.size());
	}

	virtual void drawContactPoint(const b3Vector3& PointOnB, const b3Vector3& normalOnB, b3Scalar distance, int lifeTime, const Colorf& color)
	{
		drawLine(PointOnB, PointOnB + normalOnB * distance, color);
        Colorf ncolor(0, 0, 0);
		drawLine(PointOnB, PointOnB + normalOnB * 0.01, ncolor);
	}

	virtual void reportErrorWarning(const char* warningString)
	{
	}

	virtual void draw3dText(const b3Vector3& location, const char* textString)
	{
	}

	virtual void setDebugMode(int debugMode)
	{
		m_debugMode = debugMode;
	}

	virtual int getDebugMode() const
	{
		return m_debugMode;
	}

	virtual void flushLines()
	{
		int sz = m_linePoints.size();
		if (sz)
		{
            Colorf debugColor;
			debugColor[0] = m_currentLineColor.r;
			debugColor[1] = m_currentLineColor.g;
			debugColor[2] = m_currentLineColor.b;
			debugColor[3] = 1.f;
			m_glApp->m_renderer->drawLines(&m_linePoints[0].x, debugColor,
										   m_linePoints.size(), sizeof(Vec3f),
										   &m_lineIndices[0],
										   m_lineIndices.size(),
										   1);
			m_linePoints.clear();
			m_lineIndices.clear();
		}
	}
};

static Colorf sColors[4] =
	{
        Colorf(60. / 256.f, 186. / 256.f, 84. / 256.f, 1),
        Colorf(244. / 256.f, 194. / 256.f, 13. / 256.f, 1),
        Colorf(219. / 256.f, 50. / 256.f, 54. / 256.f, 1),
        Colorf(72. / 256.f, 133. / 256.f, 237. / 256.f, 1),

		//Colorf(1,1,0,1),
};

struct MyHashShape
{
	int m_shapeKey;
	int m_shapeType;
	b3Vector3 m_sphere0Pos;
	b3Vector3 m_sphere1Pos;
	b3Vector3 m_halfExtents;
	b3Scalar m_radius0;
	b3Scalar m_radius1;
	b3Transform m_childTransform;
	int m_deformFunc;
	int m_upAxis;
	b3Scalar m_halfHeight;

	MyHashShape()
		: m_shapeKey(0),
		  m_shapeType(0),
		  m_sphere0Pos(b3Vector3(0, 0, 0)),
		  m_sphere1Pos(b3Vector3(0, 0, 0)),
		  m_halfExtents(b3Vector3(0, 0, 0)),
		  m_radius0(0),
		  m_radius1(0),
		  m_deformFunc(0),
		  m_upAxis(-1),
		  m_halfHeight(0)
	{
		m_childTransform.setIdentity();
	}

	bool equals(const MyHashShape& other) const
	{
		bool sameShapeType = m_shapeType == other.m_shapeType;
		bool sameSphere0 = m_sphere0Pos == other.m_sphere0Pos;
		bool sameSphere1 = m_sphere1Pos == other.m_sphere1Pos;
		bool sameHalfExtents = m_halfExtents == other.m_halfExtents;
		bool sameRadius0 = m_radius0 == other.m_radius0;
		bool sameRadius1 = m_radius1 == other.m_radius1;
		bool sameTransform = m_childTransform == other.m_childTransform;
		bool sameUpAxis = m_upAxis == other.m_upAxis;
		bool sameHalfHeight = m_halfHeight == other.m_halfHeight;
		return sameShapeType && sameSphere0 && sameSphere1 && sameHalfExtents && sameRadius0 && sameRadius1 && sameTransform && sameUpAxis && sameHalfHeight;
	}
	//to our success
	B3_FORCE_INLINE unsigned int getHash() const
	{
		unsigned int key = m_shapeKey;
		// Thomas Wang's hash
		key += ~(key << 15);
		key ^= (key >> 10);
		key += (key << 3);
		key ^= (key >> 6);
		key += ~(key << 11);
		key ^= (key >> 16);

		return key;
	}
};

struct OpenGLGuiHelperInternalData
{
	CommonGraphicsApp* m_glApp;
	class MyDebugDrawer* m_debugDraw;
	bool m_vrMode;
	int m_vrSkipShadowPass;

	b3AlignedObjectArray<unsigned char> m_rgbaPixelBuffer1;
	b3AlignedObjectArray<float> m_depthBuffer1;
	b3AlignedObjectArray<int> m_segmentationMaskBuffer;
	b3HashMap<MyHashShape, int> m_hashShapes;

	VisualizerFlagCallback m_visualizerFlagCallback;

	int m_checkedTexture;
	int m_checkedTextureGrey;

	OpenGLGuiHelperInternalData()
		: m_vrMode(false),
		  m_vrSkipShadowPass(0),
		  m_visualizerFlagCallback(0),
		  m_checkedTexture(-1),
		  m_checkedTextureGrey(-1)
	{
	}
};

void OpenGLGuiHelper::setVRMode(bool vrMode)
{
	m_data->m_vrMode = vrMode;
	m_data->m_vrSkipShadowPass = 0;
}

OpenGLGuiHelper::OpenGLGuiHelper(CommonGraphicsApp* glApp, bool  )
{
	m_data = new OpenGLGuiHelperInternalData;
	m_data->m_glApp = glApp;
	m_data->m_debugDraw = 0;
}

OpenGLGuiHelper::~OpenGLGuiHelper()
{
	delete m_data->m_debugDraw;

	delete m_data;
}


b3Vector3 OpenGLGuiHelper::getRayTo(int x, int y)
{
    CommonRenderInterface* renderer = getRenderInterface();

    if (!renderer)
    {
        b3Assert(0);
        return b3Vector3(0, 0, 0);
    }

    float top = 1.f;
    float bottom = -1.f;
    float nearPlane = 1.f;
    float tanFov = (top - bottom) * 0.5f / nearPlane;
    float fov = b3Scalar(2.0) * b3Atan(tanFov);

    b3Vector3 camPos, camTarget;

    renderer->getActiveCamera()->getCameraPosition(camPos);
    renderer->getActiveCamera()->getCameraTargetPosition(camTarget);

    b3Vector3 rayFrom = camPos;
    b3Vector3 rayForward = (camTarget - camPos);
    rayForward.normalize();
    float farPlane = 10000.f;
    rayForward *= farPlane;

    b3Vector3 rightOffset;
    b3Vector3 cameraUp = b3Vector3(0, 0, 0);
    cameraUp[getAppInterface()->getUpAxis()] = 1;

    b3Vector3 vertical = cameraUp;

    b3Vector3 hor;
    hor = rayForward.cross(vertical);
    hor.safeNormalize();
    vertical = hor.cross(rayForward);
    vertical.safeNormalize();

    float tanfov = tanf(0.5f * fov);

    hor *= 2.f * farPlane * tanfov;
    vertical *= 2.f * farPlane * tanfov;

    b3Scalar aspect;
    float width = float(renderer->getScreenWidth());
    float height = float(renderer->getScreenHeight());

    aspect = width / height;

    hor *= aspect;

    b3Vector3 rayToCenter = rayFrom + rayForward;
    b3Vector3 dHor = hor * 1.f / width;
    b3Vector3 dVert = vertical * 1.f / height;

    b3Vector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
    rayTo += b3Scalar(x) * dHor;
    rayTo -= b3Scalar(y) * dVert;
    return rayTo;
}

CommonRenderInterface* OpenGLGuiHelper::getRenderInterface()
{
	return m_data->m_glApp->m_renderer;
}

const CommonRenderInterface* OpenGLGuiHelper::getRenderInterface() const
{
	return m_data->m_glApp->m_renderer;
}
 

class MyTriangleCollector2 : public btTriangleCallback
{
public:
	b3AlignedObjectArray<GLInstanceVertex>* m_pVerticesOut;
	b3AlignedObjectArray<int>* m_pIndicesOut;
	b3Vector3 m_aabbMin, m_aabbMax;
	b3Scalar m_textureScaling;

	MyTriangleCollector2(const b3Vector3& aabbMin, const b3Vector3& aabbMax)
		:m_aabbMin(aabbMin), m_aabbMax(aabbMax), m_textureScaling(1)
	{
		m_pVerticesOut = 0;
		m_pIndicesOut = 0;
	}

	virtual void processTriangle(b3Vector3* tris, int partId, int triangleIndex)
	{
		for (int k = 0; k < 3; k++)
		{
			GLInstanceVertex v;
			v.xyzw[3] = 0;
			
			b3Vector3 normal = (tris[0] - tris[1]).cross(tris[0] - tris[2]);
			normal.safeNormalize();
			for (int l = 0; l < 3; l++)
			{
				v.xyzw[l] = tris[k][l];
				v.normal[l] = normal[l];
			}
			
			b3Vector3 extents = m_aabbMax - m_aabbMin;
			
			v.uv[0] = (1.-((v.xyzw[0] - m_aabbMin[0]) / (m_aabbMax[0] - m_aabbMin[0])))*m_textureScaling;
			v.uv[1] = (1.-(v.xyzw[1] - m_aabbMin[1]) / (m_aabbMax[1] - m_aabbMin[1]))*m_textureScaling;

			m_pIndicesOut->push_back(m_pVerticesOut->size());
			m_pVerticesOut->push_back(v);
		}
	}
};
void OpenGLGuiHelper::createCollisionObjectGraphicsObject(btCollisionObject* body, const Colorf& color)
{
	if (body->getUserIndex() < 0)
	{
		btCollisionShape* shape = body->getCollisionShape();
		b3Transform startTransform = body->getWorldTransform();
		int graphicsShapeId = shape->getUserIndex();
		if (graphicsShapeId >= 0)
		{
			//	b3Assert(graphicsShapeId >= 0);
			//the graphics shape is already scaled
			b3Vector3 localScaling(1, 1, 1);
			int graphicsInstanceId = m_data->m_glApp->m_renderer->registerGraphicsInstance(graphicsShapeId, startTransform.getOrigin(), startTransform.getRotation(), color.c, localScaling);
			body->setUserIndex(graphicsInstanceId);
		}
	}
}

int OpenGLGuiHelper::registerTexture(const unsigned char* texels, int width, int height)
{
	int textureId = m_data->m_glApp->m_renderer->registerTexture(texels, width, height);
	return textureId;
}

void OpenGLGuiHelper::removeTexture(int textureUid)
{
	m_data->m_glApp->m_renderer->removeTexture(textureUid);
}

void OpenGLGuiHelper::changeTexture(int textureUniqueId, const unsigned char* rgbTexels, int width, int height)
{
	bool flipPixelsY = true;
	m_data->m_glApp->m_renderer->updateTexture(textureUniqueId, rgbTexels, flipPixelsY);
}

int OpenGLGuiHelper::registerGraphicsShape(const float* vertices, int numvertices, const int* indices, int numIndices, int primitiveType, int textureId)
{
	if (textureId == -2)
	{
		if (m_data->m_checkedTextureGrey < 0)
		{
			m_data->m_checkedTextureGrey = createCheckeredTexture(192, 192, 192);
		}
		textureId = m_data->m_checkedTextureGrey;
	}

	int shapeId = m_data->m_glApp->m_renderer->registerShape(vertices, numvertices, indices, numIndices, primitiveType, textureId);
	return shapeId;
}

int OpenGLGuiHelper::registerGraphicsInstance(int shapeIndex, const float* position, const float* quaternion, const float* color, const float* scaling)
{
	return m_data->m_glApp->m_renderer->registerGraphicsInstance(shapeIndex, position, quaternion, color, scaling);
}

void OpenGLGuiHelper::removeAllGraphicsInstances()
{
	m_data->m_hashShapes.clear();
	m_data->m_glApp->m_renderer->removeAllInstances();
}

void OpenGLGuiHelper::removeGraphicsInstance(int graphicsUid)
{
	if (graphicsUid >= 0)
	{
		m_data->m_glApp->m_renderer->removeGraphicsInstance(graphicsUid);
	};
}

int OpenGLGuiHelper::getShapeIndexFromInstance(int instanceUid)
{
	return m_data->m_glApp->m_renderer->getShapeIndexFromInstance(instanceUid);
}

void OpenGLGuiHelper::replaceTexture(int shapeIndex, int textureUid)
{
	if (shapeIndex >= 0)
	{
		m_data->m_glApp->m_renderer->replaceTexture(shapeIndex, textureUid);
	};
}
void OpenGLGuiHelper::changeInstanceFlags(int instanceUid, int flags)
{
	if (instanceUid >= 0)
	{
		//careful, flags/instanceUid is swapped
		m_data->m_glApp->m_renderer->writeSingleInstanceFlagsToCPU(  flags, instanceUid);
	}
}
void OpenGLGuiHelper::changeRGBAColor(int instanceUid, const Colorf& rgbaColor)
{
	if (instanceUid >= 0)
	{
		m_data->m_glApp->m_renderer->writeSingleInstanceColorToCPU(rgbaColor.c, instanceUid);
	};
}
void OpenGLGuiHelper::changeSpecularColor(int instanceUid, const Colorf& specularColor)
{
	if (instanceUid >= 0)
	{
		m_data->m_glApp->m_renderer->writeSingleInstanceSpecularColorToCPU(specularColor.c, instanceUid);
	};
}
int OpenGLGuiHelper::createCheckeredTexture(int red, int green, int blue)
{
	int texWidth = 1024;
	int texHeight = 1024;
	b3AlignedObjectArray<unsigned char> texels;
	texels.resize(texWidth * texHeight * 3);
	for (int i = 0; i < texWidth * texHeight * 3; i++)
		texels[i] = 255;

	for (int i = 0; i < texWidth; i++)
	{
		for (int j = 0; j < texHeight; j++)
		{
			int a = i < texWidth / 2 ? 1 : 0;
			int b = j < texWidth / 2 ? 1 : 0;

			if (a == b)
			{
				texels[(i + j * texWidth) * 3 + 0] = red;
				texels[(i + j * texWidth) * 3 + 1] = green;
				texels[(i + j * texWidth) * 3 + 2] = blue;
				//					texels[(i+j*texWidth)*4+3] = 255;
			}
			/*else
				{
					texels[i*3+0+j*texWidth] = 255;
					texels[i*3+1+j*texWidth] = 255;
					texels[i*3+2+j*texWidth] = 255;
				}
				*/
		}
	}

	int texId = registerTexture(&texels[0], texWidth, texHeight);
	return texId;
}

void OpenGLGuiHelper::createCollisionShapeGraphicsObject(btCollisionShape* collisionShape)
{
	//already has a graphics object?
	if (collisionShape->getUserIndex() >= 0)
		return;

	if (m_data->m_checkedTexture < 0)
	{
		m_data->m_checkedTexture = createCheckeredTexture(173, 199, 255);
	}

	if (m_data->m_checkedTextureGrey < 0)
	{
		m_data->m_checkedTextureGrey = createCheckeredTexture(192, 192, 192);
	}

	b3AlignedObjectArray<GLInstanceVertex> gfxVertices;
	b3AlignedObjectArray<int> indices;
	int strideInBytes = GLInstanceVertex::Length * sizeof(float);
	if (collisionShape->getShapeType() <= EMPTY_SHAPE_PROXYTYPE)
	{///...
	}

	b3Transform startTrans;
	startTrans.setIdentity();
	//todo: create some textured objects for popular objects, like plane, cube, sphere, capsule

	{
		b3AlignedObjectArray<b3Vector3> vertexPositions;
		b3AlignedObjectArray<b3Vector3> vertexNormals;
		CollisionShape2TriangleMesh(collisionShape, startTrans, vertexPositions, vertexNormals, indices);
		gfxVertices.resize(vertexPositions.size());
		for (int i = 0; i < vertexPositions.size(); i++)
		{
			for (int j = 0; j < 4; j++)
			{
				gfxVertices[i].xyzw[j] = vertexPositions[i][j];
			}
			for (int j = 0; j < 3; j++)
			{
				gfxVertices[i].normal[j] = vertexNormals[i][j];
			}
			for (int j = 0; j < 2; j++)
			{
				gfxVertices[i].uv[j] = 0.5;  //we don't have UV info...
			}
		}
	}

	if (gfxVertices.size() && indices.size())
	{
		int shapeId = registerGraphicsShape(&gfxVertices[0].xyzw[0], gfxVertices.size(), &indices[0], indices.size(), B3_GL_TRIANGLES, -1);
		collisionShape->setUserIndex(shapeId);
	}
}
void OpenGLGuiHelper::syncPhysicsToGraphics(const btDynamicsWorld* rbWorld)
{
	//in VR mode, we skip the synchronization for the second eye
	if (m_data->m_vrMode && m_data->m_vrSkipShadowPass == 1)
		return;

	int numCollisionObjects = rbWorld->getNumCollisionObjects();
	{
		B3_PROFILE("write all InstanceTransformToCPU");
		for (int i = 0; i < numCollisionObjects; i++)
		{
			//B3_PROFILE("writeSingleInstanceTransformToCPU");
			btCollisionObject* colObj = rbWorld->getCollisionObjectArray()[i];
			btCollisionShape* collisionShape = colObj->getCollisionShape();
			if (collisionShape->getShapeType() == SOFTBODY_SHAPE_PROXYTYPE && collisionShape->getUserIndex() >= 0)
			{
				b3AlignedObjectArray<GLInstanceVertex> gfxVertices;
				b3AlignedObjectArray<int> indices;
				computeSoftBodyVertices(collisionShape, gfxVertices, indices);
				m_data->m_glApp->m_renderer->updateShape(collisionShape->getUserIndex(), &gfxVertices[0].xyzw[0]);
				continue;
			}
			b3Vector3 pos = colObj->getWorldTransform().getOrigin();
			b3Quaternion orn = colObj->getWorldTransform().getRotation();
			int index = colObj->getUserIndex();
			if (index >= 0)
			{
				m_data->m_glApp->m_renderer->writeSingleInstanceTransformToCPU(pos, orn, index);
			}
		}
	}
	{
		B3_PROFILE("writeTransforms");
		m_data->m_glApp->m_renderer->writeTransforms();
	}
}

void OpenGLGuiHelper::render(const btDynamicsWorld* rbWorld)
{
	if (m_data->m_vrMode)
	{
		//in VR, we skip the shadow generation for the second eye

		if (m_data->m_vrSkipShadowPass >= 1)
		{
			m_data->m_glApp->m_renderer->renderSceneInternal(B3_USE_SHADOWMAP_RENDERMODE);
			m_data->m_vrSkipShadowPass = 0;
		}
		else
		{
			m_data->m_glApp->m_renderer->renderScene();
			m_data->m_vrSkipShadowPass++;
		}
	}
	else
	{
		m_data->m_glApp->m_renderer->renderScene();
	}
}
void OpenGLGuiHelper::createPhysicsDebugDrawer(btDynamicsWorld* rbWorld)
{
	b3Assert(rbWorld);
	if (m_data->m_debugDraw)
	{
		delete m_data->m_debugDraw;
		m_data->m_debugDraw = 0;
	}

	m_data->m_debugDraw = new MyDebugDrawer(m_data->m_glApp);
	rbWorld->setDebugDrawer(m_data->m_debugDraw);

	m_data->m_debugDraw->setDebugMode(
		btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawAabb
		//btIDebugDraw::DBG_DrawContactPoints
	);
}

Common2dCanvasInterface* OpenGLGuiHelper::get2dCanvasInterface()
{
	return m_data->m_glApp->m_2dCanvasInterface;
}

CommonParameterInterface* OpenGLGuiHelper::getParameterInterface()
{
	return m_data->m_glApp->m_parameterInterface;
}

void OpenGLGuiHelper::setUpAxis(int axis)
{
	m_data->m_glApp->setUpAxis(axis);
}

void OpenGLGuiHelper::setVisualizerFlagCallback(VisualizerFlagCallback callback)
{
	m_data->m_visualizerFlagCallback = callback;
}

void OpenGLGuiHelper::setVisualizerFlag(int flag, int enable)
{
	if (getRenderInterface() && flag == 16)  //COV_ENABLE_PLANAR_REFLECTION
	{
		getRenderInterface()->setPlaneReflectionShapeIndex(enable);
	}
	if (m_data->m_visualizerFlagCallback)
		(m_data->m_visualizerFlagCallback)(flag, enable != 0);
}

void OpenGLGuiHelper::resetCamera(float camDist, float yaw, float pitch, float camPosX, float camPosY, float camPosZ)
{
	if (getRenderInterface() && getRenderInterface()->getActiveCamera())
	{
		getRenderInterface()->getActiveCamera()->setCameraDistance(camDist);
		getRenderInterface()->getActiveCamera()->setCameraPitch(pitch);
		getRenderInterface()->getActiveCamera()->setCameraYaw(yaw);
		getRenderInterface()->getActiveCamera()->setCameraTargetPosition(camPosX, camPosY, camPosZ);
	}
}

bool OpenGLGuiHelper::getCameraInfo(int* width, int* height, float viewMatrix[16], float projectionMatrix[16], float camUp[3], float camForward[3], float hor[3], float vert[3], float* yaw, float* pitch, float* camDist, float cameraTarget[3]) const
{
	if (getRenderInterface() && getRenderInterface()->getActiveCamera())
	{
		*width = m_data->m_glApp->m_window->getWidth();
		*height = m_data->m_glApp->m_window->getHeight();
		getRenderInterface()->getActiveCamera()->getCameraViewMatrix(viewMatrix);
		getRenderInterface()->getActiveCamera()->getCameraProjectionMatrix(projectionMatrix);
		getRenderInterface()->getActiveCamera()->getCameraUpVector(camUp);
		getRenderInterface()->getActiveCamera()->getCameraForwardVector(camForward);

		float top = 1.f;
		float bottom = -1.f;
		float tanFov = (top - bottom) * 0.5f / 1;
		float fov = b3Scalar(2.0) * b3Atan(tanFov);
		b3Vector3 camPos, camTarget;
		getRenderInterface()->getActiveCamera()->getCameraPosition(camPos);
		getRenderInterface()->getActiveCamera()->getCameraTargetPosition(camTarget);
		b3Vector3 rayFrom = camPos;
		b3Vector3 rayForward = (camTarget - camPos);
		rayForward.normalize();
		float farPlane = 10000.f;
		rayForward *= farPlane;

		b3Vector3 rightOffset;
		b3Vector3 cameraUp = b3Vector3(camUp[0], camUp[1], camUp[2]);
		b3Vector3 vertical = cameraUp;
		b3Vector3 hori;
		hori = rayForward.cross(vertical);
		hori.normalize();
		vertical = hori.cross(rayForward);
		vertical.normalize();
		float tanfov = tanf(0.5f * fov);
		hori *= 2.f * farPlane * tanfov;
		vertical *= 2.f * farPlane * tanfov;
		b3Scalar aspect = float(*width) / float(*height);
		hori *= aspect;
		//compute 'hor' and 'vert' vectors, useful to generate raytracer rays
		hor[0] = hori[0];
		hor[1] = hori[1];
		hor[2] = hori[2];
		vert[0] = vertical[0];
		vert[1] = vertical[1];
		vert[2] = vertical[2];

		*yaw = getRenderInterface()->getActiveCamera()->getCameraYaw();
		*pitch = getRenderInterface()->getActiveCamera()->getCameraPitch();
		*camDist = getRenderInterface()->getActiveCamera()->getCameraDistance();
		cameraTarget[0] = camTarget[0];
		cameraTarget[1] = camTarget[1];
		cameraTarget[2] = camTarget[2];
		return true;
	}
	return false;
}

void OpenGLGuiHelper::setProjectiveTextureMatrices(const float viewMatrix[16], const float projectionMatrix[16])
{
	m_data->m_glApp->m_renderer->setProjectiveTextureMatrices(viewMatrix, projectionMatrix);
}

void OpenGLGuiHelper::setProjectiveTexture(bool useProjectiveTexture)
{
	m_data->m_glApp->m_renderer->setProjectiveTexture(useProjectiveTexture);
}

void OpenGLGuiHelper::copyCameraImageData(const float viewMatrix[16], const float projectionMatrix[16],
										  unsigned char* pixelsRGBA, int rgbaBufferSizeInPixels,
										  float* depthBuffer, int depthBufferSizeInPixels,
										  int* segmentationMaskBuffer, int segmentationMaskBufferSizeInPixels,
										  int startPixelIndex, int destinationWidth,
										  int destinationHeight, int* numPixelsCopied)
{
	int sourceWidth = b3Min(destinationWidth, (int)(m_data->m_glApp->m_window->getWidth() * m_data->m_glApp->m_window->getRetinaScale()));
	int sourceHeight = b3Min(destinationHeight, (int)(m_data->m_glApp->m_window->getHeight() * m_data->m_glApp->m_window->getRetinaScale()));
	m_data->m_glApp->setViewport(sourceWidth, sourceHeight);

	if (numPixelsCopied)
		*numPixelsCopied = 0;

	int numTotalPixels = destinationWidth * destinationHeight;
	int numRemainingPixels = numTotalPixels - startPixelIndex;
	int numBytesPerPixel = 4;  //RGBA
	int numRequestedPixels = b3Min(rgbaBufferSizeInPixels, numRemainingPixels);
	if (numRequestedPixels)
	{
		if (startPixelIndex == 0)
		{
			CommonCameraInterface* oldCam = getRenderInterface()->getActiveCamera();
			SimpleCamera tempCam;
			getRenderInterface()->setActiveCamera(&tempCam);
			getRenderInterface()->getActiveCamera()->setVRCamera(viewMatrix, projectionMatrix);
			{
				B3_PROFILE("renderScene");
				getRenderInterface()->renderScene();
			}

			{
				B3_PROFILE("copy pixels");
				b3AlignedObjectArray<unsigned char> sourceRgbaPixelBuffer;
				b3AlignedObjectArray<float> sourceDepthBuffer;
				//copy the image into our local cache
				sourceRgbaPixelBuffer.resize(sourceWidth * sourceHeight * numBytesPerPixel);
				sourceDepthBuffer.resize(sourceWidth * sourceHeight);
				{
					B3_PROFILE("getScreenPixels");
					m_data->m_glApp->getScreenPixels(&(sourceRgbaPixelBuffer[0]), sourceRgbaPixelBuffer.size(), &sourceDepthBuffer[0], sizeof(float) * sourceDepthBuffer.size());
				}

				m_data->m_rgbaPixelBuffer1.resize(destinationWidth * destinationHeight * numBytesPerPixel);
				m_data->m_depthBuffer1.resize(destinationWidth * destinationHeight);
				//rescale and flip
				{
					B3_PROFILE("resize and flip");
					for (int j = 0; j < destinationHeight; j++)
					{
						for (int i = 0; i < destinationWidth; i++)
						{
							int xIndex = int(float(i) * (float(sourceWidth) / float(destinationWidth)));
							int yIndex = int(float(destinationHeight - 1 - j) * (float(sourceHeight) / float(destinationHeight)));
							b3Clamp(xIndex, 0, sourceWidth);
							b3Clamp(yIndex, 0, sourceHeight);
							int bytesPerPixel = 4;  //RGBA

							int sourcePixelIndex = (xIndex + yIndex * sourceWidth) * bytesPerPixel;
							int sourceDepthIndex = xIndex + yIndex * sourceWidth;
#define COPY4PIXELS 1
#ifdef COPY4PIXELS
							int* dst = (int*)&m_data->m_rgbaPixelBuffer1[(i + j * destinationWidth) * 4 + 0];
							int* src = (int*)&sourceRgbaPixelBuffer[sourcePixelIndex + 0];
							*dst = *src;

#else
							m_data->m_rgbaPixelBuffer1[(i + j * destinationWidth) * 4 + 0] = sourceRgbaPixelBuffer[sourcePixelIndex + 0];
							m_data->m_rgbaPixelBuffer1[(i + j * destinationWidth) * 4 + 1] = sourceRgbaPixelBuffer[sourcePixelIndex + 1];
							m_data->m_rgbaPixelBuffer1[(i + j * destinationWidth) * 4 + 2] = sourceRgbaPixelBuffer[sourcePixelIndex + 2];
							m_data->m_rgbaPixelBuffer1[(i + j * destinationWidth) * 4 + 3] = 255;
#endif
							if (depthBuffer)
							{
								m_data->m_depthBuffer1[i + j * destinationWidth] = sourceDepthBuffer[sourceDepthIndex];
							}
						}
					}
				}
			}

			//segmentation mask

			if (segmentationMaskBuffer)
			{
				{
					m_data->m_glApp->m_window->startRendering();
					m_data->m_glApp->setViewport(sourceWidth, sourceHeight);
					B3_PROFILE("renderScene");
					getRenderInterface()->renderSceneInternal(B3_SEGMENTATION_MASK_RENDERMODE);
				}

				{
					B3_PROFILE("copy pixels");
					b3AlignedObjectArray<unsigned char> sourceRgbaPixelBuffer;
					b3AlignedObjectArray<float> sourceDepthBuffer;
					//copy the image into our local cache
					sourceRgbaPixelBuffer.resize(sourceWidth * sourceHeight * numBytesPerPixel);
					sourceDepthBuffer.resize(sourceWidth * sourceHeight);
					{
						B3_PROFILE("getScreenPixelsSegmentationMask");
						m_data->m_glApp->getScreenPixels(&(sourceRgbaPixelBuffer[0]), sourceRgbaPixelBuffer.size(), &sourceDepthBuffer[0], sizeof(float) * sourceDepthBuffer.size());
					}
					m_data->m_segmentationMaskBuffer.resize(destinationWidth * destinationHeight, -1);

					//rescale and flip
					{
						B3_PROFILE("resize and flip segmentation mask");
						for (int j = 0; j < destinationHeight; j++)
						{
							for (int i = 0; i < destinationWidth; i++)
							{
								int xIndex = int(float(i) * (float(sourceWidth) / float(destinationWidth)));
								int yIndex = int(float(destinationHeight - 1 - j) * (float(sourceHeight) / float(destinationHeight)));
								b3Clamp(xIndex, 0, sourceWidth);
								b3Clamp(yIndex, 0, sourceHeight);
								int bytesPerPixel = 4;  //RGBA
								int sourcePixelIndex = (xIndex + yIndex * sourceWidth) * bytesPerPixel;
								int sourceDepthIndex = xIndex + yIndex * sourceWidth;

								if (segmentationMaskBuffer)
								{
									float depth = sourceDepthBuffer[sourceDepthIndex];
									if (depth < 1)
									{
										int segMask = sourceRgbaPixelBuffer[sourcePixelIndex + 0] + 256 * (sourceRgbaPixelBuffer[sourcePixelIndex + 1]) + 256 * 256 * (sourceRgbaPixelBuffer[sourcePixelIndex + 2]);
										m_data->m_segmentationMaskBuffer[i + j * destinationWidth] = segMask;
									}
									else
									{
										m_data->m_segmentationMaskBuffer[i + j * destinationWidth] = -1;
									}
								}
							}
						}
					}
				}
			}

			getRenderInterface()->setActiveCamera(oldCam);

			if (1)
			{
				getRenderInterface()->getActiveCamera()->disableVRCamera();
				DrawGridData dg;
				dg.upAxis = m_data->m_glApp->getUpAxis();
				getRenderInterface()->updateCamera(dg.upAxis);
				m_data->m_glApp->m_window->startRendering();
			}
		}
		if (pixelsRGBA)
		{
			B3_PROFILE("copy rgba pixels");

			for (int i = 0; i < numRequestedPixels * numBytesPerPixel; i++)
			{
				pixelsRGBA[i] = m_data->m_rgbaPixelBuffer1[i + startPixelIndex * numBytesPerPixel];
			}
		}
		if (depthBuffer)
		{
			B3_PROFILE("copy depth buffer pixels");

			for (int i = 0; i < numRequestedPixels; i++)
			{
				depthBuffer[i] = m_data->m_depthBuffer1[i + startPixelIndex];
			}
		}
		if (segmentationMaskBuffer)
		{
			B3_PROFILE("copy segmentation mask pixels");
			for (int i = 0; i < numRequestedPixels; i++)
			{
				segmentationMaskBuffer[i] = m_data->m_segmentationMaskBuffer[i + startPixelIndex];
			}
		}
		if (numPixelsCopied)
			*numPixelsCopied = numRequestedPixels;
	}

	m_data->m_glApp->setViewport(-1, -1);
}

struct MyConvertPointerSizeT
{
	union {
		const void* m_ptr;
		size_t m_int;
	};
};
bool shapePointerCompareFunc(const btCollisionObject* colA, const btCollisionObject* colB)
{
	MyConvertPointerSizeT a, b;
	a.m_ptr = colA->getCollisionShape();
	b.m_ptr = colB->getCollisionShape();
	return (a.m_int < b.m_int);
}

void OpenGLGuiHelper::autogenerateGraphicsObjects(btDynamicsWorld* rbWorld)
{
	//sort the collision objects based on collision shape, the gfx library requires instances that re-use a shape to be added after eachother

	b3AlignedObjectArray<btCollisionObject*> sortedObjects;
	sortedObjects.reserve(rbWorld->getNumCollisionObjects());
	for (int i = 0; i < rbWorld->getNumCollisionObjects(); i++)
	{
		btCollisionObject* colObj = rbWorld->getCollisionObjectArray()[i];
		sortedObjects.push_back(colObj);
	}
	sortedObjects.quickSort(shapePointerCompareFunc);
	for (int i = 0; i < sortedObjects.size(); i++)
	{
		btCollisionObject* colObj = sortedObjects[i];
        //btSoftBody* sb = btSoftBody::upcast(colObj);
        if (0)
        {///...
            colObj->getCollisionShape()->setUserPointer(colObj);
        }
		createCollisionShapeGraphicsObject(colObj->getCollisionShape());
		int colorIndex = colObj->getUserIndex2() & 3;//Uid

		Colorf color;
		color = sColors[colorIndex];
		if (colObj->getCollisionShape()->getShapeType() == STATIC_PLANE_PROXYTYPE)
		{
			color = Colorf(1, 1, 1, 1);
		}
		createCollisionObjectGraphicsObject(colObj, color);
		if (0)
		{///...
			int graphicsInstanceId = colObj->getUserIndex();
			changeInstanceFlags(graphicsInstanceId, B3_INSTANCE_DOUBLE_SIDED);
		}
	}
}

void OpenGLGuiHelper::drawText3D(const char* txt, float position[3], float orientation[4], const Colorf& color, float size, int optionFlags)
{
	B3_PROFILE("OpenGLGuiHelper::drawText3D");

	b3Assert(m_data->m_glApp);
	m_data->m_glApp->drawText3D(txt, position, orientation, color, size, optionFlags);
}

void OpenGLGuiHelper::drawText3D(const char* txt, float posX, float posY, float posZ, float size)
{
	B3_PROFILE("OpenGLGuiHelper::drawText3D");

	b3Assert(m_data->m_glApp);
	m_data->m_glApp->drawText3D(txt, posX, posY, posZ, size);
}

CommonGraphicsApp* OpenGLGuiHelper::getAppInterface()
{
	return m_data->m_glApp;
}

void OpenGLGuiHelper::dumpFramesToVideo(const char* mp4FileName)
{
	if (m_data->m_glApp)
	{
		m_data->m_glApp->dumpFramesToVideo(mp4FileName);
	}
}

void OpenGLGuiHelper::computeSoftBodyVertices(btCollisionShape* collisionShape,
											  b3AlignedObjectArray<GLInstanceVertex>& gfxVertices,
											  b3AlignedObjectArray<int>& indices)
{
	if (collisionShape->getUserPointer() == 0)
		return;
	b3Assert(collisionShape->getUserPointer());
    //btSoftBody* sb = (btSoftBody*)collisionShape->getUserPointer();
	///...

 
}
 
void OpenGLGuiHelper::updateShape(int shapeIndex, float* vertices)
{
	m_data->m_glApp->m_renderer->updateShape(shapeIndex, vertices);
}