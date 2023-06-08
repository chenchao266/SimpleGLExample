#if 1
#include "fontstashcallbacks.h"
#include "OpenGLWindow/GLPrimitiveRenderer.h"
#include "OpenGLWindow/GLPrimInternalData.h"

#include "fontstash.h"
#include "OpenGLWindow/OpenGLInclude.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_image/stb_image_write.h"

static unsigned int s_indexData[INDEX_COUNT];

 
OpenGL2RenderCallbacks::OpenGL2RenderCallbacks(GLPrimitiveRenderer* primRender)
	: m_primRender2(primRender)
{
}
OpenGL2RenderCallbacks::~OpenGL2RenderCallbacks()
{
}

PrimInternalData* OpenGL2RenderCallbacks::getData()
{
	return m_primRender2->getData();
}
InternalOpenGL2RenderCallbacks::~InternalOpenGL2RenderCallbacks()
{
}

void InternalOpenGL2RenderCallbacks::display2()
{
	assert(glGetError() == GL_NO_ERROR);
	// glViewport(0,0,10,10);

	//const float timeScale = 0.008f;
	PrimInternalData* data = getData();

    data->use();
    s_buf.Bind();

	assert(glGetError() == GL_NO_ERROR);

	//   glBindTexture(GL_TEXTURE_2D,m_texturehandle);

	assert(glGetError() == GL_NO_ERROR);
	float identity[16] = {1, 0, 0, 0,
						  0, 1, 0, 0,
						  0, 0, 1, 0,
						  0, 0, 0, 1};
    data->setUniformMatrix4(data->ModelViewMatrix, identity);
    data->setUniformMatrix4(data->ProjectionMatrix, identity);

    Vec2f p(0.f, 0.f);  //?b?0.5f * sinf(timeValue), 0.5f * cosf(timeValue) );
    data->setUniform(data->m_positionUniform, p);

	assert(glGetError() == GL_NO_ERROR);

    data->setVertexAttrib(data->m_positionAttribute, 4, sizeof(PrimVertex), -1, (const GLvoid *)0);
    data->setVertexAttrib(data->m_colourAttribute, 4, sizeof(PrimVertex), -1, (const GLvoid *)sizeof(Vec4f));
    data->setVertexAttrib(data->m_textureAttribute, 2, sizeof(PrimVertex), -1, (const GLvoid *)(sizeof(Vec4f) + sizeof(Vec4f)));
  
}

void InternalOpenGL2RenderCallbacks::updateTexture(sth_texture* texture, sth_glyph* glyph, int textureWidth, int textureHeight)
{
	assert(glGetError() == GL_NO_ERROR);

	if (glyph)
	{
		// Update texture (entire texture, could use glyph to update partial texture using glTexSubImage2D)
	 
		glBindTexture(GL_TEXTURE_2D, texture->m_texId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		assert(glGetError() == GL_NO_ERROR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, texture->m_texels);

		assert(glGetError() == GL_NO_ERROR);
	}
	else
	{
		if (textureWidth && textureHeight)
		{ 
			//create new texture
			glGenTextures(1, &texture->m_texId);
			assert(glGetError() == GL_NO_ERROR);

			glBindTexture(GL_TEXTURE_2D, texture->m_texId);
			texture->m_texels = (unsigned char*)malloc(textureWidth * textureHeight);
			memset(texture->m_texels, 0, textureWidth * textureHeight);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, texture->m_texels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			assert(glGetError() == GL_NO_ERROR);

			////////////////////////////
			//create the other data
			{
                s_buf.create(0);
                s_buf.Bind();
                s_buf.toData(VERT_COUNT * sizeof(PrimVertex), texture->newverts);
                 
				for (int i = 0; i < INDEX_COUNT; i++)
				{
					s_indexData[i] = i;
				}
                s_buf.toElement(INDEX_COUNT, s_indexData, false);
			 
			}
		}
		else
		{
			//delete texture
			if (texture->m_texId)
			{ 
				glDeleteTextures(1, &texture->m_texId);
			 
				texture->m_texId = 0;
			}
		}
	}
}

void InternalOpenGL2RenderCallbacks::render(sth_texture* texture)
{
	display2();
 
	assert(glGetError() == GL_NO_ERROR);

	glActiveTexture(GL_TEXTURE0);
	assert(glGetError() == GL_NO_ERROR);

	glBindTexture(GL_TEXTURE_2D, texture->m_texId);
	bool useFiltering = false;
	if (useFiltering)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	assert(glGetError() == GL_NO_ERROR);
    s_buf.Bind();
    s_buf.toData(texture->nverts * sizeof(PrimVertex), &texture->newverts[0].position[0]);
 
	assert(glGetError() == GL_NO_ERROR);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_buf.IndexVbo);

	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	int indexCount = texture->nverts;
	assert(glGetError() == GL_NO_ERROR);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	assert(glGetError() == GL_NO_ERROR);

    s_buf.unBind();
	//	 glDisableVertexAttribArray(m_textureAttribute);
	glUseProgram(0);
}

void dumpTextureToPng(int textureWidth, int textureHeight, const char* fileName)
{
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	unsigned char* pixels = (unsigned char*)malloc(textureWidth * textureHeight);
	glReadPixels(0, 0, textureWidth, textureHeight, GL_RED, GL_UNSIGNED_BYTE, pixels);
	//swap the pixels
	unsigned char* tmp = (unsigned char*)malloc(textureWidth);
	for (int j = 0; j < textureHeight; j++)
	{
		pixels[j * textureWidth + j] = 255;
	}
	if (0)
	{
		for (int j = 0; j < textureHeight / 2; j++)
		{
			for (int i = 0; i < textureWidth; i++)
			{
				tmp[i] = pixels[j * textureWidth + i];
				pixels[j * textureWidth + i] = pixels[(textureHeight - j - 1) * textureWidth + i];
				pixels[(textureHeight - j - 1) * textureWidth + i] = tmp[i];
			}
		}
	}

	int comp = 1;  //1=Y
	stbi_write_png(fileName, textureWidth, textureHeight, comp, pixels, textureWidth);

	free(pixels);
}
#endif
