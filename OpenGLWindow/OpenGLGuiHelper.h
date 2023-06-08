#ifndef OPENGL_GUI_HELPER_H
#define OPENGL_GUI_HELPER_H
#include "CommonInterfaces/CommonGUIHelperInterface.h"

class btCollisionObject;
class btCollisionShape;
class b3Transform;
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "Bullet3Common/Color.h"
#include "OpenGLWindow/GLInstanceGraphicsShape.h"

struct OpenGLGuiHelper : public GUIHelperInterface
{
	struct OpenGLGuiHelperInternalData* m_data;

	OpenGLGuiHelper(  CommonGraphicsApp* glApp, bool  );

	virtual ~OpenGLGuiHelper();
    b3Vector3 getRayTo(int x, int y);
	virtual   CommonRenderInterface* getRenderInterface();
	virtual const   CommonRenderInterface* getRenderInterface() const;
     
	virtual void createCollisionObjectGraphicsObject(btCollisionObject* body, const Colorf& color);

	virtual int registerTexture(const unsigned char* texels, int width, int height);
	virtual int registerGraphicsShape(const float* vertices, int numvertices, const int* indices, int numIndices, int primitiveType, int textureId);
	virtual int registerGraphicsInstance(int shapeIndex, const float* position, const float* quaternion, const float* color, const float* scaling);
	virtual void removeAllGraphicsInstances();
	virtual void removeGraphicsInstance(int graphicsUid);
	virtual void changeInstanceFlags(int instanceUid, int flags);
	virtual void changeRGBAColor(int instanceUid, const Colorf& rgbaColor);
	virtual void changeSpecularColor(int instanceUid, const Colorf& specularColor);
	virtual void changeTexture(int textureUniqueId, const unsigned char* rgbTexels, int width, int height);
	virtual void removeTexture(int textureUid);
	virtual int getShapeIndexFromInstance(int instanceUid);
	virtual void replaceTexture(int shapeIndex, int textureUid);
	virtual void updateShape(int shapeIndex, float* vertices);

	virtual void createCollisionShapeGraphicsObject(btCollisionShape* collisionShape);

	virtual void syncPhysicsToGraphics(const btDynamicsWorld* rbWorld);

	virtual void render(const btDynamicsWorld* rbWorld);

	virtual void createPhysicsDebugDrawer(btDynamicsWorld* rbWorld);

	virtual   Common2dCanvasInterface* get2dCanvasInterface();

	virtual CommonParameterInterface* getParameterInterface();

	virtual   CommonGraphicsApp* getAppInterface();

	virtual void setUpAxis(int axis);

	virtual void resetCamera(float camDist, float yaw, float pitch, float camPosX, float camPosY, float camPosZ);
	virtual bool getCameraInfo(int* width, int* height, float viewMatrix[16], float projectionMatrix[16], 
        float camUp[3], float camForward[3], float hor[3], float vert[3], float* yaw, float* pitch, float* camDist, float cameraTarget[3]) const;

	virtual void copyCameraImageData(const float viewMatrix[16], const float projectionMatrix[16],
									 unsigned char* pixelsRGBA, int rgbaBufferSizeInPixels,
									 float* depthBuffer, int depthBufferSizeInPixels,
									 int* segmentationMaskBuffer, int segmentationMaskBufferSizeInPixels,
									 int startPixelIndex, int destinationWidth,
									 int destinationHeight, int* numPixelsCopied);

	virtual void setProjectiveTextureMatrices(const float viewMatrix[16], const float projectionMatrix[16]);
	virtual void setProjectiveTexture(bool useProjectiveTexture);

	virtual void autogenerateGraphicsObjects(btDynamicsWorld* rbWorld);

	virtual void drawText3D(const char* txt, float position[3], float orientation[4], const Colorf& color, float size, int optionFlag);

	virtual void drawText3D(const char* txt, float posX, float posY, float posZ, float size);

	virtual int addUserDebugText3D(const char* txt, const double positionXYZ[3], const float textColorRGB[3], double size, double lifeTime)
	{
		return -1;
	}

	virtual int addUserDebugLine(const double debugLineFromXYZ[3], const double debugLineToXYZ[3], const Colorf& debugLineColorRGB,
        double lineWidth, double lifeTime, int trackingVisualShapeIndex, int replaceItemUid)
	{
		return -1;
	}
	virtual int addUserDebugParameter(const char* txt, double rangeMin, double rangeMax, double startValue)
	{
		return -1;
	}

	virtual void removeUserDebugItem(int debugItemUniqueId)
	{
	}
	virtual void removeAllUserDebugItems()
	{
	}

	void renderInternalGl2(int pass, const btDynamicsWorld* dynamicsWorld);

	void setVRMode(bool vrMode);

	void setVisualizerFlag(int flag, int enable);

	virtual void setVisualizerFlagCallback(VisualizerFlagCallback callback);

	virtual void dumpFramesToVideo(const char* mp4FileName);

	int createCheckeredTexture(int r, int g, int b);

	void computeSoftBodyVertices(btCollisionShape* collisionShape,
								 b3AlignedObjectArray<GLInstanceVertex>& gfxVertices,
								 b3AlignedObjectArray<int>& indices);

     
};

#endif  //OPENGL_GUI_HELPER_H
