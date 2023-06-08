#ifndef COMMON_RENDER_INTERFACE_H
#define COMMON_RENDER_INTERFACE_H

struct CommonCameraInterface;
#include "Bullet3Common/Color.h"
enum
{
	B3_GL_TRIANGLES = 1,
	B3_GL_POINTS,
	B3_GL_LINES
};

enum
{
	B3_INSTANCE_TRANSPARANCY = 1,
	B3_INSTANCE_TEXTURE = 2,
	B3_INSTANCE_DOUBLE_SIDED = 4,
};

enum
{
	B3_DEFAULT_RENDERMODE = 1,
	//B3_WIREFRAME_RENDERMODE,
	B3_CREATE_SHADOWMAP_RENDERMODE,
	B3_USE_SHADOWMAP_RENDERMODE,
	B3_USE_SHADOWMAP_RENDERMODE_REFLECTION,
	B3_USE_SHADOWMAP_RENDERMODE_REFLECTION_PLANE,
	B3_USE_PROJECTIVE_TEXTURE_RENDERMODE,
	B3_SEGMENTATION_MASK_RENDERMODE,
};

struct CommonRenderInterface
{
	virtual ~CommonRenderInterface() {}
	virtual void init() = 0;
	virtual void updateCamera(int upAxis) = 0;
	virtual void removeAllInstances() = 0;
	virtual void removeGraphicsInstance(int instanceUid) = 0;

	virtual const CommonCameraInterface* getActiveCamera() const = 0;
	virtual CommonCameraInterface* getActiveCamera() = 0;
	virtual void setActiveCamera(CommonCameraInterface* cam) = 0;

	virtual void setLightPosition(const float lightPos[3]) = 0;
	//virtual void setLightPosition(const double lightPos[3]) = 0;
	virtual void setShadowMapResolution(int shadowMapResolution) = 0;
	virtual void setShadowMapWorldSize(float worldSize) = 0;
	
	virtual void setProjectiveTextureMatrices(const float viewMatrix[16], const float projectionMatrix[16]){};
	virtual void setProjectiveTexture(bool useProjectiveTexture){};

	virtual void renderScene() = 0;
	virtual void renderSceneInternal(int renderMode = B3_DEFAULT_RENDERMODE){};
	virtual int getScreenWidth() = 0;
	virtual int getScreenHeight() = 0;

	virtual void resize(int width, int height) = 0;

	virtual int registerGraphicsInstance(int shapeIndex, const float* position, const float* quaternion, const float* color, const float* scaling) = 0;
	//virtual int registerGraphicsInstance(int shapeIndex, const double* position, const double* quaternion, const double* color, const double* scaling) = 0;
	virtual void drawLines(const float* positions, const Colorf& color, int numPoints, int pointStrideInBytes, const unsigned int* indices, int numIndices, float pointDrawSize) = 0;
	virtual void drawLine(const float from[4], const float to[4], const Colorf& color, float lineWidth) = 0;
	//virtual void drawLine(const double from[4], const double to[4], const double color[4], double lineWidth) = 0;
	virtual void drawPoint(const float* position, const Colorf& color, float pointDrawSize) = 0;
	//virtual void drawPoint(const double* position, const double color[4], double pointDrawSize) = 0;
	virtual void drawTexturedTriangleMesh(float worldPosition[3], float worldOrientation[4], const float* vertices, int numvertices, 
        const unsigned int* indices, int numIndices, const Colorf& color, int textureIndex = -1, int vertexLayout = 0) = 0;

	virtual int registerShape(const float* vertices, int numvertices, const int* indices, int numIndices, int primitiveType = B3_GL_TRIANGLES, int textureIndex = -1) = 0;
	virtual void updateShape(int shapeIndex, const float* vertices) = 0;

	virtual int registerTexture(const unsigned char* texels, int width, int height, bool flipPixelsY = true) = 0;
	virtual void updateTexture(int textureIndex, const unsigned char* texels, bool flipPixelsY = true) = 0;
	virtual void activateTexture(int textureIndex) = 0;
	virtual void replaceTexture(int shapeIndex, int textureIndex){};
	virtual void removeTexture(int textureIndex) = 0;

	virtual void setPlaneReflectionShapeIndex(int index) {}

	virtual int getShapeIndexFromInstance(int srcIndex) { return -1; }

	virtual bool readSingleInstanceTransformToCPU(float* position, float* orientation, int srcIndex) = 0;

	virtual void writeSingleInstanceTransformToCPU(const float* position, const float* orientation, int srcIndex) = 0;
	//virtual void writeSingleInstanceTransformToCPU(const double* position, const double* orientation, int srcIndex) = 0;
	virtual void writeSingleInstanceColorToCPU(const float* color, int srcIndex) = 0;
	//virtual void writeSingleInstanceColorToCPU(const double* color, int srcIndex) = 0;
	virtual void writeSingleInstanceScaleToCPU(const float* scale, int srcIndex) = 0;
	//virtual void writeSingleInstanceScaleToCPU(const double* scale, int srcIndex) = 0;
    virtual void writeSingleInstanceSpecularColorToCPU(const float* specular, int srcIndex) = 0;
	//virtual void writeSingleInstanceSpecularColorToCPU(const double* specular, int srcIndex) = 0;
	virtual void writeSingleInstanceFlagsToCPU(int flags, int srcIndex) = 0;
	
	virtual int getTotalNumInstances() const = 0;

	virtual void writeTransforms() = 0;

	virtual void clearZBuffer() = 0;

	//This is internal access to OpenGL3+ features, mainly used for OpenCL-OpenGL interop
	//Only the GLInstancingRenderer supports it, just return 0 otherwise.
	virtual struct GLInstanceRendererInternalData* getInternalData() = 0;
};


#endif  //COMMON_RENDER_INTERFACE_H
