#ifndef SIMPLE_OPENGL3_APP_H
#define SIMPLE_OPENGL3_APP_H

#include "OpenGLWindow/GLInstancingRenderer.h"
#include "OpenGLWindow/GLPrimitiveRenderer.h"
#include "CommonInterfaces/CommonWindowInterface.h"

#include "CommonInterfaces/CommonGraphicsAppInterface.h"

struct SimpleOpenGL3App : public CommonGraphicsApp
{
	struct SimpleInternalData* m_data;
    static SimpleOpenGL3App* instance();
	class GLPrimitiveRenderer* m_primRenderer;
	class GLInstancingRenderer* m_instancingRenderer;
	virtual void setBackgroundColor(float red, float green, float blue);

	SimpleOpenGL3App(const char* title, int width, int height, bool allowRetina = true, int windowType = 0, int renderDevice = -1, int maxNumObjectCapacity = 128 * 1024, int maxShapeCapacityInBytes = 128 * 1024 * 1024);

	virtual ~SimpleOpenGL3App();

	virtual int registerCubeShape(float halfExtentsX = 1.f, float halfExtentsY = 1.f, float halfExtentsZ = 1.f, int textureIndex = -1, float textureScaling = 1);
	virtual int registerGraphicsUnitSphereShape(EnumSphereLevelOfDetail lod, int textureId = -1);
	virtual void registerGrid(int xres, int yres, const Colorf& color0, const Colorf& color1);
	void dumpNextFrameToPng(const char* pngFilename);
	void dumpFramesToVideo(const char* mp4Filename);
	virtual void getScreenPixels(unsigned char* rgbaBuffer, int bufferSizeInBytes, float* depthBuffer, int depthBufferSizeInBytes);
	virtual void setViewport(int width, int height);

	void drawGrid(DrawGridData data = DrawGridData());
	virtual void setUpAxis(int axis);
	virtual int getUpAxis() const;

	virtual void swapBuffer();
	virtual void drawText(const char* txt, int posX, int posY, float size, const Colorf& colorRGBA);
	virtual void drawText3D(const char* txt, float posX, float posZY, float posZ, float size);
	virtual void drawText3D(const char* txt, float position[3], float orientation[4], const Colorf& color, float size, int optionFlag);

	virtual void drawTexturedRect(float x0, float y0, float x1, float y1, const Colorf& color, float u0, float v0, float u1, float v1, int useRGBA);
	struct sth_stash* getFontStash();
};

#endif  //SIMPLE_OPENGL3_APP_H
