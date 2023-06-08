#if 1
#include "GLPrimitiveRenderer.h"
#include "GLPrimInternalData.h"
#include "Bullet3Common/b3Scalar.h"
#include "LoadShader.h"
#include "OpenGLInclude.h"

static const char *vertexShader3D =
	"#version 150   \n"
	"\n"
	"uniform mat4 ModelViewMatrix, ProjectionMatrix;\n"
	"in vec4 position;\n"
	"in vec4 colour;\n"
	"out vec4 colourV;\n"
	"\n"
	"in vec2 texuv;\n"
	"out vec2 texuvV;\n"
	"\n"
	"\n"
	"void main (void)\n"
	"{\n"
	"    colourV = colour;\n"
	"   gl_Position = ProjectionMatrix * ModelViewMatrix * position ;\n"
	"	texuvV=texuv;\n"
	"}\n";

static const char *fragmentShader3D =
	"#version 150\n"
	"\n"
	"uniform vec2 p;\n"
	"in vec4 colourV;\n"
	"out vec4 fragColour;\n"
	"in vec2 texuvV;\n"
	"\n"
	"uniform sampler2D Diffuse;\n"
	"\n"
	"void main(void)\n"
	"{\n"
	"	vec4 texcolor = texture(Diffuse,texuvV);\n"
	"  if (p.x==0.f)\n"
	"  {\n"
	"		texcolor = vec4(1,1,1,texcolor.x);\n"
	"  }\n"
	"   fragColour = colourV*texcolor;\n"
	"}\n";

static unsigned int s_indexData[6] = {0, 1, 2, 0, 2, 3};
#define MAX_VERTICES2 8192
struct PrimInternalData2
{
	PrimInternalData2()
		: m_numVerticesText(0),
		  m_numVerticesRect(0)
	{
	}
	int m_numVerticesText;
	int m_numVerticesRect;
	PrimVertex m_verticesText[MAX_VERTICES2];
	PrimVertex m_verticesRect[MAX_VERTICES2];
};

GLPrimitiveRenderer::GLPrimitiveRenderer(int screenWidth, int screenHeight)
	: m_screenWidth(screenWidth),
	  m_screenHeight(screenHeight)
{
	m_data = new PrimInternalData;
	m_data2 = new PrimInternalData2;

	m_data->create(vertexShader3D, fragmentShader3D);
 
	m_data->m_positionUniform = m_data->getLoc("p");
 
	m_data->m_colourAttribute =  m_data->getAttribLoc( "colour");
 
	m_data->m_positionAttribute =  m_data->getAttribLoc( "position");
 
	m_data->m_textureAttribute =  m_data->getAttribLoc( "texuv");
 
	loadBufferData();
}

void GLPrimitiveRenderer::loadBufferData()
{
	PrimVertex vertexData[4] = {
		PrimVertex(Vec4f(-1, -1, 0.0, 1.0), Vec4f(1.0, 0.0, 0.0, 1.0), Vec2f(0, 0)),
		PrimVertex(Vec4f(-1, 1, 0.0, 1.0), Vec4f(0.0, 1.0, 0.0, 1.0), Vec2f(0, 1)),
		PrimVertex(Vec4f(1, 1, 0.0, 1.0), Vec4f(0.0, 0.0, 1.0, 1.0), Vec2f(1, 1)),
		PrimVertex(Vec4f(1, -1, 0.0, 1.0), Vec4f(1.0, 1.0, 1.0, 1.0), Vec2f(1, 0))};

    m_data->m_buf1.create(0);
    m_data->m_buf1.Bind();

    m_data->m_buf1.toData(4 * sizeof(PrimVertex), vertexData);
    m_data->m_buf1.toElement(6, s_indexData, false);

     m_data->m_buf2.create(MAX_VERTICES2 * sizeof(PrimVertex));
     m_data->m_buf2.Bind();
     m_data->m_buf2.toData(4 * sizeof(PrimVertex), vertexData);
  
	unsigned int indexData[MAX_VERTICES2 * 2];
	int count = 0;
	for (int i = 0; i < MAX_VERTICES2; i += 4)
	{
		indexData[count++] = i;
		indexData[count++] = i + 1;
		indexData[count++] = i + 2;

		indexData[count++] = i;
		indexData[count++] = i + 2;
		indexData[count++] = i + 3;
	}
 
    m_data->m_buf2.toElement(count, indexData, false);
     
    m_data->setVertexAttrib(m_data->m_positionAttribute, 4, sizeof(PrimVertex), -1, (const GLvoid *)0);
    m_data->setVertexAttrib(m_data->m_colourAttribute, 4, sizeof(PrimVertex), -1, (const GLvoid *)sizeof(Vec4f));
    m_data->setVertexAttrib(m_data->m_textureAttribute, 2, sizeof(PrimVertex), -1, (const GLvoid *)(sizeof(Vec4f) + sizeof(Vec4f)));
  
	glActiveTexture(GL_TEXTURE0);

	GLubyte *image = new GLubyte[256 * 256 * 3];
	for (int y = 0; y < 256; ++y)
	{
		//   const int	t=y>>5;
		GLubyte *pi = image + y * 256 * 3;
		for (int x = 0; x < 256; ++x)
		{
			if (x < y)  //x<2||y<2||x>253||y>253)
			{
				pi[0] = 255;
				pi[1] = 0;
				pi[2] = 0;
			}
			else

			{
				pi[0] = 255;
				pi[1] = 255;
				pi[2] = 255;
			}

			pi += 3;
		}
	}

	glGenTextures(1, (GLuint *)&m_data->m_texturehandle);
	glBindTexture(GL_TEXTURE_2D, m_data->m_texturehandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	b3Assert(glGetError() == GL_NO_ERROR);

	delete[] image;
}

GLPrimitiveRenderer::~GLPrimitiveRenderer()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteProgram(m_data->Shader);
	delete m_data;
	delete m_data2;
}

void GLPrimitiveRenderer::drawLine()
{
}

void GLPrimitiveRenderer::drawRect(float x0, float y0, float x1, float y1, const Colorf& color)
{
	b3Assert(glGetError() == GL_NO_ERROR);
	glActiveTexture(GL_TEXTURE0);
	b3Assert(glGetError() == GL_NO_ERROR);

	glBindTexture(GL_TEXTURE_2D, m_data->m_texturehandle);
	b3Assert(glGetError() == GL_NO_ERROR);
	drawTexturedRect(x0, y0, x1, y1, color, 0, 0, 1, 1, false);
	b3Assert(glGetError() == GL_NO_ERROR);
}

void GLPrimitiveRenderer::drawTexturedRect3D(const PrimVertex &v0, const PrimVertex &v1, const PrimVertex &v2, const PrimVertex &v3, float viewMat[16], float projMat[16], bool useRGBA)
{
	//B3_PROFILE("GLPrimitiveRenderer::drawTexturedRect3D");

	b3Assert(glGetError() == GL_NO_ERROR);

    m_data->use();

    m_data->setUniformMatrix4(m_data->ModelViewMatrix,   viewMat);
    m_data->setUniformMatrix4(m_data->ProjectionMatrix,   projMat);

	b3Assert(glGetError() == GL_NO_ERROR);
    m_data->m_buf1.Bind();


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

	PrimVertex vertexData[4] = {
		v0, v1, v2, v3};
     
    m_data->m_buf1.toData(4 * sizeof(PrimVertex), vertexData);
	b3Assert(glGetError() == GL_NO_ERROR);

	Vec2f p(0.f, 0.f);  //?b?0.5f * sinf(timeValue), 0.5f * cosf(timeValue) );
	if (useRGBA)
	{
		p[0] = 1.f;
		p[1] = 1.f;
	}

    m_data->setUniform(m_data->m_positionUniform, p);

	//??glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	b3Assert(glGetError() == GL_NO_ERROR);


    m_data->setVertexAttrib(m_data->m_positionAttribute, 4, sizeof(PrimVertex), -1, (const GLvoid *)0);
    m_data->setVertexAttrib(m_data->m_colourAttribute, 4, sizeof(PrimVertex), -1, (const GLvoid *)sizeof(Vec4f));
    m_data->setVertexAttrib(m_data->m_textureAttribute, 2, sizeof(PrimVertex), -1, (const GLvoid *)(sizeof(Vec4f) + sizeof(Vec4f)));
      
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	int indexCount = 6;
	b3Assert(glGetError() == GL_NO_ERROR);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	b3Assert(glGetError() == GL_NO_ERROR);

    m_data->m_buf1.unBind();

	//glDisableVertexAttribArray(m_data->m_textureAttribute);
	b3Assert(glGetError() == GL_NO_ERROR);

    m_data->unuse();

	b3Assert(glGetError() == GL_NO_ERROR);
}

void GLPrimitiveRenderer::drawTexturedRect3D2Text(bool useRGBA)
{
	drawTexturedRect3D2(&m_data2->m_verticesText[0], m_data2->m_numVerticesText, useRGBA);
	m_data2->m_numVerticesText = 0;
}

void GLPrimitiveRenderer::drawTexturedRect3D2(PrimVertex *vertices, int numVertices, bool useRGBA)
{
	//B3_PROFILE("drawTexturedRect3D2");
	if (numVertices == 0)
	{
		return;
	}
	//B3_PROFILE("GLPrimitiveRenderer::drawTexturedRect3D");

	b3Assert(glGetError() == GL_NO_ERROR);
	float identity[16] = {1, 0, 0, 0,
						  0, 1, 0, 0,
						  0, 0, 1, 0,
						  0, 0, 0, 1};

	    m_data->use();

        m_data->setUniformMatrix4(m_data->ModelViewMatrix,  identity);
        m_data->setUniformMatrix4(m_data->ProjectionMatrix, identity);

	b3Assert(glGetError() == GL_NO_ERROR);

 
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

	/*   PrimVertex vertexData[4] = {
		   v0,v1,v2,v3
		};
    */
    m_data->m_buf2.Bind();
    m_data->m_buf2.toData(numVertices * sizeof(PrimVertex), vertices);
 
	b3Assert(glGetError() == GL_NO_ERROR);

	Vec2f p(0.f, 0.f);  //?b?0.5f * sinf(timeValue), 0.5f * cosf(timeValue) );
	if (useRGBA)
	{
		p[0] = 1.f;
		p[1] = 1.f;
	}

    m_data->setUniform(m_data->m_positionUniform, p);

	//??glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	b3Assert(glGetError() == GL_NO_ERROR);

    m_data->setVertexAttrib(m_data->m_positionAttribute, 4, sizeof(PrimVertex), -1, (const GLvoid *)0);
    m_data->setVertexAttrib(m_data->m_colourAttribute, 4, sizeof(PrimVertex), -1, (const GLvoid *)sizeof(Vec4f));
    m_data->setVertexAttrib(m_data->m_textureAttribute, 2, sizeof(PrimVertex), -1, (const GLvoid *)(sizeof(Vec4f) + sizeof(Vec4f)));
  
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	int indexCount = (numVertices / 4) * 6;
 
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	b3Assert(glGetError() == GL_NO_ERROR);

    m_data->m_buf2.unBind();
	//glDisableVertexAttribArray(m_data->m_textureAttribute);
	b3Assert(glGetError() == GL_NO_ERROR);

	glUseProgram(0);

	b3Assert(glGetError() == GL_NO_ERROR);
}

void GLPrimitiveRenderer::drawTexturedRect2a(float x0, float y0, float x1, float y1, const Colorf& color, float u0, float v0, float u1, float v1, bool useRGBA)
{
	PrimVertex vertexData[4] = {
		PrimVertex(Vec4f(-1.f + 2.f * x0 / float(m_screenWidth), 1.f - 2.f * y0 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u0, v0)),
		PrimVertex(Vec4f(-1.f + 2.f * x0 / float(m_screenWidth), 1.f - 2.f * y1 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u0, v1)),
		PrimVertex(Vec4f(-1.f + 2.f * x1 / float(m_screenWidth), 1.f - 2.f * y1 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u1, v1)),
		PrimVertex(Vec4f(-1.f + 2.f * x1 / float(m_screenWidth), 1.f - 2.f * y0 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u1, v0))};

	//	int sz = m_data2->m_numVerticesText;

	m_data2->m_verticesRect[m_data2->m_numVerticesRect++] = vertexData[0];
	m_data2->m_verticesRect[m_data2->m_numVerticesRect++] = vertexData[1];
	m_data2->m_verticesRect[m_data2->m_numVerticesRect++] = vertexData[2];
	m_data2->m_verticesRect[m_data2->m_numVerticesRect++] = vertexData[3];

	if (m_data2->m_numVerticesRect >= MAX_VERTICES2)
	{
		flushBatchedRects();
	}
}

void GLPrimitiveRenderer::flushBatchedRects()
{
	if (m_data2->m_numVerticesRect == 0)
		return;

	glActiveTexture(GL_TEXTURE0);
	b3Assert(glGetError() == GL_NO_ERROR);
	glBindTexture(GL_TEXTURE_2D, m_data->m_texturehandle);
	drawTexturedRect3D2(m_data2->m_verticesRect, m_data2->m_numVerticesRect, 0);
	m_data2->m_numVerticesRect = 0;
}
void GLPrimitiveRenderer::drawTexturedRect2(float x0, float y0, float x1, float y1, const Colorf& color, float u0, float v0, float u1, float v1, bool useRGBA)
{
	PrimVertex vertexData[4] = {
		PrimVertex(Vec4f(-1.f + 2.f * x0 / float(m_screenWidth), 1.f - 2.f * y0 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u0, v0)),
		PrimVertex(Vec4f(-1.f + 2.f * x0 / float(m_screenWidth), 1.f - 2.f * y1 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u0, v1)),
		PrimVertex(Vec4f(-1.f + 2.f * x1 / float(m_screenWidth), 1.f - 2.f * y1 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u1, v1)),
		PrimVertex(Vec4f(-1.f + 2.f * x1 / float(m_screenWidth), 1.f - 2.f * y0 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u1, v0))};

	//	int sz = m_data2->m_numVerticesText;

	m_data2->m_verticesText[m_data2->m_numVerticesText++] = vertexData[0];
	m_data2->m_verticesText[m_data2->m_numVerticesText++] = vertexData[1];
	m_data2->m_verticesText[m_data2->m_numVerticesText++] = vertexData[2];
	m_data2->m_verticesText[m_data2->m_numVerticesText++] = vertexData[3];

	if (m_data2->m_numVerticesText >= MAX_VERTICES2)
	{
		drawTexturedRect3D2(m_data2->m_verticesText, m_data2->m_numVerticesText, useRGBA);
		m_data2->m_numVerticesText = 0;
	}
}

void GLPrimitiveRenderer::drawTexturedRect(float x0, float y0, float x1, float y1, const Colorf& color, float u0, float v0, float u1, float v1, bool useRGBA)
{
	float identity[16] = {1, 0, 0, 0,
						  0, 1, 0, 0,
						  0, 0, 1, 0,
						  0, 0, 0, 1};
	PrimVertex vertexData[4] = {
		PrimVertex(Vec4f(-1.f + 2.f * x0 / float(m_screenWidth), 1.f - 2.f * y0 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u0, v0)),
		PrimVertex(Vec4f(-1.f + 2.f * x0 / float(m_screenWidth), 1.f - 2.f * y1 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u0, v1)),
		PrimVertex(Vec4f(-1.f + 2.f * x1 / float(m_screenWidth), 1.f - 2.f * y1 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u1, v1)),
		PrimVertex(Vec4f(-1.f + 2.f * x1 / float(m_screenWidth), 1.f - 2.f * y0 / float(m_screenHeight), 0.f, 1.f), Vec4f(color[0], color[1], color[2], color[3]), Vec2f(u1, v0))};

	drawTexturedRect3D(vertexData[0], vertexData[1], vertexData[2], vertexData[3], identity, identity, useRGBA);
}

void GLPrimitiveRenderer::setScreenSize(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
}
#endif
