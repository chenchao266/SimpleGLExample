﻿#ifndef GUI_HELPER_INTERFACE_H
#define GUI_HELPER_INTERFACE_H
#include "Bullet3Common/Color.h"
class b3Vector3;
class btCollisionObject;
class btDynamicsWorld;
class btCollisionShape;
struct Common2dCanvasInterface;
struct CommonParameterInterface;
struct CommonRenderInterface;
struct CommonGraphicsApp;

struct GUISyncPosition
{
	int m_graphicsInstanceId;
	float m_pos[4];
	float m_orn[4];
};

typedef void (*VisualizerFlagCallback)(int flag, bool enable);

///The Bullet 2 GraphicsPhysicsBridge let's the graphics engine create graphics representation and synchronize
struct GUIHelperInterface
{
	virtual ~GUIHelperInterface() {}

	//virtual void createRigidBodyGraphicsObject(btRigidBody* body, const Colorf& color) = 0;

	virtual void createCollisionObjectGraphicsObject(btCollisionObject* obj, const Colorf& color) = 0;

	virtual void createCollisionShapeGraphicsObject(btCollisionShape* collisionShape) = 0;

	virtual void syncPhysicsToGraphics(const btDynamicsWorld* rbWorld) = 0;
	virtual void syncPhysicsToGraphics2(const btDynamicsWorld* rbWorld) {}
	virtual void syncPhysicsToGraphics2(const GUISyncPosition* positions, int numPositions) {}
	
	virtual void render(const btDynamicsWorld* rbWorld) = 0;

	virtual void createPhysicsDebugDrawer(btDynamicsWorld* rbWorld) = 0;

	virtual int registerTexture(const unsigned char* texels, int width, int height) = 0;
	virtual int registerGraphicsShape(const float* vertices, int numvertices, const int* indices, int numIndices, int primitiveType, int textureId) = 0;
	virtual int registerGraphicsInstance(int shapeIndex, const float* position, const float* quaternion, const float* color, const float* scaling) = 0;
	virtual void removeAllGraphicsInstances() = 0;
	virtual void removeGraphicsInstance(int graphicsUid) {}
	virtual void changeInstanceFlags(int instanceUid, int flags) {}
	virtual void changeRGBAColor(int instanceUid, const Colorf& rgbaColor) {}
	virtual void changeSpecularColor(int instanceUid, const Colorf& specularColor) {}
	virtual void changeTexture(int textureUniqueId, const unsigned char* rgbTexels, int width, int height) {}
	virtual void updateShape(int shapeIndex, float* vertices) {}
	virtual int getShapeIndexFromInstance(int instanceUid) { return -1; }
	virtual void replaceTexture(int shapeIndex, int textureUid) {}
	virtual void removeTexture(int textureUid) {}

	virtual Common2dCanvasInterface* get2dCanvasInterface() = 0;

	virtual CommonParameterInterface* getParameterInterface() = 0;

	virtual CommonRenderInterface* getRenderInterface() = 0;

	virtual const CommonRenderInterface* getRenderInterface() const
	{
		return 0;
	}

	virtual CommonGraphicsApp* getAppInterface() = 0;

	virtual void setUpAxis(int axis) = 0;

	virtual void resetCamera(float camDist, float yaw, float pitch, float camPosX, float camPosY, float camPosZ) = 0;

	virtual bool getCameraInfo(int* width, int* height, float viewMatrix[16], float projectionMatrix[16], 
        float camUp[3], float camForward[3], float hor[3], float vert[3], float* yaw, float* pitch, float* camDist, float camTarget[3]) const
	{
		return false;
	}

	virtual void setVisualizerFlag(int flag, int enable){};

	virtual void copyCameraImageData(const float viewMatrix[16], const float projectionMatrix[16],
									 unsigned char* pixelsRGBA, int rgbaBufferSizeInPixels,
									 float* depthBuffer, int depthBufferSizeInPixels,
									 int startPixelIndex, int destinationWidth, int destinationHeight, int* numPixelsCopied)
	{
		copyCameraImageData(viewMatrix, projectionMatrix, pixelsRGBA, rgbaBufferSizeInPixels,
							depthBuffer, depthBufferSizeInPixels,
							0, 0,
							startPixelIndex, destinationWidth,
							destinationHeight, numPixelsCopied);
	}

	virtual void copyCameraImageData(const float viewMatrix[16], const float projectionMatrix[16],
									 unsigned char* pixelsRGBA, int rgbaBufferSizeInPixels,
									 float* depthBuffer, int depthBufferSizeInPixels,
									 int* segmentationMaskBuffer, int segmentationMaskBufferSizeInPixels,
									 int startPixelIndex, int destinationWidth, int destinationHeight, int* numPixelsCopied) = 0;
	virtual void debugDisplayCameraImageData(const float viewMatrix[16], const float projectionMatrix[16],
											 unsigned char* pixelsRGBA, int rgbaBufferSizeInPixels,
											 float* depthBuffer, int depthBufferSizeInPixels,
											 int* segmentationMaskBuffer, int segmentationMaskBufferSizeInPixels,
											 int startPixelIndex, int destinationWidth, int destinationHeight, int* numPixelsCopied) {}

	virtual void setProjectiveTextureMatrices(const float viewMatrix[16], const float projectionMatrix[16]) {}
	virtual void setProjectiveTexture(bool useProjectiveTexture) {}

	virtual void autogenerateGraphicsObjects(btDynamicsWorld* rbWorld) = 0;

	virtual void drawText3D(const char* txt, float posX, float posY, float posZ, float size) {}
	virtual void drawText3D(const char* txt, float position[3], float orientation[4], const Colorf& color, float size, int optionFlag) {}

	virtual int addUserDebugText3D(const char* txt, const double positionXYZ[3], const double orientation[4], const Colorf& textColorRGB,
        double size, double lifeTime, int trackingVisualShapeIndex, int optionFlags, int replaceItemUid) { return -1; }
	virtual int addUserDebugLine(const double debugLineFromXYZ[3], const double debugLineToXYZ[3], const Colorf& debugLineColorRGB,
        double lineWidth, double lifeTime, int trackingVisualShapeIndex, int replaceItemUid) { return -1; };
	virtual int addUserDebugParameter(const char* txt, double rangeMin, double rangeMax, double startValue) { return -1; };
	virtual int readUserDebugParameter(int itemUniqueId, double* value) { return 0; }

	virtual void removeUserDebugItem(int debugItemUniqueId){};
	virtual void removeAllUserDebugItems(){};
	virtual void setVisualizerFlagCallback(VisualizerFlagCallback callback) {}

	//empty name stops dumping video
	virtual void dumpFramesToVideo(const char* mp4FileName){};
	virtual void drawDebugDrawerLines(){}
	virtual void clearLines(){}
};

///the DummyGUIHelper does nothing, so we can test the examples without GUI/graphics (in 'console mode')
struct DummyGUIHelper : public GUIHelperInterface
{
	DummyGUIHelper(CommonGraphicsApp* app = nullptr)
        : m_app(app)
	{

	}
	virtual ~DummyGUIHelper() {}

	//virtual void createRigidBodyGraphicsObject(btRigidBody* body, const Colorf&& color) {}

	virtual void createCollisionObjectGraphicsObject(btCollisionObject* obj, const Colorf&& color) {}

	virtual void createCollisionShapeGraphicsObject(btCollisionShape* collisionShape) {}

	virtual void syncPhysicsToGraphics(const btDynamicsWorld* rbWorld) {}

	virtual void render(const btDynamicsWorld* rbWorld) {}

	virtual void createPhysicsDebugDrawer(btDynamicsWorld* rbWorld) {}

	virtual int registerTexture(const unsigned char* texels, int width, int height) { return -1; }
	virtual int registerGraphicsShape(const float* vertices, int numvertices, const int* indices, int numIndices, int primitiveType, int textureId) { return -1; }
	virtual int registerGraphicsInstance(int shapeIndex, const float* position, const float* quaternion, const float* color, const float* scaling) { return -1; }
	virtual void removeAllGraphicsInstances() {}
	virtual void removeGraphicsInstance(int graphicsUid) {}
	virtual void changeRGBAColor(int instanceUid, const Colorf& rgbaColor) {}

	virtual Common2dCanvasInterface* get2dCanvasInterface()
	{
		return 0;
	}

	virtual CommonParameterInterface* getParameterInterface()
	{
		return 0;
	}

	virtual CommonRenderInterface* getRenderInterface()
	{
		return 0;
	}

	virtual CommonGraphicsApp* getAppInterface()
	{
		return m_app;
	}

	virtual void setUpAxis(int axis)
	{
	}
	virtual void resetCamera(float camDist, float yaw, float pitch, float camPosX, float camPosY, float camPosZ)
	{
	}

	virtual void copyCameraImageData(const float viewMatrix[16], const float projectionMatrix[16],
									 unsigned char* pixelsRGBA, int rgbaBufferSizeInPixels,
									 float* depthBuffer, int depthBufferSizeInPixels,
									 int* segmentationMaskBuffer, int segmentationMaskBufferSizeInPixels,
									 int startPixelIndex, int width, int height, int* numPixelsCopied)

	{
		if (numPixelsCopied)
			*numPixelsCopied = 0;
	}

	virtual void setProjectiveTextureMatrices(const float viewMatrix[16], const float projectionMatrix[16])
	{
	}

	virtual void setProjectiveTexture(bool useProjectiveTexture)
	{
	}

	virtual void autogenerateGraphicsObjects(btDynamicsWorld* rbWorld)
	{
	}

	virtual void drawText3D(const char* txt, float posX, float posZY, float posZ, float size)
	{
	}

	virtual void drawText3D(const char* txt, float position[3], float orientation[4], const Colorf& color, float size, int optionFlag)
	{
	}

	virtual int addUserDebugLine(const double debugLineFromXYZ[3], const double debugLineToXYZ[3], const Colorf& debugLineColorRGB,
        double lineWidth, double lifeTime, int trackingVisualShapeIndex, int replaceItemUid)
	{
		return -1;
	}
	virtual void removeUserDebugItem(int debugItemUniqueId)
	{
	}
	virtual void removeAllUserDebugItems()
	{
	}
    CommonGraphicsApp* m_app;
};

#endif  //GUI_HELPER_INTERFACE_H
