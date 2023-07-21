#if 1
/*
Copyright (c) 2012 Advanced Micro Devices, Inc.

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
//Originally written by Erwin Coumans

///todo: make this configurable in the gui
bool useShadowMap = true;  // true;//false;//true;


#include <stdio.h>
#include "Bullet3Common/PrimVertex.h"
#include "OpenGLWindow/OpenGLHelper.h"

#define MAX_POINTS_IN_BATCH 1024
#define MAX_LINES_IN_BATCH 1024
#define MAX_TRIANGLES_IN_BATCH 8192

#include "OpenGLInclude.h"
#include "CommonInterfaces/CommonWindowInterface.h"
//#include "Bullet3Common/b3MinMax.h"
 
#include "GLInstancingRenderer.h"

#include <string.h>
//#include "DemoSettings.h"
#include <stdio.h>

#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3Transform.h"
#include "Bullet3Common/b3Matrix3x3.h"
#include "Bullet3Common/b3ResizablePool.h"

#include "LoadShader.h"

#include "GLInstanceRendererInternalData.h"

//GLSL shader strings, embedded using build3/stringify
#include "Shaders/pointSpriteVS.h"
#include "Shaders/pointSpritePS.h"
#include "Shaders/instancingVS.h"
#include "Shaders/instancingPS.h"
#include "Shaders/createShadowMapInstancingVS.h"
#include "Shaders/createShadowMapInstancingPS.h"
#include "Shaders/useShadowMapInstancingVS.h"
#include "Shaders/useShadowMapInstancingPS.h"
#include "Shaders/projectiveTextureInstancingVS.h"
#include "Shaders/projectiveTextureInstancingPS.h"

#include "Shaders/segmentationMaskInstancingVS.h"
#include "Shaders/segmentationMaskInstancingPS.h"

#include "Shaders/linesPS.h"
#include "Shaders/linesVS.h"

#include "GLRenderToTexture.h"

#include "stb_image/stb_image_write.h"


static const char* triangleVertexShaderText =
	"#version 330\n"
	"precision highp float;"
	"uniform mat4 MVP;\n"
	"uniform vec3 vCol;\n"
	"layout (location = 0) in vec3 vPos;\n"
	"layout (location = 1) in vec2 vUV;\n"

	"out vec3 clr;\n"
	"out vec2 uv0;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = MVP * vec4(vPos,1);\n"
	"    clr = vCol;\n"
	"    uv0 = vUV;\n"
	"}\n";

static const char* triangleFragmentShader =
	"#version 330\n"
	"precision highp float;"
	"in vec3 clr;\n"
	"in vec2 uv0;"
	"out vec4 color;\n"
	"uniform sampler2D Diffuse;\n"
	"void main()\n"
	"{\n"
	"    vec4 texel = texture(Diffuse,uv0);\n"
	"    color = vec4(clr,texel.r)*texel;\n"
	"}\n";

 


GLint lineWidthRange[2] = {1, 1};



struct b3GraphicsInstance
{
    BufferObject m_buf;
 
	GLuint m_textureIndex;
	int m_numIndices;
	int m_numVertices;

	int m_numGraphicsInstances;
	b3AlignedObjectArray<int> m_tempObjectUids;
	int m_instanceOffset;
	int m_vertexArrayOffset;
	int m_primitiveType;
	float m_materialShinyNess;
	b3Vector3 m_materialSpecularColor;
	int m_flags;  //transparency etc

	b3GraphicsInstance()
		: 
		  m_textureIndex(-1),
		  m_numIndices(-1),
		  m_numVertices(-1),
		  m_numGraphicsInstances(0),
		  m_instanceOffset(0),
		  m_vertexArrayOffset(0),
		  m_primitiveType(B3_GL_TRIANGLES),
		  m_materialShinyNess(41),
		  m_materialSpecularColor(b3MakeVector3(.5, .5, .5)),
		  m_flags(0)
	{
	}
};

bool m_ortho = false;

//static float depthLightModelviewMatrix[16];


extern int gShapeIndex;

struct InternalTextureHandle
{
	GLuint m_glTexture;
	int m_width;
	int m_height;
	int m_enableFiltering;
};

struct b3PublicGraphicsInstanceData
{
	int m_shapeIndex;
	int m_internalInstanceIndex;
	float m_position[4];
	float m_orientation[4];
	float m_color[4];
	float m_scale[4];

	void clear()
	{
	}
};

typedef b3PoolBodyHandle<b3PublicGraphicsInstanceData> b3PublicGraphicsInstance;

struct InternalDataRenderer : public GLInstanceRendererInternalData
{
	SimpleCamera m_defaultCamera1;
	CommonCameraInterface* m_activeCamera;

	float m_projectionMatrix[16];
	float m_viewMatrix[16];
	float m_projectiveTextureProjectionMatrix[16];
	float m_projectiveTextureViewMatrix[16];
	float m_viewMatrixInverse[16];
	bool m_useProjectiveTexture;

	b3Vector3 m_lightPos;
	b3Vector3 m_lightSpecularIntensity;

	GLuint m_defaultTexturehandle;
	b3AlignedObjectArray<InternalTextureHandle> m_textureHandles;

	GLRenderToTexture* m_shadowMap;
	GLuint m_shadowTexture;

	GLuint m_renderFrameBuffer;

	b3ResizablePool<b3PublicGraphicsInstance> m_publicGraphicsInstances;

	int m_shadowMapWidth;
	int m_shadowMapHeight;
	float m_shadowMapWorldSize;
	bool m_updateShadowMap;

	InternalDataRenderer() : m_activeCamera(&m_defaultCamera1),
							 m_shadowMap(0),
							 m_shadowTexture(0),
							 m_renderFrameBuffer(0),
							m_shadowMapWidth(4096),
							m_shadowMapHeight(4096),
							m_shadowMapWorldSize(10),
							m_updateShadowMap(true)

	{
		
		m_lightPos = b3MakeVector3(-50, 30, 40);
		m_lightSpecularIntensity.setValue(1, 1, 1);

		//clear to zero to make it obvious if the matrix is used uninitialized
		for (int i = 0; i < 16; i++)
		{
			m_projectionMatrix[i] = 0;
			m_viewMatrix[i] = 0;
			m_viewMatrixInverse[i] = 0;
			m_projectiveTextureProjectionMatrix[i] = 0;
			m_projectiveTextureViewMatrix[i] = 0;
		}

		m_useProjectiveTexture = false;
	}
};

struct GLInstanceRendererInternalData* GLInstancingRenderer::getInternalData()
{
	return m_data;
}

struct TriangleProgram : public ProgramBase {
 
      GLint mvp_location = -1;
      GLint vpos_location = -1;
      GLint vUV_location = -1;
      GLint vcol_location = -1;
};


struct   CreateShadowMapProgram : public ProgramBase {
  
      GLint  depthMVP = 0;
};

struct   UseShadowMapProgram : public ProgramBase {
 
     GLint ViewMatrixInverse = 0;

     GLint lightSpecularIntensity = 0;
     GLint materialSpecularColor = 0;
     GLint MVP = 0;
     GLint lightPosIn = 0;
     GLint cameraPositionIn = 0;
     GLint materialShininessIn = 0;

    
     GLint DepthBiasModelViewMatrix = 0;
     GLint uniform_texture_diffuse = 0;
     GLint shadowMap = 0;
};
//static bool                 done = false;

struct   LinesProgram : public ProgramBase {
                      // The line renderer

 
      GLint position = 0;
      GLint colour = 0;
};

 
 
struct   ProjectiveTextureProgram : public ProgramBase {
 
     GLint ViewMatrixInverse = 0;
 
     GLint lightSpecularIntensity = 0;
     GLint materialSpecularColor = 0;
     GLint MVP = 0;
     GLint lightPosIn = 0;
     GLint cameraPositionIn = 0;
     GLint materialShininessIn = 0;

 
     GLint TextureMVP = 0;
     GLint uniform_texture_diffuse = 0;
     GLint shadowMap = 0;
};

struct   InstancingProgram : public ProgramBase {
 
 
     GLint uniform_texture_diffuse = 0;
     GLint regularLightDirIn = 0;

};

struct SegmentationMaskProgram : public ProgramBase {
    
 
};

struct   InstancingPointSpriteProgram : public ProgramBase {
 
     GLint screenWidth = 0;
 
    //GLint	uniform_texture_diffusePointSprite= 0;
};



GLInstancingRenderer::GLInstancingRenderer(int maxNumObjectCapacity, int maxShapeCapacityInBytes)
	: m_textureenabled(true),
	  m_textureinitialized(false),
	  m_screenWidth(0),
	  m_screenHeight(0),
	  m_upAxis(1),
	  m_planeReflectionShapeIndex(-1)
{
	m_data = new InternalDataRenderer;
	m_data->m_maxNumObjectCapacity = maxNumObjectCapacity;
	m_data->m_maxShapeCapacityInBytes = maxShapeCapacityInBytes;

	m_data->m_totalNumInstances = 0;

	//sData2 = m_data;

	m_data->m_instance_positions_ptr.resize(m_data->m_maxNumObjectCapacity * 4);
	m_data->m_instance_quaternion_ptr.resize(m_data->m_maxNumObjectCapacity * 4);
	m_data->m_instance_colors_ptr.resize(m_data->m_maxNumObjectCapacity * 4);
	m_data->m_instance_scale_ptr.resize(m_data->m_maxNumObjectCapacity * 4);
}

void GLInstancingRenderer::removeAllInstances()
{
	m_data->m_totalNumInstances = 0;

	for (int i = 0; i < m_graphicsInstances.size(); i++)
	{
		if (m_graphicsInstances[i]->m_buf.IndexVbo)
		{
			glDeleteBuffers(1, &m_graphicsInstances[i]->m_buf.IndexVbo);
		}
		if (m_graphicsInstances[i]->m_buf.VertexArrayObject)
		{
			glDeleteVertexArrays(1, &m_graphicsInstances[i]->m_buf.VertexArrayObject);
		}
		delete m_graphicsInstances[i];
	}
	m_graphicsInstances.clear();
	m_data->m_publicGraphicsInstances.exitHandles();
	m_data->m_publicGraphicsInstances.initHandles();
}

GLInstancingRenderer::~GLInstancingRenderer()
{
	delete m_data->m_shadowMap;
	glDeleteTextures(1, &m_data->m_shadowTexture);
	glDeleteTextures(1, &m_data->m_defaultTexturehandle);

	removeAllInstances();
    for (ProgramBase* prog : programs)
    {
        delete prog;
    }

	//sData2 = 0;

	if (m_data)
	{
		if (m_data->m_vbo)
			glDeleteBuffers(1, &m_data->m_vbo);
	}
	delete m_data;
    m_data = nullptr;
}

int GLInstancingRenderer::getShapeIndexFromInstance(int srcIndex)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex);
	if (pg)
	{
		return pg->m_shapeIndex;
	}
	return -1;
}

bool GLInstancingRenderer::readSingleInstanceTransformToCPU(float* position, float* orientation, int shapeIndex)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(shapeIndex);
	if (pg)
	{
		int srcIndex = pg->m_internalInstanceIndex;

		if ((srcIndex < m_data->m_totalNumInstances) && (srcIndex >= 0))
		{
			position[0] = m_data->m_instance_positions_ptr[srcIndex * 4 + 0];
			position[1] = m_data->m_instance_positions_ptr[srcIndex * 4 + 1];
			position[2] = m_data->m_instance_positions_ptr[srcIndex * 4 + 2];

			orientation[0] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 0];
			orientation[1] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 1];
			orientation[2] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 2];
			orientation[3] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 3];
			return true;
		}
	}

	return false;
}

void GLInstancingRenderer::writeSingleInstanceTransformToCPU(const float* position, const float* orientation, int srcIndex2)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex2);
	b3Assert(pg);

	if (pg == 0)
		return;

	int srcIndex = pg->m_internalInstanceIndex;

	b3Assert(srcIndex < m_data->m_totalNumInstances);
	b3Assert(srcIndex >= 0);
	m_data->m_instance_positions_ptr[srcIndex * 4 + 0] = position[0];
	m_data->m_instance_positions_ptr[srcIndex * 4 + 1] = position[1];
	m_data->m_instance_positions_ptr[srcIndex * 4 + 2] = position[2];
	m_data->m_instance_positions_ptr[srcIndex * 4 + 3] = 1;

	m_data->m_instance_quaternion_ptr[srcIndex * 4 + 0] = orientation[0];
	m_data->m_instance_quaternion_ptr[srcIndex * 4 + 1] = orientation[1];
	m_data->m_instance_quaternion_ptr[srcIndex * 4 + 2] = orientation[2];
	m_data->m_instance_quaternion_ptr[srcIndex * 4 + 3] = orientation[3];
}


void GLInstancingRenderer::readSingleInstanceTransformFromCPU(int srcIndex2, float* position, float* orientation)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex2);
	b3Assert(pg);
	int srcIndex = pg->m_internalInstanceIndex;

	b3Assert(srcIndex < m_data->m_totalNumInstances);
	b3Assert(srcIndex >= 0);
	position[0] = m_data->m_instance_positions_ptr[srcIndex * 4 + 0];
	position[1] = m_data->m_instance_positions_ptr[srcIndex * 4 + 1];
	position[2] = m_data->m_instance_positions_ptr[srcIndex * 4 + 2];

	orientation[0] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 0];
	orientation[1] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 1];
	orientation[2] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 2];
	orientation[3] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 3];
}

void GLInstancingRenderer::writeSingleInstanceFlagsToCPU(int flags, int srcIndex2)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex2);
	b3Assert(pg);
	int srcIndex = pg->m_internalInstanceIndex;

	int shapeIndex = pg->m_shapeIndex;
	b3GraphicsInstance* gfxObj = m_graphicsInstances[shapeIndex];
	if (flags & B3_INSTANCE_DOUBLE_SIDED)
	{
		gfxObj->m_flags |= B3_INSTANCE_DOUBLE_SIDED;
	}
	else
	{
		gfxObj->m_flags &= ~B3_INSTANCE_DOUBLE_SIDED;
	}
}



void GLInstancingRenderer::writeSingleInstanceColorToCPU(const float* color, int srcIndex2)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex2);
	b3Assert(pg);
	int srcIndex = pg->m_internalInstanceIndex;
	int shapeIndex = pg->m_shapeIndex;
	b3GraphicsInstance* gfxObj = m_graphicsInstances[shapeIndex];

	if (color[3] < 1)
	{
		gfxObj->m_flags |= B3_INSTANCE_TRANSPARANCY;
	}
	else
	{
		gfxObj->m_flags &= ~B3_INSTANCE_TRANSPARANCY;
	}

	m_data->m_instance_colors_ptr[srcIndex * 4 + 0] = color[0];
	m_data->m_instance_colors_ptr[srcIndex * 4 + 1] = color[1];
	m_data->m_instance_colors_ptr[srcIndex * 4 + 2] = color[2];
	m_data->m_instance_colors_ptr[srcIndex * 4 + 3] = color[3];
}

void GLInstancingRenderer::writeSingleInstanceScaleToCPU(const float* scale, int srcIndex2)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex2);
	b3Assert(pg);
	int srcIndex = pg->m_internalInstanceIndex;

	m_data->m_instance_scale_ptr[srcIndex * 4 + 0] = scale[0];
	m_data->m_instance_scale_ptr[srcIndex * 4 + 1] = scale[1];
	m_data->m_instance_scale_ptr[srcIndex * 4 + 2] = scale[2];
	caster2 c;
	c.setInt(srcIndex2);
	m_data->m_instance_scale_ptr[srcIndex * 4 + 3] = c.getFloat();
}


void GLInstancingRenderer::writeSingleInstanceSpecularColorToCPU(const float* specular, int srcIndex2)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex2);
	b3Assert(pg);
	int srcIndex = pg->m_internalInstanceIndex;

	int totalNumInstances = 0;

	int gfxObjIndex = -1;

	for (int i = 0; i < m_graphicsInstances.size(); i++)
	{
		totalNumInstances += m_graphicsInstances[i]->m_numGraphicsInstances;
		if (srcIndex2 < totalNumInstances)
		{
			gfxObjIndex = i;
			break;
		}
	}
	if (gfxObjIndex > 0)
	{
		m_graphicsInstances[gfxObjIndex]->m_materialSpecularColor[0] = specular[0];
		m_graphicsInstances[gfxObjIndex]->m_materialSpecularColor[1] = specular[1];
		m_graphicsInstances[gfxObjIndex]->m_materialSpecularColor[2] = specular[2];
	}
}


void GLInstancingRenderer::writeSingleInstanceTransformToGPU(float* position, float* orientation, int objectUniqueId)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vbo);
	//glFlush();

	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(objectUniqueId);
	b3Assert(pg);
	int objectIndex = pg->m_internalInstanceIndex;

	char* orgBase = (char*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	//b3GraphicsInstance* gfxObj = m_graphicsInstances[k];
	int totalNumInstances = 0;
	for (int k = 0; k < m_graphicsInstances.size(); k++)
	{
		b3GraphicsInstance* gfxObj = m_graphicsInstances[k];
		totalNumInstances += gfxObj->m_numGraphicsInstances;
	}

	int POSITION_BUFFER_SIZE = (totalNumInstances * sizeof(float) * 4);

	char* base = orgBase;

	float* positions = (float*)(base + m_data->m_maxShapeCapacityInBytes);
	float* orientations = (float*)(base + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE);

	positions[objectIndex * 4] = position[0];
	positions[objectIndex * 4 + 1] = position[1];
	positions[objectIndex * 4 + 2] = position[2];
	positions[objectIndex * 4 + 3] = position[3];

	orientations[objectIndex * 4] = orientation[0];
	orientations[objectIndex * 4 + 1] = orientation[1];
	orientations[objectIndex * 4 + 2] = orientation[2];
	orientations[objectIndex * 4 + 3] = orientation[3];

	glUnmapBuffer(GL_ARRAY_BUFFER);
	//glFlush();
}

void GLInstancingRenderer::writeTransforms()
{
	{
		//B3_PROFILE("b3Assert(glGetError() 1");
		b3Assert(glGetError() == GL_NO_ERROR);
	}
	{
		//B3_PROFILE("glBindBuffer");
		glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vbo);
	}

	{
		//B3_PROFILE("glFlush()");
		//without the flush, the glBufferSubData can spike to really slow (seconds slow)
		glFlush();
	}

	{
		//B3_PROFILE("b3Assert(glGetError() 2");
		b3Assert(glGetError() == GL_NO_ERROR);
	}

#ifdef B3_DEBUG
	{
		//B3_PROFILE("m_data->m_totalNumInstances == totalNumInstances");

		int totalNumInstances = 0;
		for (int k = 0; k < m_graphicsInstances.size(); k++)
		{
			b3GraphicsInstance* gfxObj = m_graphicsInstances[k];
			totalNumInstances += gfxObj->m_numGraphicsInstances;
		}
		b3Assert(m_data->m_totalNumInstances == totalNumInstances);
	}
#endif  //B3_DEBUG

	int POSITION_BUFFER_SIZE = (m_data->m_totalNumInstances * sizeof(float) * 4);
	int ORIENTATION_BUFFER_SIZE = (m_data->m_totalNumInstances * sizeof(float) * 4);
	int COLOR_BUFFER_SIZE = (m_data->m_totalNumInstances * sizeof(float) * 4);
	//	int SCALE_BUFFER_SIZE = (totalNumInstances*sizeof(float)*4);

#if 1
	{
		//	printf("m_data->m_totalNumInstances = %d\n", m_data->m_totalNumInstances);
		{
			//B3_PROFILE("glBufferSubData pos");
			glBufferSubData(GL_ARRAY_BUFFER, m_data->m_maxShapeCapacityInBytes, m_data->m_totalNumInstances * sizeof(float) * 4,
							&m_data->m_instance_positions_ptr[0]);
		}
		{
			//			B3_PROFILE("glBufferSubData orn");
			glBufferSubData(GL_ARRAY_BUFFER, m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE, m_data->m_totalNumInstances * sizeof(float) * 4,
							&m_data->m_instance_quaternion_ptr[0]);
		}
		{
			//			B3_PROFILE("glBufferSubData color");
			glBufferSubData(GL_ARRAY_BUFFER, m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE, m_data->m_totalNumInstances * sizeof(float) * 4,
							&m_data->m_instance_colors_ptr[0]);
		}
		{
			//			B3_PROFILE("glBufferSubData scale");
			glBufferSubData(GL_ARRAY_BUFFER, m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE + COLOR_BUFFER_SIZE, m_data->m_totalNumInstances * sizeof(float) * 4,
							&m_data->m_instance_scale_ptr[0]);
		}
	}
#else

	char* orgBase = (char*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	if (orgBase)
	{
		for (int k = 0; k < m_graphicsInstances.size(); k++)
		{
			//int k=0;
			b3GraphicsInstance* gfxObj = m_graphicsInstances[k];

			char* base = orgBase;

			float* positions = (float*)(base + m_data->m_maxShapeCapacityInBytes);
			float* orientations = (float*)(base + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE);
			float* colors = (float*)(base + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE);
			float* scaling = (float*)(base + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE + COLOR_BUFFER_SIZE);

			//static int offset=0;
			//offset++;

			for (int i = 0; i < gfxObj->m_numGraphicsInstances; i++)
			{
				int srcIndex = i + gfxObj->m_instanceOffset;

				positions[srcIndex * 4] = m_data->m_instance_positions_ptr[srcIndex * 4];
				positions[srcIndex * 4 + 1] = m_data->m_instance_positions_ptr[srcIndex * 4 + 1];
				positions[srcIndex * 4 + 2] = m_data->m_instance_positions_ptr[srcIndex * 4 + 2];
				positions[srcIndex * 4 + 3] = m_data->m_instance_positions_ptr[srcIndex * 4 + 3];

				orientations[srcIndex * 4] = m_data->m_instance_quaternion_ptr[srcIndex * 4];
				orientations[srcIndex * 4 + 1] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 1];
				orientations[srcIndex * 4 + 2] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 2];
				orientations[srcIndex * 4 + 3] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 3];

				colors[srcIndex * 4] = m_data->m_instance_colors_ptr[srcIndex * 4];
				colors[srcIndex * 4 + 1] = m_data->m_instance_colors_ptr[srcIndex * 4 + 1];
				colors[srcIndex * 4 + 2] = m_data->m_instance_colors_ptr[srcIndex * 4 + 2];
				colors[srcIndex * 4 + 3] = m_data->m_instance_colors_ptr[srcIndex * 4 + 3];

				scaling[srcIndex * 4] = m_data->m_instance_scale_ptr[srcIndex * 4];
				scaling[srcIndex * 4 + 1] = m_data->m_instance_scale_ptr[srcIndex * 4 + 1];
				scaling[srcIndex * 4 + 2] = m_data->m_instance_scale_ptr[srcIndex * 4 + 2];
				scaling[srcIndex * 4 + 3] = m_data->m_instance_scale_ptr[srcIndex * 4 + 3];
			}
		}
	}
	else
	{
		b3Error("ERROR glMapBuffer failed\n");
	}
	b3Assert(glGetError() == GL_NO_ERROR);

	glUnmapBuffer(GL_ARRAY_BUFFER);
	//if this glFinish is removed, the animation is not always working/blocks
	//@todo: figure out why
	//glFlush();

#endif

	{
		//		B3_PROFILE("glBindBuffer 2");
		glBindBuffer(GL_ARRAY_BUFFER, 0);  //m_data->m_vbo);
	}

	{
		//	B3_PROFILE("b3Assert(glGetError() 4");
		b3Assert(glGetError() == GL_NO_ERROR);
	}
}


void GLInstancingRenderer::rebuildGraphicsInstances()
{
	m_data->m_totalNumInstances = 0;

	b3AlignedObjectArray<int> usedObjects;
	m_data->m_publicGraphicsInstances.getUsedHandles(usedObjects);

	for (int i = 0; i < usedObjects.size(); i++)
	{
		int srcIndex2 = usedObjects[i];
		b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex2);
		b3Assert(pg);
		int srcIndex = pg->m_internalInstanceIndex;

		pg->m_position[0] = m_data->m_instance_positions_ptr[srcIndex * 4 + 0];
		pg->m_position[1] = m_data->m_instance_positions_ptr[srcIndex * 4 + 1];
		pg->m_position[2] = m_data->m_instance_positions_ptr[srcIndex * 4 + 2];
		pg->m_orientation[0] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 0];
		pg->m_orientation[1] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 1];
		pg->m_orientation[2] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 2];
		pg->m_orientation[3] = m_data->m_instance_quaternion_ptr[srcIndex * 4 + 3];
		pg->m_color[0] = m_data->m_instance_colors_ptr[srcIndex * 4 + 0];
		pg->m_color[1] = m_data->m_instance_colors_ptr[srcIndex * 4 + 1];
		pg->m_color[2] = m_data->m_instance_colors_ptr[srcIndex * 4 + 2];
		pg->m_color[3] = m_data->m_instance_colors_ptr[srcIndex * 4 + 3];
		pg->m_scale[0] = m_data->m_instance_scale_ptr[srcIndex * 4 + 0];
		pg->m_scale[1] = m_data->m_instance_scale_ptr[srcIndex * 4 + 1];
		pg->m_scale[2] = m_data->m_instance_scale_ptr[srcIndex * 4 + 2];
		pg->m_scale[3] = m_data->m_instance_scale_ptr[srcIndex * 4 + 3];
	}
	for (int i = 0; i < m_graphicsInstances.size(); i++)
	{
		m_graphicsInstances[i]->m_numGraphicsInstances = 0;
		m_graphicsInstances[i]->m_instanceOffset = 0;
		m_graphicsInstances[i]->m_tempObjectUids.clear();
	}
	for (int i = 0; i < usedObjects.size(); i++)
	{
		int srcIndex2 = usedObjects[i];
		b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(srcIndex2);
		if (pg && pg->m_shapeIndex < m_graphicsInstances.size() && pg->m_shapeIndex >= 0)
		{
			m_graphicsInstances[pg->m_shapeIndex]->m_tempObjectUids.push_back(srcIndex2);
		}
	}

	int curOffset = 0;
	m_data->m_totalNumInstances = 0;

	for (int i = 0; i < m_graphicsInstances.size(); i++)
	{
		m_graphicsInstances[i]->m_instanceOffset = curOffset;
		m_graphicsInstances[i]->m_numGraphicsInstances = 0;

		for (int g = 0; g < m_graphicsInstances[i]->m_tempObjectUids.size(); g++)
		{
			curOffset++;
			int objectUniqueId = m_graphicsInstances[i]->m_tempObjectUids[g];
			b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(objectUniqueId);

			registerGraphicsInstanceInternal(objectUniqueId, pg->m_position, pg->m_orientation, pg->m_color, pg->m_scale);
		}
	}
}

void GLInstancingRenderer::removeGraphicsInstance(int instanceUid)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(instanceUid);
	b3Assert(pg);
	if (pg)
	{
		m_data->m_publicGraphicsInstances.freeHandle(instanceUid);
		rebuildGraphicsInstances();
	}
}

int GLInstancingRenderer::registerGraphicsInstanceInternal(int newUid, const float* position, const float* quaternion, const float* color, const float* scaling)
{
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(newUid);
	int shapeIndex = pg->m_shapeIndex;
	//	b3Assert(pg);
	//	int objectIndex = pg->m_internalInstanceIndex;

	b3GraphicsInstance* gfxObj = m_graphicsInstances[shapeIndex];
	int index = gfxObj->m_numGraphicsInstances + gfxObj->m_instanceOffset;
	pg->m_internalInstanceIndex = index;

	int maxElements = m_data->m_instance_positions_ptr.size();
	if (index * 4 < maxElements)
	{
		m_data->m_instance_positions_ptr[index * 4] = position[0];
		m_data->m_instance_positions_ptr[index * 4 + 1] = position[1];
		m_data->m_instance_positions_ptr[index * 4 + 2] = position[2];
		m_data->m_instance_positions_ptr[index * 4 + 3] = 1;

		m_data->m_instance_quaternion_ptr[index * 4] = quaternion[0];
		m_data->m_instance_quaternion_ptr[index * 4 + 1] = quaternion[1];
		m_data->m_instance_quaternion_ptr[index * 4 + 2] = quaternion[2];
		m_data->m_instance_quaternion_ptr[index * 4 + 3] = quaternion[3];

		m_data->m_instance_colors_ptr[index * 4] = color[0];
		m_data->m_instance_colors_ptr[index * 4 + 1] = color[1];
		m_data->m_instance_colors_ptr[index * 4 + 2] = color[2];
		m_data->m_instance_colors_ptr[index * 4 + 3] = color[3];

		m_data->m_instance_scale_ptr[index * 4] = scaling[0];
		m_data->m_instance_scale_ptr[index * 4 + 1] = scaling[1];
		m_data->m_instance_scale_ptr[index * 4 + 2] = scaling[2];
		caster2 c;
		c.setInt(newUid);
		m_data->m_instance_scale_ptr[index * 4 + 3] = c.getFloat();

		if (color[3] < 1 && color[3] > 0)
		{
			gfxObj->m_flags |= B3_INSTANCE_TRANSPARANCY;
		}
		gfxObj->m_numGraphicsInstances++;
		m_data->m_totalNumInstances++;
	}
	else
	{
		b3Error("registerGraphicsInstance out of range, %d\n", maxElements);
		return -1;
	}
	return newUid;  //gfxObj->m_numGraphicsInstances;
}

int GLInstancingRenderer::registerGraphicsInstance(int shapeIndex, const float* position, const float* quaternion, const float* color, const float* scaling)
{
	int newUid = m_data->m_publicGraphicsInstances.allocHandle();
	b3PublicGraphicsInstance* pg = m_data->m_publicGraphicsInstances.getHandle(newUid);
	pg->m_shapeIndex = shapeIndex;

	//b3Assert(shapeIndex == (m_graphicsInstances.size()-1));
	b3Assert(m_graphicsInstances.size() < m_data->m_maxNumObjectCapacity - 1);
	if (shapeIndex == (m_graphicsInstances.size() - 1))
	{
		registerGraphicsInstanceInternal(newUid, position, quaternion, color, scaling);
	}
	else
	{
		int srcIndex = m_data->m_totalNumInstances++;
		pg->m_internalInstanceIndex = srcIndex;

		m_data->m_instance_positions_ptr[srcIndex * 4 + 0] = position[0];
		m_data->m_instance_positions_ptr[srcIndex * 4 + 1] = position[1];
		m_data->m_instance_positions_ptr[srcIndex * 4 + 2] = position[2];
		m_data->m_instance_positions_ptr[srcIndex * 4 + 3] = 1.;

		m_data->m_instance_quaternion_ptr[srcIndex * 4 + 0] = quaternion[0];
		m_data->m_instance_quaternion_ptr[srcIndex * 4 + 1] = quaternion[1];
		m_data->m_instance_quaternion_ptr[srcIndex * 4 + 2] = quaternion[2];
		m_data->m_instance_quaternion_ptr[srcIndex * 4 + 3] = quaternion[3];

		m_data->m_instance_colors_ptr[srcIndex * 4 + 0] = color[0];
		m_data->m_instance_colors_ptr[srcIndex * 4 + 1] = color[1];
		m_data->m_instance_colors_ptr[srcIndex * 4 + 2] = color[2];
		m_data->m_instance_colors_ptr[srcIndex * 4 + 3] = color[3];

		m_data->m_instance_scale_ptr[srcIndex * 4 + 0] = scaling[0];
		m_data->m_instance_scale_ptr[srcIndex * 4 + 1] = scaling[1];
		m_data->m_instance_scale_ptr[srcIndex * 4 + 2] = scaling[2];
		caster2 c;
		c.setInt(newUid);
		m_data->m_instance_scale_ptr[srcIndex * 4 + 3] = c.getFloat();

		rebuildGraphicsInstances();
	}

	return newUid;
}

void GLInstancingRenderer::removeTexture(int textureIndex)
{
	if ((textureIndex >= 0) && (textureIndex < m_data->m_textureHandles.size()))
	{
		InternalTextureHandle& h = m_data->m_textureHandles[textureIndex];
		glDeleteTextures(1, &h.m_glTexture);
	}
}

int GLInstancingRenderer::registerTexture(const unsigned char* texels, int width, int height, bool flipPixelsY)
{
	B3_PROFILE("GLInstancingRenderer::registerTexture");
	b3Assert(glGetError() == GL_NO_ERROR);
	glActiveTexture(GL_TEXTURE0);
	int textureIndex = m_data->m_textureHandles.size();
	//  const GLubyte*	image= (const GLubyte*)texels;
	GLuint textureHandle;
	glGenTextures(1, (GLuint*)&textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	b3Assert(glGetError() == GL_NO_ERROR);

	InternalTextureHandle h;
	h.m_glTexture = textureHandle;
	h.m_width = width;
	h.m_height = height;
	h.m_enableFiltering = true;
	m_data->m_textureHandles.push_back(h);
	if (texels)
	{
		B3_PROFILE("updateTexture");
		updateTexture(textureIndex, texels, flipPixelsY);
	}
	return textureIndex;
}

void GLInstancingRenderer::replaceTexture(int shapeIndex, int textureId)
{
	if ((shapeIndex >= 0) && (shapeIndex < m_graphicsInstances.size()))
	{
		b3GraphicsInstance* gfxObj = m_graphicsInstances[shapeIndex];
		if (textureId >= 0 && textureId < m_data->m_textureHandles.size())
		{
			gfxObj->m_textureIndex = textureId;
			gfxObj->m_flags |= B3_INSTANCE_TEXTURE;
		} else
		{
			gfxObj->m_textureIndex = -1;
			gfxObj->m_flags &= ~B3_INSTANCE_TEXTURE;
		}
	}
}

void GLInstancingRenderer::updateTexture(int textureIndex, const unsigned char* texels, bool flipPixelsY)
{
	B3_PROFILE("updateTexture");
	if ((textureIndex >= 0) && (textureIndex < m_data->m_textureHandles.size()))
	{
		glActiveTexture(GL_TEXTURE0);
		b3Assert(glGetError() == GL_NO_ERROR);
		InternalTextureHandle& h = m_data->m_textureHandles[textureIndex];
		glBindTexture(GL_TEXTURE_2D, h.m_glTexture);
		b3Assert(glGetError() == GL_NO_ERROR);

		if (flipPixelsY)
		{
			B3_PROFILE("flipPixelsY");
			//textures need to be flipped for OpenGL...
			b3AlignedObjectArray<unsigned char> flippedTexels;
			flippedTexels.resize(h.m_width * h.m_height * 3);

			for (int j = 0; j < h.m_height; j++)
			{
				for (int i = 0; i < h.m_width; i++)
				{
					flippedTexels[(i + j * h.m_width) * 3] = texels[(i + (h.m_height - 1 - j) * h.m_width) * 3];
					flippedTexels[(i + j * h.m_width) * 3 + 1] = texels[(i + (h.m_height - 1 - j) * h.m_width) * 3 + 1];
					flippedTexels[(i + j * h.m_width) * 3 + 2] = texels[(i + (h.m_height - 1 - j) * h.m_width) * 3 + 2];
				}
			}

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, h.m_width, h.m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, &flippedTexels[0]);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, h.m_width, h.m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, &texels[0]);
		}
		b3Assert(glGetError() == GL_NO_ERROR);
		if (h.m_enableFiltering)
		{
			B3_PROFILE("glGenerateMipmap");
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		b3Assert(glGetError() == GL_NO_ERROR);
	}
}

void GLInstancingRenderer::activateTexture(int textureIndex)
{
	glActiveTexture(GL_TEXTURE0);

	if (textureIndex >= 0 && textureIndex < m_data->m_textureHandles.size())
	{
		glBindTexture(GL_TEXTURE_2D, m_data->m_textureHandles[textureIndex].m_glTexture);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void GLInstancingRenderer::updateShape(int shapeIndex, const float* vertices)
{
	b3GraphicsInstance* gfxObj = m_graphicsInstances[shapeIndex];
	int numvertices = gfxObj->m_numVertices;

	glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vbo);
	int vertexStrideInBytes = GLInstanceVertex::Length * sizeof(float);
	int sz = numvertices * vertexStrideInBytes;
#if 0
	char* dest=  (char*)glMapBuffer( GL_ARRAY_BUFFER,GL_WRITE_ONLY);//GL_WRITE_ONLY
	memcpy(dest+vertexStrideInBytes*gfxObj->m_vertexArrayOffset,vertices,sz);
	glUnmapBuffer( GL_ARRAY_BUFFER);
#else
	glBufferSubData(GL_ARRAY_BUFFER, vertexStrideInBytes * gfxObj->m_vertexArrayOffset, sz,
					vertices);
#endif
}

int GLInstancingRenderer::registerShape(const float* vertices, int numvertices, const int* indices, int numIndices, int primitiveType, int textureId)
{
	b3GraphicsInstance* gfxObj = new b3GraphicsInstance;

	if (textureId >= 0)
	{
		gfxObj->m_textureIndex = textureId;
		gfxObj->m_flags |= B3_INSTANCE_TEXTURE;
	}

	gfxObj->m_primitiveType = primitiveType;

	if (m_graphicsInstances.size())
	{
		b3GraphicsInstance* prevObj = m_graphicsInstances[m_graphicsInstances.size() - 1];
		gfxObj->m_instanceOffset = prevObj->m_instanceOffset + prevObj->m_numGraphicsInstances;
		gfxObj->m_vertexArrayOffset = prevObj->m_vertexArrayOffset + prevObj->m_numVertices;
	}
	else
	{
		gfxObj->m_instanceOffset = 0;
	}

	m_graphicsInstances.push_back(gfxObj);
	gfxObj->m_numIndices = numIndices;
	gfxObj->m_numVertices = numvertices;

	int vertexStrideInBytes = GLInstanceVertex::Length * sizeof(float);
	int sz = numvertices * vertexStrideInBytes;
	int totalUsed = vertexStrideInBytes * gfxObj->m_vertexArrayOffset + sz;
	b3Assert(totalUsed < m_data->m_maxShapeCapacityInBytes);
	if (totalUsed >= m_data->m_maxShapeCapacityInBytes)
	{
		return -1;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vbo);

#if 0

	char* dest=  (char*)glMapBuffer( GL_ARRAY_BUFFER,GL_WRITE_ONLY);//GL_WRITE_ONLY
 
	memcpy(dest+vertexStrideInBytes*gfxObj->m_vertexArrayOffset,vertices,sz);
	glUnmapBuffer( GL_ARRAY_BUFFER);
#else
	glBufferSubData(GL_ARRAY_BUFFER, vertexStrideInBytes * gfxObj->m_vertexArrayOffset, sz,
					vertices);
#endif

	glGenBuffers(1, &gfxObj->m_buf.IndexVbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gfxObj->m_buf.IndexVbo);
	int indexBufferSizeInBytes = gfxObj->m_numIndices * sizeof(int);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSizeInBytes, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexBufferSizeInBytes, indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &gfxObj->m_buf.VertexArrayObject);
	glBindVertexArray(gfxObj->m_buf.VertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vbo);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return m_graphicsInstances.size() - 1;
}

void GLInstancingRenderer::InitShaders()
{
	int POSITION_BUFFER_SIZE = (m_data->m_maxNumObjectCapacity * sizeof(float) * 4);
	int ORIENTATION_BUFFER_SIZE = (m_data->m_maxNumObjectCapacity * sizeof(float) * 4);
	int COLOR_BUFFER_SIZE = (m_data->m_maxNumObjectCapacity * sizeof(float) * 4);
	int SCALE_BUFFER_SIZE = (m_data->m_maxNumObjectCapacity * sizeof(float) * 4);


    TriangleProgram* triangleProg = new TriangleProgram;                        programs.push_back(triangleProg);
    CreateShadowMapProgram* createShadowMapProg = new CreateShadowMapProgram;   programs.push_back(createShadowMapProg);
    UseShadowMapProgram* useShadowProg = new UseShadowMapProgram;               programs.push_back(useShadowProg);
    LinesProgram* linesProg = new LinesProgram;                                 programs.push_back(linesProg);
    ProjectiveTextureProgram* projectiveTextureProg = new ProjectiveTextureProgram;             programs.push_back(projectiveTextureProg);
    InstancingProgram* instancingProg = new InstancingProgram;                                  programs.push_back(instancingProg);
    SegmentationMaskProgram* segmentationMaskProg = new SegmentationMaskProgram;                programs.push_back(segmentationMaskProg);
    InstancingPointSpriteProgram* instancingPointSpriteProg = new InstancingPointSpriteProgram; programs.push_back(instancingPointSpriteProg);

	{
        triangleProg->create(triangleVertexShaderText, triangleFragmentShader);
		//triangleProg->vpos_location =  triangleProg->getAttribLoc( "vPos");
		//triangleProg->vUV_location = triangleProg->getAttribLoc("vUV");
		triangleProg->mvp_location = triangleProg->getLoc("MVP");
		triangleProg->vcol_location = triangleProg->getLoc("vCol");
		triangleProg->link();
		triangleProg->use();       
        triangleBuf.create(MAX_TRIANGLES_IN_BATCH * sizeof(PrimVertex));
        ProgramBase::unuse();
	}
    linesProg->create(linesVertexShader, linesFragmentShader);
  	linesProg->colour = linesProg->getLoc("colour");
	linesProg->position = linesProg->getAttribLoc("position");
    linesProg->link();
    linesProg->use();
    {
        linesBuf.create(MAX_LINES_IN_BATCH * sizeof(b3Vector3));//??	 
        lineBuf.create(MAX_POINTS_IN_BATCH * sizeof(b3Vector3));
	}
	//glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, range);
	glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, lineWidthRange);
    ProgramBase::unuse();

    projectiveTextureProg->create(projectiveTextureInstancingVertexShader, projectiveTextureInstancingFragmentShader);
	projectiveTextureProg->link();
	projectiveTextureProg->use();
	projectiveTextureProg->ViewMatrixInverse = projectiveTextureProg->getLoc( "ViewMatrixInverse");
    projectiveTextureProg->lightSpecularIntensity = projectiveTextureProg->getLoc( "lightSpecularIntensityIn");
	projectiveTextureProg->materialSpecularColor = projectiveTextureProg->getLoc( "materialSpecularColorIn");
	projectiveTextureProg->MVP = projectiveTextureProg->getLoc( "MVP");
 	projectiveTextureProg->TextureMVP = projectiveTextureProg->getLoc( "TextureMVP");
	projectiveTextureProg->uniform_texture_diffuse = projectiveTextureProg->getLoc( "Diffuse");
	projectiveTextureProg->shadowMap = projectiveTextureProg->getLoc( "shadowMap");
	projectiveTextureProg->lightPosIn = projectiveTextureProg->getLoc( "lightPosIn");
	projectiveTextureProg->cameraPositionIn = projectiveTextureProg->getLoc( "cameraPositionIn");
	projectiveTextureProg->materialShininessIn = projectiveTextureProg->getLoc( "materialShininessIn");

	ProgramBase::unuse();

    useShadowProg->create(useShadowMapInstancingVertexShader, useShadowMapInstancingFragmentShader);
	useShadowProg->link();
	useShadowProg->use();
	useShadowProg->ViewMatrixInverse = useShadowProg->getLoc("ViewMatrixInverse");
	useShadowProg->ModelViewMatrix = useShadowProg->getLoc("ModelViewMatrix");
	useShadowProg->lightSpecularIntensity = useShadowProg->getLoc("lightSpecularIntensityIn");
	useShadowProg->materialSpecularColor = useShadowProg->getLoc("materialSpecularColorIn");
	useShadowProg->MVP = useShadowProg->getLoc("MVP");
	useShadowProg->ProjectionMatrix = useShadowProg->getLoc("ProjectionMatrix");
	useShadowProg->DepthBiasModelViewMatrix = useShadowProg->getLoc("DepthBiasModelViewProjectionMatrix");
	useShadowProg->uniform_texture_diffuse = useShadowProg->getLoc("Diffuse");
	useShadowProg->shadowMap = useShadowProg->getLoc("shadowMap");
	useShadowProg->lightPosIn = useShadowProg->getLoc("lightPosIn");
	useShadowProg->cameraPositionIn = useShadowProg->getLoc("cameraPositionIn");
	useShadowProg->materialShininessIn = useShadowProg->getLoc("materialShininessIn");
    ProgramBase::unuse();

    createShadowMapProg->create(createShadowMapInstancingVertexShader, createShadowMapInstancingFragmentShader);
    createShadowMapProg->link();
	createShadowMapProg->use();
    createShadowMapProg->depthMVP = createShadowMapProg-> getLoc( "depthMVP");
	ProgramBase::unuse();
 
    segmentationMaskProg->create(segmentationMaskInstancingVertexShader, segmentationMaskInstancingFragmentShader);
    segmentationMaskProg->link();
	segmentationMaskProg->use();
	ProgramBase::unuse();
 
    instancingProg->create(instancingVertexShader, instancingFragmentShader);
	instancingProg->link();
	instancingProg->use();
    instancingProg->uniform_texture_diffuse = instancingProg->getLoc("Diffuse");
    instancingProg->regularLightDirIn = instancingProg->getLoc("lightDirIn");
	ProgramBase::unuse();
    
    instancingPointSpriteProg->create(pointSpriteVertexShader, pointSpriteFragmentShader);
    instancingPointSpriteProg->use();
    instancingPointSpriteProg->screenWidth = instancingPointSpriteProg->getLoc("screenWidth");
	ProgramBase::unuse();

	//GLuint offset = 0;

	glGenBuffers(1, &m_data->m_vbo);
	checkError("glGenBuffers");

	glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vbo);

	int size = m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE + COLOR_BUFFER_SIZE + SCALE_BUFFER_SIZE;
	m_data->m_vboSize = size;

	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);  // );

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLInstancingRenderer::init()
{
	b3Assert(glGetError() == GL_NO_ERROR);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	b3Assert(glGetError() == GL_NO_ERROR);

	//	glClearColor(float(0.),float(0.),float(0.4),float(0));

	b3Assert(glGetError() == GL_NO_ERROR);

	b3Assert(glGetError() == GL_NO_ERROR);

	{
		B3_PROFILE("texture");
		if (m_textureenabled)
		{
			if (!m_textureinitialized)
			{
				glActiveTexture(GL_TEXTURE0);

				GLubyte* image = new GLubyte[256 * 256 * 3];
				for (int y = 0; y < 256; ++y)
				{
					//					const int	t=y>>5;
					GLubyte* pi = image + y * 256 * 3;
					for (int x = 0; x < 256; ++x)
					{
						if (x < 2 || y < 2 || x > 253 || y > 253)
						{
							pi[0] = 255;  //0;
							pi[1] = 255;  //0;
							pi[2] = 255;  //0;
						}
						else
						{
							pi[0] = 255;
							pi[1] = 255;
							pi[2] = 255;
						}

						/*
						const int		s=x>>5;
						const GLubyte	b=180;
						GLubyte			c=b+((s+t&1)&1)*(255-b);
						pi[0]=c;
						pi[1]=c;
						pi[2]=c;
						*/

						pi += 3;
					}
				}

				glGenTextures(1, (GLuint*)&m_data->m_defaultTexturehandle);
				glBindTexture(GL_TEXTURE_2D, m_data->m_defaultTexturehandle);
				b3Assert(glGetError() == GL_NO_ERROR);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				glGenerateMipmap(GL_TEXTURE_2D);

				b3Assert(glGetError() == GL_NO_ERROR);

				delete[] image;
				m_textureinitialized = true;
			}

			b3Assert(glGetError() == GL_NO_ERROR);

			glBindTexture(GL_TEXTURE_2D, m_data->m_defaultTexturehandle);
			b3Assert(glGetError() == GL_NO_ERROR);
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			b3Assert(glGetError() == GL_NO_ERROR);
		}
	}
	//glEnable(GL_COLOR_MATERIAL);

	b3Assert(glGetError() == GL_NO_ERROR);

	//	  glEnable(GL_CULL_FACE);
	//	  glCullFace(GL_BACK);
}

void GLInstancingRenderer::resize(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
}

const CommonCameraInterface* GLInstancingRenderer::getActiveCamera() const
{
	return m_data->m_activeCamera;
}

CommonCameraInterface* GLInstancingRenderer::getActiveCamera()
{
	return m_data->m_activeCamera;
}

void GLInstancingRenderer::setActiveCamera(CommonCameraInterface* cam)
{
	m_data->m_activeCamera = cam;
}

void GLInstancingRenderer::setLightSpecularIntensity(const float lightSpecularIntensity[3])
{
	m_data->m_lightSpecularIntensity[0] = lightSpecularIntensity[0];
	m_data->m_lightSpecularIntensity[1] = lightSpecularIntensity[1];
	m_data->m_lightSpecularIntensity[2] = lightSpecularIntensity[2];
}

void GLInstancingRenderer::setLightPosition(const float lightPos[3])
{
	m_data->m_lightPos[0] = lightPos[0];
	m_data->m_lightPos[1] = lightPos[1];
	m_data->m_lightPos[2] = lightPos[2];
}

void GLInstancingRenderer::setShadowMapResolution(int shadowMapResolution)
{
	m_data->m_shadowMapWidth = shadowMapResolution;
	m_data->m_shadowMapHeight = shadowMapResolution;
	m_data->m_updateShadowMap = true;
}

void GLInstancingRenderer::setShadowMapWorldSize(float worldSize)
{
	m_data->m_shadowMapWorldSize = worldSize;
	m_data->m_updateShadowMap = true;
}


void GLInstancingRenderer::setProjectiveTextureMatrices(const float viewMatrix[16], const float projectionMatrix[16])
{
	for (int i = 0; i < 16; i++)
	{
		m_data->m_projectiveTextureViewMatrix[i] = viewMatrix[i];
		m_data->m_projectiveTextureProjectionMatrix[i] = projectionMatrix[i];
	}
}

void GLInstancingRenderer::setProjectiveTexture(bool useProjectiveTexture)
{
	m_data->m_useProjectiveTexture = useProjectiveTexture;
}

void GLInstancingRenderer::updateCamera(int upAxis)
{
	b3Assert(glGetError() == GL_NO_ERROR);
	m_upAxis = upAxis;

	m_data->m_activeCamera->setCameraUpAxis(upAxis);
	m_data->m_activeCamera->setAspectRatio((float)m_screenWidth / (float)m_screenHeight);
	m_data->m_defaultCamera1.update();
	m_data->m_activeCamera->getCameraProjectionMatrix(m_data->m_projectionMatrix);
	m_data->m_activeCamera->getCameraViewMatrix(m_data->m_viewMatrix);
	b3Scalar viewMat[16];
	b3Scalar viewMatInverse[16];

	for (int i = 0; i < 16; i++)
	{
		viewMat[i] = m_data->m_viewMatrix[i];
	}
	b3Transform tr;
	tr.setFromOpenGLMatrix(viewMat);
	tr = tr.inverse();
	tr.getOpenGLMatrix(viewMatInverse);
	for (int i = 0; i < 16; i++)
	{
		m_data->m_viewMatrixInverse[i] = viewMatInverse[i];
	}
}

void writeTextureToPng(int textureWidth, int textureHeight, const char* fileName, int numComponents)
{
	b3Assert(glGetError() == GL_NO_ERROR);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);

	glReadBuffer(GL_NONE);
	float* orgPixels = (float*)malloc(textureWidth * textureHeight * numComponents * 4);
	char* pixels = (char*)malloc(textureWidth * textureHeight * numComponents * 4);
	glReadPixels(0, 0, textureWidth, textureHeight, GL_DEPTH_COMPONENT, GL_FLOAT, orgPixels);
	b3Assert(glGetError() == GL_NO_ERROR);
	for (int j = 0; j < textureHeight; j++)
	{
		for (int i = 0; i < textureWidth; i++)
		{
			float val = orgPixels[(j * textureWidth + i)];
			if (val != 1.f)
			{
				//printf("val[%d,%d]=%f\n", i,j,val);
			}
			pixels[(j * textureWidth + i) * numComponents] = char(orgPixels[(j * textureWidth + i)] * 255.f);
			pixels[(j * textureWidth + i) * numComponents + 1] = 0;  //255.f;
			pixels[(j * textureWidth + i) * numComponents + 2] = 0;  //255.f;
			pixels[(j * textureWidth + i) * numComponents + 3] = 127;

			//pixels[(j*textureWidth+i)*+1]=val;
			//pixels[(j*textureWidth+i)*numComponents+2]=val;
			//pixels[(j*textureWidth+i)*numComponents+3]=255;
		}

		/*	pixels[(j*textureWidth+j)*numComponents]=255;
			pixels[(j*textureWidth+j)*numComponents+1]=0;
			pixels[(j*textureWidth+j)*numComponents+2]=0;
			pixels[(j*textureWidth+j)*numComponents+3]=255;
			*/
	}
	if (0)
	{
		//swap the pixels
		unsigned char tmp;

		for (int j = 0; j < textureHeight / 2; j++)
		{
			for (int i = 0; i < textureWidth; i++)
			{
				for (int c = 0; c < numComponents; c++)
				{
					tmp = pixels[(j * textureWidth + i) * numComponents + c];
					pixels[(j * textureWidth + i) * numComponents + c] =
						pixels[((textureHeight - j - 1) * textureWidth + i) * numComponents + c];
					pixels[((textureHeight - j - 1) * textureWidth + i) * numComponents + c] = tmp;
				}
			}
		}
	}

	stbi_write_png(fileName, textureWidth, textureHeight, numComponents, pixels, textureWidth * numComponents);

	free(pixels);
}

void GLInstancingRenderer::renderScene()
{
	//avoid some Intel driver on a Macbook Pro to lock-up
	//todo: figure out what is going on on that machine

	//glFlush();

	if (m_data->m_useProjectiveTexture)
	{
		renderSceneInternal(B3_USE_PROJECTIVE_TEXTURE_RENDERMODE);
	}
	else
	{
		if (useShadowMap)
		{
			renderSceneInternal(B3_CREATE_SHADOWMAP_RENDERMODE);

			if (m_planeReflectionShapeIndex >= 0)
			{
				/* Don't update color or depth. */
				glDisable(GL_DEPTH_TEST);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				/* Draw 1 into the stencil buffer. */
				glEnable(GL_STENCIL_TEST);
				glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
				glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

				/* Now render floor; floor pixels just get their stencil set to 1. */
				renderSceneInternal(B3_USE_SHADOWMAP_RENDERMODE_REFLECTION_PLANE);

				/* Re-enable update of color and depth. */
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glEnable(GL_DEPTH_TEST);

				/* Now, only render where stencil is set to 1. */
				glStencilFunc(GL_EQUAL, 1, 0xffffffff); /* draw if ==1 */
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

				//draw the reflection objects
				renderSceneInternal(B3_USE_SHADOWMAP_RENDERMODE_REFLECTION);

				glDisable(GL_STENCIL_TEST);
			}

			renderSceneInternal(B3_USE_SHADOWMAP_RENDERMODE);
		}
		else
		{
			renderSceneInternal();
		}
	}
}



void GLInstancingRenderer::drawTexturedTriangleMesh(float worldPosition[3], float worldOrientation[4], const float* vertices, int numvertices, 
    const unsigned int* indices, int numIndices, const Colorf& colorRGBA, int textureIndex, int vertexLayout)
{
	int sz = sizeof(PrimVertex);

	glActiveTexture(GL_TEXTURE0);
	activateTexture(textureIndex);
	checkError("activateTexture");
    TriangleProgram* triangleProg =  (TriangleProgram*)programs.at(0);
 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    triangleProg->use();

	b3Quaternion orn(worldOrientation[0], worldOrientation[1], worldOrientation[2], worldOrientation[3]);
	b3Vector3 pos = b3MakeVector3(worldPosition[0], worldPosition[1], worldPosition[2]);

	b3Transform worldTrans(orn, pos);
	b3Scalar worldMatUnk[16];
	worldTrans.getOpenGLMatrix(worldMatUnk);
	float modelMat[16];
	for (int i = 0; i < 16; i++)
	{
		modelMat[i] = worldMatUnk[i];
	}
	float viewProjection[16];
	OpenGLHelper::b3Matrix4x4Mul16(m_data->m_projectionMatrix, m_data->m_viewMatrix, viewProjection);
	float MVP[16];
    OpenGLHelper::b3Matrix4x4Mul16(viewProjection, modelMat, MVP);
    triangleProg->setUniformMatrix4(triangleProg->mvp_location, (const float*)MVP);
	checkError("setUniformMatrix4");

    triangleProg->setUniform(triangleProg->vcol_location, colorRGBA[0], colorRGBA[1], colorRGBA[2]);
	checkError("setUniform3f");
    triangleBuf.Bind();
    triangleBuf.toData(numvertices * sizeof(PrimVertex), vertices);

 
    triangleProg->setVertexAttrib(0, 3, sizeof(PrimVertex), 0, (const GLvoid*)0);
    triangleProg->setVertexAttrib(1, 2,  sizeof(PrimVertex), 0, (const GLvoid*)((PrimVertex::Length - 2) * sizeof(float)));
   
    triangleBuf.toElement(numIndices, indices);
 
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

	//glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT,indices);
	checkError("glDrawElements");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	ProgramBase::unuse();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	checkError("glBindVertexArray");
}


void GLInstancingRenderer::drawCylinder(const float* a, const float* b, const Colorf& color, const float radius /*= 0.02*/, const unsigned int subdivisions /*= 8*/)
{
    //todo
}


void GLInstancingRenderer::drawSphere(const float* translation, float radius, const Colorf& color, const unsigned int subDivision /*= 16*/)
{
    //todo
}


void GLInstancingRenderer::drawQuad(const float* a, const float* b, const float* c, const float* d, const float* norm, const Colorf& color)
{
    //todo
}

#if 0

void GLInstancingRenderer::writeSingleInstanceScaleToCPU(const double* scale, int srcIndex2)
{
    float scalef[4] = { scale[0],scale[1],scale[2],scale[3] };
    writeSingleInstanceScaleToCPU(scalef, srcIndex2);
}
void GLInstancingRenderer::writeSingleInstanceSpecularColorToCPU(const double* specular, int srcIndex2)
{
    float specularf[4] = { specular[0],specular[1],specular[2],specular[3] };
    writeSingleInstanceSpecularColorToCPU(specularf, srcIndex2);
}

void GLInstancingRenderer::writeSingleInstanceColorToCPU(const double* color, int srcIndex2)
{
    float colorf[4] = { color[0],color[1],color[2],color[3] };
    writeSingleInstanceColorToCPU(colorf, srcIndex2);
}

void GLInstancingRenderer::setLightPosition(const double lightPos[3])
{
    m_data->m_lightPos[0] = lightPos[0];
    m_data->m_lightPos[1] = lightPos[1];
    m_data->m_lightPos[2] = lightPos[2];
}

void GLInstancingRenderer::writeSingleInstanceTransformToCPU(const double* position, const double* orientation, int srcIndex)
{
    float pos[4];
    float orn[4];
    pos[0] = (float)position[0];
    pos[1] = (float)position[1];
    pos[2] = (float)position[2];
    pos[3] = (float)position[3];
    orn[0] = (float)orientation[0];
    orn[1] = (float)orientation[1];
    orn[2] = (float)orientation[2];
    orn[3] = (float)orientation[3];
    writeSingleInstanceTransformToCPU(pos, orn, srcIndex);
}

int GLInstancingRenderer::registerGraphicsInstance(int shapeIndex, const double* pos1, const double* orn1, const double* color1, const double* scaling1)
{
    float pos[4] = { (float)pos1[0], (float)pos1[1], (float)pos1[2], (float)pos1[3] };
    float orn[4] = { (float)orn1[0], (float)orn1[1], (float)orn1[2], (float)orn1[3] };
    float color[4] = { (float)color1[0], (float)color1[1], (float)color1[2], (float)color1[3] };
    float scaling[4] = { (float)scaling1[0], (float)scaling1[1], (float)scaling1[2], (float)scaling1[3] };
    return registerGraphicsInstance(shapeIndex, pos, orn, color, scaling);
}

void GLInstancingRenderer::drawPoint(const double* position, const double color[4], double pointDrawSize)
{
	float pos[4] = {(float)position[0], (float)position[1], (float)position[2], 0};
	float clr[4] = {(float)color[0], (float)color[1], (float)color[2], (float)color[3]};
	drawPoints(pos, clr, 1, 3 * sizeof(float), float(pointDrawSize));
}

void GLInstancingRenderer::drawLine(const double fromIn[4], const double toIn[4], const double colorIn[4], double lineWidthIn)
{
    float from[4] = { float(fromIn[0]), float(fromIn[1]), float(fromIn[2]), float(fromIn[3]) };
    float to[4] = { float(toIn[0]), float(toIn[1]), float(toIn[2]), float(toIn[3]) };
    float color[4] = { float(colorIn[0]), float(colorIn[1]), float(colorIn[2]), float(colorIn[3]) };
    float lineWidth = float(lineWidthIn);
    drawLine(from, to, color, lineWidth);
}

#endif

void GLInstancingRenderer::drawPoint(const float* positions, const Colorf& color, float pointDrawSize)
{
	drawPoints(positions, color, 1, 3 * sizeof(float), pointDrawSize);
}
void GLInstancingRenderer::drawPoints(const float* positions, const Colorf& color, int numPoints, int pointStrideInBytes, float pointDrawSize)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
    LinesProgram* linesProg = (LinesProgram*)programs.at(3);
	b3Assert(glGetError() == GL_NO_ERROR);
	    linesProg->use();
        linesProg->setUniformMatrix4(linesProg->ProjectionMatrix, &m_data->m_projectionMatrix[0]);
        linesProg->setUniformMatrix4(linesProg->ModelViewMatrix, &m_data->m_viewMatrix[0]);
        linesProg->setUniform(linesProg->colour, color[0], color[1], color[2], color[3]);

	glPointSize(pointDrawSize);
	glBindVertexArray(lineBuf.VertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, lineBuf.VertexBufferObject);

	int maxPointsInBatch = MAX_POINTS_IN_BATCH;
	int remainingPoints = numPoints;
	int offsetNumPoints = 0;
	while (1)
	{
		int curPointsInBatch = b3Min(maxPointsInBatch, remainingPoints);
		if (curPointsInBatch)
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0, curPointsInBatch * pointStrideInBytes, positions + offsetNumPoints * (pointStrideInBytes / sizeof(float)));
			glEnableVertexAttribArray(0);
			int numFloats = 3;  // pointStrideInBytes / sizeof(float);
			glVertexAttribPointer(0, numFloats, GL_FLOAT, GL_FALSE, pointStrideInBytes, 0);
			glDrawArrays(GL_POINTS, 0, curPointsInBatch);
			remainingPoints -= curPointsInBatch;
			offsetNumPoints += curPointsInBatch;
		}
		else
		{
			break;
		}
	}
	
	glBindVertexArray(0);
	glPointSize(1);
	ProgramBase::unuse();
}

void GLInstancingRenderer::drawLines(const float* positions, const Colorf& color, int numPoints, int pointStrideInBytes, 
    const unsigned int* indices, int numIndices, float lineWidthIn)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
    LinesProgram* linesProg = (LinesProgram*)programs.at(3);
	float lineWidth = lineWidthIn;
	b3Clamp(lineWidth, (float)lineWidthRange[0], (float)lineWidthRange[1]);
	glLineWidth(lineWidth);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	b3Assert(glGetError() == GL_NO_ERROR);
	    linesProg->use();
        linesProg->setUniformMatrix4(linesProg->ProjectionMatrix, &m_data->m_projectionMatrix[0]);
        linesProg->setUniformMatrix4(linesProg->ModelViewMatrix, &m_data->m_viewMatrix[0]);
        linesProg->setUniform(linesProg->colour, color[0], color[1], color[2], color[3]);

	//	glPointSize(pointDrawSize);
	glBindVertexArray(linesBuf.VertexArrayObject);

	b3Assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ARRAY_BUFFER, linesBuf.VertexBufferObject);

	{
		glBufferData(GL_ARRAY_BUFFER, numPoints * pointStrideInBytes, 0, GL_DYNAMIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, numPoints * pointStrideInBytes, positions);
		b3Assert(glGetError() == GL_NO_ERROR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, linesBuf.VertexBufferObject);
		glEnableVertexAttribArray(0);

		b3Assert(glGetError() == GL_NO_ERROR);
		int numFloats = 3;
		glVertexAttribPointer(0, numFloats, GL_FLOAT, GL_FALSE, pointStrideInBytes, 0);
		b3Assert(glGetError() == GL_NO_ERROR);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesBuf.IndexVbo);
		int indexBufferSizeInBytes = numIndices * sizeof(int);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSizeInBytes, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexBufferSizeInBytes, indices);

		glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	for (int i=0;i<numIndices;i++)
	//		printf("indicec[i]=%d]\n",indices[i]);
	b3Assert(glGetError() == GL_NO_ERROR);
	glBindVertexArray(0);
	b3Assert(glGetError() == GL_NO_ERROR);
	glPointSize(1);
	b3Assert(glGetError() == GL_NO_ERROR);
	ProgramBase::unuse();
}


void GLInstancingRenderer::drawLine(const float from[4], const float to[4], const Colorf& color, float lineWidth)
{
	b3Assert(glGetError() == GL_NO_ERROR);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	b3Assert(glGetError() == GL_NO_ERROR);
    LinesProgram* linesProg = (LinesProgram*)programs.at(3);
    linesProg->use();

	b3Assert(glGetError() == GL_NO_ERROR);

    linesProg->setUniformMatrix4(linesProg->ProjectionMatrix, &m_data->m_projectionMatrix[0]);
    linesProg->setUniformMatrix4(linesProg->ModelViewMatrix, &m_data->m_viewMatrix[0]);
    linesProg->setUniform(linesProg->colour, color[0], color[1], color[2], color[3]);

	b3Assert(glGetError() == GL_NO_ERROR);

	const float vertexPositions[] = {
		from[0], from[1], from[2], 1,
		to[0], to[1], to[2], 1};
 
	b3Assert(glGetError() == GL_NO_ERROR);

	b3Clamp(lineWidth, (float)lineWidthRange[0], (float)lineWidthRange[1]);
	glLineWidth(lineWidth);

	b3Assert(glGetError() == GL_NO_ERROR);
    lineBuf.Bind();
 
    lineBuf.toData(sizeof(vertexPositions), vertexPositions);//??
 
    linesProg->setVertexAttrib(0, 4, 0, -1, nullptr);

	glDrawArrays(GL_LINES, 0, 2);
	b3Assert(glGetError() == GL_NO_ERROR);

    lineBuf.unBind();

	b3Assert(glGetError() == GL_NO_ERROR);
	ProgramBase::unuse();
}

B3_ATTRIBUTE_ALIGNED16(struct)
SortableTransparentInstance
{
	b3Scalar m_projection;

	int m_shapeIndex;
	int m_instanceId;
};

B3_ATTRIBUTE_ALIGNED16(struct)
TransparentDistanceSortPredicate{

	inline bool operator()(const SortableTransparentInstance& a, const SortableTransparentInstance& b) const {

		return (a.m_projection > b.m_projection);
}
}
;

void GLInstancingRenderer::renderSceneInternal(int orgRenderMode)
{
	B3_PROFILE("renderSceneInternal");
	int renderMode = orgRenderMode;
	bool reflectionPass = false;
	bool reflectionPlanePass = false;

	if (orgRenderMode == B3_USE_SHADOWMAP_RENDERMODE_REFLECTION_PLANE)
	{
		reflectionPlanePass = true;
		renderMode = B3_USE_SHADOWMAP_RENDERMODE;
	}
	if (orgRenderMode == B3_USE_SHADOWMAP_RENDERMODE_REFLECTION)
	{
		reflectionPass = true;
		renderMode = B3_USE_SHADOWMAP_RENDERMODE;
	}

	if (!useShadowMap)
	{
		renderMode = orgRenderMode;
	}

	if (orgRenderMode == B3_USE_PROJECTIVE_TEXTURE_RENDERMODE)
	{
		renderMode = B3_USE_PROJECTIVE_TEXTURE_RENDERMODE;
	}

	//	glEnable(GL_DEPTH_TEST);

	GLint dims[4];
	glGetIntegerv(GL_VIEWPORT, dims);
	//we need to get the viewport dims, because on Apple Retina the viewport dimension is different from screenWidth
	//printf("dims=%d,%d,%d,%d\n",dims[0],dims[1],dims[2],dims[3]);
	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	

	{
		B3_PROFILE("init");
		init();
	}

	b3Assert(glGetError() == GL_NO_ERROR);

	float depthProjectionMatrix[4][4];
	float depthModelViewMatrix[4][4];
	//float depthModelViewMatrix2[4][4];

	// For projective texture mapping
	//float textureProjectionMatrix[4][4];
	//float textureModelViewMatrix[4][4];

	// Compute the MVP matrix from the light's point of view
	if (renderMode == B3_CREATE_SHADOWMAP_RENDERMODE)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		if (m_data->m_shadowMap && m_data->m_updateShadowMap)
		{
			m_data->m_updateShadowMap = false;
			glDeleteTextures(1, &m_data->m_shadowTexture);
			delete m_data->m_shadowMap;
			m_data->m_shadowMap = 0;
		}
		if (!m_data->m_shadowMap)
		{
			glActiveTexture(GL_TEXTURE0);

			glGenTextures(1, &m_data->m_shadowTexture);
			glBindTexture(GL_TEXTURE_2D, m_data->m_shadowTexture);
			//glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT16,m_screenWidth,m_screenHeight,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);
			//glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32,m_screenWidth,m_screenHeight,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);

#ifdef OLD_SHADOWMAP_INIT
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_data->m_shadowMapWidth, m_data->m_shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
#else   //OLD_SHADOWMAP_INIT                                                              \
		//Reduce size of shadowMap if glTexImage2D call fails as may happen in some cases \
		//https://github.com/bulletphysics/bullet3/issues/40

			int size;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
			if (size < m_data->m_shadowMapWidth)
			{
				m_data->m_shadowMapWidth = size;
			}
			if (size < m_data->m_shadowMapHeight)
			{
				m_data->m_shadowMapHeight = size;
			}
			GLuint err;
			do
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
					m_data->m_shadowMapWidth, m_data->m_shadowMapHeight,
							 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
				err = glGetError();
				if (err != GL_NO_ERROR)
				{
					m_data->m_shadowMapHeight >>= 1;
					m_data->m_shadowMapWidth >>= 1;
				}
			} while (err != GL_NO_ERROR && m_data->m_shadowMapWidth > 0);
#endif  //OLD_SHADOWMAP_INIT

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			float l_ClampColor[] = {1.0, 1.0, 1.0, 1.0};
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, l_ClampColor);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

			m_data->m_shadowMap = new GLRenderToTexture();
			m_data->m_shadowMap->init(m_data->m_shadowMapWidth, m_data->m_shadowMapHeight, m_data->m_shadowTexture, RENDERTEXTURE_DEPTH);
		}
		m_data->m_shadowMap->enable();
		glViewport(0, 0, m_data->m_shadowMapWidth, m_data->m_shadowMapHeight);
		//glClearColor(1,1,1,1);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glClearColor(0.3,0.3,0.3,1);

		//		m_data->m_shadowMap->disable();
		//	return;
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);  // Cull back-facing triangles -> draw only front-facing triangles

		b3Assert(glGetError() == GL_NO_ERROR);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

    OpenGLHelper::b3CreateOrtho(-m_data->m_shadowMapWorldSize, m_data->m_shadowMapWorldSize, -m_data->m_shadowMapWorldSize, m_data->m_shadowMapWorldSize, 1, 300, depthProjectionMatrix);  //-14,14,-14,14,1,200, depthProjectionMatrix);
	float depthViewMatrix[4][4];
	b3Vector3 center = b3MakeVector3(0, 0, 0);
	m_data->m_activeCamera->getCameraTargetPosition(center);
	//float upf[3];
	//m_data->m_activeCamera->getCameraUpVector(upf);
	b3Vector3 up, lightFwd;
	b3Vector3 lightDir = m_data->m_lightPos.normalized();
	b3PlaneSpace1(lightDir, up, lightFwd);
	//	b3Vector3 up = b3MakeVector3(upf[0],upf[1],upf[2]);
    OpenGLHelper::b3CreateLookAt(m_data->m_lightPos + center, center, up, &depthViewMatrix[0][0]);
	//b3CreateLookAt(lightPos,m_data->m_cameraTargetPosition,b3Vector3(0,1,0),(float*)depthModelViewMatrix2);

	float depthModelMatrix[4][4];
    OpenGLHelper::b3CreateDiagonalMatrix(1.f, depthModelMatrix);

    OpenGLHelper::b3Matrix4x4Mul(depthViewMatrix, depthModelMatrix, depthModelViewMatrix);

	float depthMVP[4][4];
    OpenGLHelper::b3Matrix4x4Mul(depthProjectionMatrix, depthModelViewMatrix, depthMVP);

	float biasMatrix[4][4] = {
		{0.5, 0.0, 0.0, 0.0},
		{0.0, 0.5, 0.0, 0.0},
		{0.0, 0.0, 0.5, 0.0},
		{0.5, 0.5, 0.5, 1.0}};

	float depthBiasMVP[4][4];
    OpenGLHelper::b3Matrix4x4Mul(biasMatrix, depthMVP, depthBiasMVP);

	// TODO: Expose the projective texture matrix setup. Temporarily set it to be the same as camera view projection matrix.
	float textureMVP[16];
    OpenGLHelper::b3Matrix4x4Mul16(m_data->m_projectiveTextureProjectionMatrix, m_data->m_projectiveTextureViewMatrix, textureMVP);

	//float m_frustumZNear=0.1;
	//float m_frustumZFar=100.f;

	//b3CreateFrustum(-m_frustumZNear, m_frustumZNear, -m_frustumZNear, m_frustumZNear, m_frustumZNear, m_frustumZFar,(float*)depthProjectionMatrix);

	//b3CreateLookAt(lightPos,m_data->m_cameraTargetPosition,b3Vector3(0,0,1),(float*)depthModelViewMatrix);

	{
		B3_PROFILE("updateCamera");
		//	updateCamera();
		m_data->m_activeCamera->getCameraProjectionMatrix(m_data->m_projectionMatrix);
		m_data->m_activeCamera->getCameraViewMatrix(m_data->m_viewMatrix);
	}

	b3Assert(glGetError() == GL_NO_ERROR);

	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	{
		B3_PROFILE("glFlush2");

		glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vbo);
		//glFlush();
	}
	b3Assert(glGetError() == GL_NO_ERROR);

	int totalNumInstances = 0;

	for (int i = 0; i < m_graphicsInstances.size(); i++)
	{
		totalNumInstances += m_graphicsInstances[i]->m_numGraphicsInstances;
	}

	b3AlignedObjectArray<SortableTransparentInstance> transparentInstances;
	{
		int curOffset = 0;
		//GLuint lastBindTexture = 0;

		transparentInstances.reserve(totalNumInstances);

		float fwd[3];
		m_data->m_activeCamera->getCameraForwardVector(fwd);
		b3Vector3 camForwardVec;
		camForwardVec.setValue(fwd[0], fwd[1], fwd[2]);

		for (int obj = 0; obj < m_graphicsInstances.size(); obj++)
		{
			b3GraphicsInstance* gfxObj = m_graphicsInstances[obj];

			if (gfxObj->m_numGraphicsInstances)
			{
				SortableTransparentInstance inst;

				inst.m_shapeIndex = obj;

				if ((gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY) == 0)
				{
					inst.m_instanceId = curOffset;
					b3Vector3 centerPosition;
					centerPosition.setValue(m_data->m_instance_positions_ptr[inst.m_instanceId * 4 + 0],
											m_data->m_instance_positions_ptr[inst.m_instanceId * 4 + 1],
											m_data->m_instance_positions_ptr[inst.m_instanceId * 4 + 2]);
					centerPosition *= -1;  //reverse sort opaque instances
					inst.m_projection = centerPosition.dot(camForwardVec);
					transparentInstances.push_back(inst);
				}
				else
				{
					for (int i = 0; i < gfxObj->m_numGraphicsInstances; i++)
					{
						inst.m_instanceId = curOffset + i;
						b3Vector3 centerPosition;

						centerPosition.setValue(m_data->m_instance_positions_ptr[inst.m_instanceId * 4 + 0],
												m_data->m_instance_positions_ptr[inst.m_instanceId * 4 + 1],
												m_data->m_instance_positions_ptr[inst.m_instanceId * 4 + 2]);
						inst.m_projection = centerPosition.dot(camForwardVec);
						transparentInstances.push_back(inst);
					}
				}
				curOffset += gfxObj->m_numGraphicsInstances;
			}
		}
		TransparentDistanceSortPredicate sorter;

		transparentInstances.quickSort(sorter);
	}
    TriangleProgram* triangleProg = (TriangleProgram*)programs.at(0);
    CreateShadowMapProgram* createShadowMapProg = (CreateShadowMapProgram*)programs.at(1);
    UseShadowMapProgram* useShadowProg = (UseShadowMapProgram*)programs.at(2);
    LinesProgram* linesProg = (LinesProgram*)programs.at(3);
    ProjectiveTextureProgram* projectiveTextureProg = (ProjectiveTextureProgram*)programs.at(4);
    InstancingProgram* instancingProg = (InstancingProgram*)programs.at(5);
    SegmentationMaskProgram* segmentationMaskProg = (SegmentationMaskProgram*)programs.at(6);
    InstancingPointSpriteProgram* instancingPointSpriteProg = (InstancingPointSpriteProgram*)programs.at(7);

	//two passes: first for opaque instances, second for transparent ones.
	for (int pass = 0; pass < 2; pass++)
	{
		for (int i = 0; i < transparentInstances.size(); i++)
		{
			int shapeIndex = transparentInstances[i].m_shapeIndex;

			//during a reflectionPlanePass, only draw the plane, nothing else
			if (reflectionPlanePass)
			{
				if (shapeIndex != m_planeReflectionShapeIndex)
					continue;
			}

			b3GraphicsInstance* gfxObj = m_graphicsInstances[shapeIndex];

			//only draw stuff (opaque/transparent) if it is the right pass
			int drawThisPass = (pass == 0) == ((gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY) == 0);

			//transparent objects don't cast shadows (to simplify things)
			if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
			{
				if (renderMode == B3_CREATE_SHADOWMAP_RENDERMODE)
					drawThisPass = 0;
			}

			if (drawThisPass && gfxObj->m_numGraphicsInstances)
			{
				glActiveTexture(GL_TEXTURE0);
				GLuint curBindTexture = 0;
				if (gfxObj->m_flags & B3_INSTANCE_TEXTURE)
				{
					curBindTexture = m_data->m_textureHandles[gfxObj->m_textureIndex].m_glTexture;

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

					if (m_data->m_textureHandles[gfxObj->m_textureIndex].m_enableFiltering)
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					}
					else
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					}
				}
				else
				{
					curBindTexture = m_data->m_defaultTexturehandle;
				}

				//disable lazy evaluation, it just leads to bugs
				//if (lastBindTexture != curBindTexture)
				{
					glBindTexture(GL_TEXTURE_2D, curBindTexture);
				}
				//lastBindTexture = curBindTexture;

				b3Assert(glGetError() == GL_NO_ERROR);
				//	int myOffset = gfxObj->m_instanceOffset*4*sizeof(float);

				int POSITION_BUFFER_SIZE = (totalNumInstances * sizeof(float) * 4);
				int ORIENTATION_BUFFER_SIZE = (totalNumInstances * sizeof(float) * 4);
				int COLOR_BUFFER_SIZE = (totalNumInstances * sizeof(float) * 4);
				//		int SCALE_BUFFER_SIZE = (totalNumInstances*sizeof(float)*3);

			    gfxObj->m_buf.Bind();

				int vertexStride = GLInstanceVertex::Length * sizeof(float);
				PointerCaster vertex;
				vertex.m_baseIndex = gfxObj->m_vertexArrayOffset * vertexStride;

				//vertex position
				ProgramBase::setVertexAttrib(0, 4, vertexStride, 0, (GLvoid*)vertex.m_pointer);
				//instance_position
				ProgramBase::setVertexAttrib(1, 4, 0, 1, (GLvoid*)(transparentInstances[i].m_instanceId * 4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes));
				//instance_quaternion
				ProgramBase::setVertexAttrib(2, 4, 0, 1, (GLvoid*)(transparentInstances[i].m_instanceId * 4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE));
                 
				ProgramBase::setVertexAttrib(3, 2, vertexStride, 0, (GLvoid*)((GLInstanceVertex::Length - 2) * sizeof(float) + vertex.m_baseIndex));
				ProgramBase::setVertexAttrib(4, 3, vertexStride, 0, (GLvoid*)(4 * sizeof(float) + vertex.m_baseIndex));
				//instance_color
                ProgramBase::setVertexAttrib(5, 4, 0, 1, (GLvoid*)(transparentInstances[i].m_instanceId * 4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE));
				//instance_scale
				ProgramBase::setVertexAttrib(6, 4, 0, 1, (GLvoid*)(transparentInstances[i].m_instanceId * 4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE + COLOR_BUFFER_SIZE));
                   
				int indexCount = gfxObj->m_numIndices;
				GLvoid* indexOffset = 0;
                 
				{
					B3_PROFILE("glDrawElementsInstanced");

					if (gfxObj->m_primitiveType == B3_GL_POINTS)
					{
						    instancingPointSpriteProg->use();
                            instancingPointSpriteProg->setUniformMatrix4(instancingPointSpriteProg->ProjectionMatrix, &m_data->m_projectionMatrix[0]);
                            instancingPointSpriteProg->setUniformMatrix4(instancingPointSpriteProg->ModelViewMatrix, &m_data->m_viewMatrix[0]);
                            instancingPointSpriteProg->setUniform(instancingPointSpriteProg->screenWidth, float(m_screenWidth));

						//setUniform(uniform_texture_diffusePointSprite, 0);
						b3Assert(glGetError() == GL_NO_ERROR);
						glPointSize(20);

#ifndef __APPLE__
						glEnable(GL_POINT_SPRITE_ARB);
//					glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
#endif

						glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
						glDrawElementsInstanced(GL_POINTS, indexCount, GL_UNSIGNED_INT, indexOffset, gfxObj->m_numGraphicsInstances);
					}
					else
					{
						if (gfxObj->m_flags & B3_INSTANCE_DOUBLE_SIDED)
						{
							glDisable(GL_CULL_FACE);
						}

						switch (renderMode)
						{
							case B3_SEGMENTATION_MASK_RENDERMODE:
							{
								    segmentationMaskProg->use();
                                    segmentationMaskProg->setUniformMatrix4(segmentationMaskProg->ProjectionMatrix, &m_data->m_projectionMatrix[0]);
                                    segmentationMaskProg->setUniformMatrix4(segmentationMaskProg->ModelViewMatrix, &m_data->m_viewMatrix[0]);
								glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexOffset, gfxObj->m_numGraphicsInstances);

								break;
							}
							case B3_DEFAULT_RENDERMODE:
							{
								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									glDepthMask(false);
									glEnable(GL_BLEND);
									glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
								}

								    instancingProg->use();
                                    instancingProg->setUniformMatrix4(instancingProg->ProjectionMatrix, &m_data->m_projectionMatrix[0]);
                                    instancingProg->setUniformMatrix4(instancingProg->ModelViewMatrix, &m_data->m_viewMatrix[0]);

								b3Vector3 gLightDir = m_data->m_lightPos;
								gLightDir.normalize();
                                instancingProg->setUniform(instancingProg->regularLightDirIn, gLightDir[0], gLightDir[1], gLightDir[2]);

                                instancingProg->setUniform(instancingProg->uniform_texture_diffuse, 0);

								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									int instanceId = transparentInstances[i].m_instanceId;
                                    ProgramBase::setVertexAttrib(1, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes));
                                    ProgramBase::setVertexAttrib(2, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE));
                                    ProgramBase::setVertexAttrib(5, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE));
                                    ProgramBase::setVertexAttrib(6, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE + COLOR_BUFFER_SIZE));

									glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
								}
								else
								{
									glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexOffset, gfxObj->m_numGraphicsInstances);
								}

								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									glDisable(GL_BLEND);
									glDepthMask(true);
								}

								break;
							}
							case B3_CREATE_SHADOWMAP_RENDERMODE:
							{
							    createShadowMapProg->use();
                                createShadowMapProg->setUniformMatrix4(createShadowMapProg->depthMVP, &depthMVP[0][0]);
								glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexOffset, gfxObj->m_numGraphicsInstances);
								break;
							}

							case B3_USE_SHADOWMAP_RENDERMODE:
							{
								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									glDepthMask(false);
									glEnable(GL_BLEND);
									glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
								}

							    useShadowProg->use();
                                useShadowProg->setUniformMatrix4(useShadowProg->ProjectionMatrix,  &m_data->m_projectionMatrix[0]);
					  
                                useShadowProg->setUniform(useShadowProg->lightSpecularIntensity, m_data->m_lightSpecularIntensity[0], m_data->m_lightSpecularIntensity[1], m_data->m_lightSpecularIntensity[2]);
                                useShadowProg->setUniform(useShadowProg->materialSpecularColor, gfxObj->m_materialSpecularColor[0], gfxObj->m_materialSpecularColor[1], gfxObj->m_materialSpecularColor[2]);

								float MVP[16];
								if (reflectionPass)
								{
									//todo: create an API to select this reflection matrix, to allow
									//reflection planes different from Z-axis up through (0,0,0)
									float tmp[16];
									float reflectionMatrix[16] = {1, 0, 0, 0,
																  0, 1, 0, 0,
																  0, 0, -1, 0,
																  0, 0, 0, 1};
									glCullFace(GL_FRONT);
									OpenGLHelper::b3Matrix4x4Mul16(m_data->m_viewMatrix, reflectionMatrix, tmp);
                                    OpenGLHelper::b3Matrix4x4Mul16(m_data->m_projectionMatrix, tmp, MVP);
								}
								else
								{
                                    OpenGLHelper::b3Matrix4x4Mul16(m_data->m_projectionMatrix, m_data->m_viewMatrix, MVP);
									glCullFace(GL_BACK);
								}

                                useShadowProg->setUniformMatrix4(useShadowProg->MVP, &MVP[0]);
								//gLightDir.normalize();
                                useShadowProg->setUniform(useShadowProg->lightPosIn, m_data->m_lightPos[0], m_data->m_lightPos[1], m_data->m_lightPos[2]);
								float camPos[3];
								m_data->m_activeCamera->getCameraPosition(camPos);
                                useShadowProg->setUniform(useShadowProg->cameraPositionIn, camPos[0], camPos[1], camPos[2]);
                                useShadowProg->setUniform(useShadowProg->materialShininessIn, gfxObj->m_materialShinyNess);

                                useShadowProg->setUniformMatrix4(useShadowProg->DepthBiasModelViewMatrix, &depthBiasMVP[0][0]);
								glActiveTexture(GL_TEXTURE1);
								glBindTexture(GL_TEXTURE_2D, m_data->m_shadowTexture);
                                useShadowProg->setUniform(useShadowProg->shadowMap, 1);

								//sort transparent objects

								//gfxObj->m_instanceOffset

								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									int instanceId = transparentInstances[i].m_instanceId;
                                    ProgramBase::setVertexAttrib(1, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes));
                                    ProgramBase::setVertexAttrib(2, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE));
                                    ProgramBase::setVertexAttrib(5, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE));
                                    ProgramBase::setVertexAttrib(6, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE + COLOR_BUFFER_SIZE));
									glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
								}
								else
								{
									glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexOffset, gfxObj->m_numGraphicsInstances);
								}

								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									glDisable(GL_BLEND);
									glDepthMask(true);
								}
								glActiveTexture(GL_TEXTURE1);
								glBindTexture(GL_TEXTURE_2D, 0);

								glActiveTexture(GL_TEXTURE0);
								glBindTexture(GL_TEXTURE_2D, 0);
								break;
							}
							case B3_USE_PROJECTIVE_TEXTURE_RENDERMODE:
							{
								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									glDepthMask(false);
									glEnable(GL_BLEND);
									glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
								}

							    projectiveTextureProg->use();
                                projectiveTextureProg->setUniformMatrix4(projectiveTextureProg->ProjectionMatrix, &m_data->m_projectionMatrix[0]);
                                projectiveTextureProg->setUniform(projectiveTextureProg->lightSpecularIntensity, m_data->m_lightSpecularIntensity[0], m_data->m_lightSpecularIntensity[1], m_data->m_lightSpecularIntensity[2]);
                                projectiveTextureProg->setUniform(projectiveTextureProg->materialSpecularColor, gfxObj->m_materialSpecularColor[0], gfxObj->m_materialSpecularColor[1], gfxObj->m_materialSpecularColor[2]);

								float MVP[16];
								if (reflectionPass)
								{
									float tmp[16];
									float reflectionMatrix[16] = {1, 0, 0, 0,
																  0, 1, 0, 0,
																  0, 0, -1, 0,
																  0, 0, 0, 1};
									glCullFace(GL_FRONT);
                                    OpenGLHelper::b3Matrix4x4Mul16(m_data->m_viewMatrix, reflectionMatrix, tmp);
                                    OpenGLHelper::b3Matrix4x4Mul16(m_data->m_projectionMatrix, tmp, MVP);
								}
								else
								{
                                    OpenGLHelper::b3Matrix4x4Mul16(m_data->m_projectionMatrix, m_data->m_viewMatrix, MVP);
									glCullFace(GL_BACK);
								}

								projectiveTextureProg->setUniformMatrix4(projectiveTextureProg->MVP,  &MVP[0]);
								projectiveTextureProg->setUniform(projectiveTextureProg->lightPosIn, m_data->m_lightPos[0], m_data->m_lightPos[1], m_data->m_lightPos[2]);
								float camPos[3];
								m_data->m_activeCamera->getCameraPosition(camPos);
                                projectiveTextureProg->setUniform(projectiveTextureProg->cameraPositionIn, camPos[0], camPos[1], camPos[2]);
                                projectiveTextureProg->setUniform(projectiveTextureProg->materialShininessIn, gfxObj->m_materialShinyNess);

                                projectiveTextureProg->setUniformMatrix4(projectiveTextureProg->TextureMVP, &textureMVP[0]);

								//sort transparent objects
								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									int instanceId = transparentInstances[i].m_instanceId;
                                    ProgramBase::setVertexAttrib(1, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes));
                                    ProgramBase::setVertexAttrib(2, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE));
                                    ProgramBase::setVertexAttrib(5, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE));
                                    ProgramBase::setVertexAttrib(6, 4, 0, -1, (GLvoid*)((instanceId)*4 * sizeof(float) + m_data->m_maxShapeCapacityInBytes + POSITION_BUFFER_SIZE + ORIENTATION_BUFFER_SIZE + COLOR_BUFFER_SIZE));
									glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
								}
								else
								{
									glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indexOffset, gfxObj->m_numGraphicsInstances);
								}

								if (gfxObj->m_flags & B3_INSTANCE_TRANSPARANCY)
								{
									glDisable(GL_BLEND);
									glDepthMask(true);
								}

								glActiveTexture(GL_TEXTURE0);
								glBindTexture(GL_TEXTURE_2D, 0);
								break;
							}
							default:
							{
								//	b3Assert(0);
							}
						};
						if (gfxObj->m_flags & B3_INSTANCE_DOUBLE_SIDED)
						{
							glEnable(GL_CULL_FACE);
						}
					}

					//glDrawElementsInstanced(GL_LINE_LOOP, indexCount, GL_UNSIGNED_INT, (void*)indexOffset, gfxObj->m_numGraphicsInstances);
				}
			}
		}
	}

	{
		B3_PROFILE("glFlush");
		//glFlush();
	}
	if (renderMode == B3_CREATE_SHADOWMAP_RENDERMODE)
	{
		//	writeTextureToPng(shadowMapWidth,shadowMapHeight,"shadowmap.png",4);
		m_data->m_shadowMap->disable();
		glBindFramebuffer(GL_FRAMEBUFFER, m_data->m_renderFrameBuffer);
		glViewport(dims[0], dims[1], dims[2], dims[3]);
	}

	b3Assert(glGetError() == GL_NO_ERROR);
	{
		B3_PROFILE("ProgramBase::unuse();");
		ProgramBase::unuse();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glDisable(GL_CULL_FACE);
	b3Assert(glGetError() == GL_NO_ERROR);
}

void GLInstancingRenderer::CleanupShaders()
{
}

void GLInstancingRenderer::setPlaneReflectionShapeIndex(int index)
{
	m_planeReflectionShapeIndex = index;
}

void GLInstancingRenderer::enableShadowMap()
{
	glActiveTexture(GL_TEXTURE0);
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_data->m_shadowTexture);
	//glBindTexture(GL_TEXTURE_2D, m_data->m_defaultTexturehandle);
}

void GLInstancingRenderer::clearZBuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

int GLInstancingRenderer::getMaxShapeCapacity() const
{
	return m_data->m_maxShapeCapacityInBytes;
}
int GLInstancingRenderer::getInstanceCapacity() const
{
	return m_data->m_maxNumObjectCapacity;
}

void GLInstancingRenderer::setRenderFrameBuffer(unsigned int renderFrameBuffer)
{
	m_data->m_renderFrameBuffer = (GLuint)renderFrameBuffer;
}

int GLInstancingRenderer::getTotalNumInstances() const
{
	return m_data->m_totalNumInstances;
}

#endif  //NO_OPENGL3
