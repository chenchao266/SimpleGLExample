#ifndef _OPENGL_FONTSTASH_CALLBACKS_H
#define _OPENGL_FONTSTASH_CALLBACKS_H

#include "fontstash.h"
#include "LoadShader.h"
struct PrimInternalData;
class GLPrimitiveRenderer;

struct InternalOpenGL2RenderCallbacks : public RenderCallbacks
{
	virtual PrimInternalData* getData() = 0;

	virtual ~InternalOpenGL2RenderCallbacks();

	virtual void updateTexture(sth_texture* texture, sth_glyph* glyph, int textureWidth, int textureHeight);
	virtual void render(sth_texture* texture);

	void display2();
    BufferObject s_buf;
};

void dumpTextureToPng(int screenWidth, int screenHeight, const char* fileName);
 
struct OpenGL2RenderCallbacks : public InternalOpenGL2RenderCallbacks
{
	GLPrimitiveRenderer* m_primRender2;
	virtual PrimInternalData* getData();

	virtual void setWorldPosition(float pos[3]) {}
	virtual void setWorldOrientation(float orn[4]) {}
	virtual void setColorRGBA(const Colorf& color) {}

	OpenGL2RenderCallbacks(GLPrimitiveRenderer* primRender);
	virtual ~OpenGL2RenderCallbacks();
};

#endif  //_OPENGL_FONTSTASH_CALLBACKS_H
