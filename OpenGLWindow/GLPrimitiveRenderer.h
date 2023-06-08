#ifndef _GL_PRIMITIVE_RENDERER_H
#define _GL_PRIMITIVE_RENDERER_H

#include "Bullet3Common/PrimVertex.h"
#include "Bullet3Common/Color.h"
 
class GLPrimitiveRenderer
{
	int m_screenWidth;
	int m_screenHeight;

	struct PrimInternalData* m_data;
	struct PrimInternalData2* m_data2;
	void loadBufferData();

public:
	GLPrimitiveRenderer(int screenWidth, int screenHeight);
	virtual ~GLPrimitiveRenderer();

	void drawRect(float x0, float y0, float x1, float y1, const Colorf&  color);
	void drawTexturedRect(float x0, float y0, float x1, float y1, const Colorf&  color, float u0, float v0, float u1, float v1, bool useRGBA );
	void drawTexturedRect3D(const PrimVertex& v0, const PrimVertex& v1, const PrimVertex& v2, const PrimVertex& v3, float viewMat[16], float projMat[16], bool useRGBA);
	void drawLine();  //float from[4], float to[4], const Colorf&  color);
	void setScreenSize(int width, int height);

	void drawTexturedRect2(float x0, float y0, float x1, float y1, const Colorf&  color, float u0, float v0, float u1, float v1, bool useRGBA );
	void drawTexturedRect2a(float x0, float y0, float x1, float y1, const Colorf&  color, float u0, float v0, float u1, float v1, bool useRGBA );
	void flushBatchedRects();

	void drawTexturedRect3D2Text(bool useRGBA = true);
	void drawTexturedRect3D2(PrimVertex* vertices, int numVertices, bool useRGBA = true);

	PrimInternalData* getData()
	{
		return m_data;
	}
};

#endif  //_GL_PRIMITIVE_RENDERER_H
