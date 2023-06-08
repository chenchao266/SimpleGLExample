#include "TinyRenderer.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include "CommonInterfaces/CommonFileIOInterface.h"
#include "OpenGLWindow/ShapeData.h"
#include "b3BulletDefaultFileIO.h"
#include "b3ResourcePath.h"
//#include "Bullet3Common/b3Logging.h"
#include "Bullet3Common/b3MinMax.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/geometry.h"
#include "Bullet3Common/PrimVertex.h"
#include "model.h"
#include "TinyShader.h"
#include "tgaimage.h"

TinyRenderObjectData::TinyRenderObjectData(TGAImage& rgbColorBuffer, b3AlignedObjectArray<float>& depthBuffer, b3AlignedObjectArray<float>* shadowBuffer)
	: m_model(0),
	  m_rgbColorBuffer(rgbColorBuffer),
	  m_depthBuffer(depthBuffer),
	  m_shadowBuffer(shadowBuffer),
	  m_segmentationMaskBufferPtr(0),
	  m_userData(0),
	  m_userIndex(-1),
	  m_objectIndex(-1)
{
	Vec3f eye(1, 1, 3);
	Vec3f center(0, 0, 0);
	Vec3f up(0, 0, 1);
	m_lightDirWorld.setValue(0, 0, 0);
	m_lightColor.setValue(1, 1, 1);
	m_localScaling.setValue(1, 1, 1);
	m_modelMatrix = Matrixf::identity();
	m_lightAmbientCoeff = 0.6;
	m_lightDiffuseCoeff = 0.35;
	m_lightSpecularCoeff = 0.05;
}

TinyRenderObjectData::TinyRenderObjectData(TGAImage& rgbColorBuffer, b3AlignedObjectArray<float>& depthBuffer, 
    b3AlignedObjectArray<float>* shadowBuffer, b3AlignedObjectArray<int>* segmentationMaskBuffer, int objectIndex, int linkIndex)
	: m_model(0),
	  m_rgbColorBuffer(rgbColorBuffer),
	  m_depthBuffer(depthBuffer),
	  m_shadowBuffer(shadowBuffer),
	  m_segmentationMaskBufferPtr(segmentationMaskBuffer),
	  m_userData(0),
	  m_userIndex(-1),
	  m_objectIndex(objectIndex),
	  m_linkIndex(linkIndex)
{
	Vec3f eye(1, 1, 3);
	Vec3f center(0, 0, 0);
	Vec3f up(0, 0, 1);
	m_lightDirWorld.setValue(0, 0, 0);
	m_lightColor.setValue(1, 1, 1);
	m_localScaling.setValue(1, 1, 1);
	m_modelMatrix = Matrixf::identity();
	m_lightAmbientCoeff = 0.6;
	m_lightDiffuseCoeff = 0.35;
	m_lightSpecularCoeff = 0.05;
}

TinyRenderObjectData::TinyRenderObjectData(TGAImage& rgbColorBuffer, b3AlignedObjectArray<float>& depthBuffer)
	: m_model(0),
	  m_rgbColorBuffer(rgbColorBuffer),
	  m_depthBuffer(depthBuffer),
	  m_segmentationMaskBufferPtr(0),
	  m_userData(0),
	  m_userIndex(-1),
	  m_objectIndex(-1)
{
	Vec3f eye(1, 1, 3);
	Vec3f center(0, 0, 0);
	Vec3f up(0, 0, 1);
	m_lightDirWorld.setValue(0, 0, 0);
	m_lightColor.setValue(1, 1, 1);
	m_localScaling.setValue(1, 1, 1);
	m_modelMatrix = Matrixf::identity();
	m_lightAmbientCoeff = 0.6;
	m_lightDiffuseCoeff = 0.35;
	m_lightSpecularCoeff = 0.05;
}

TinyRenderObjectData::TinyRenderObjectData(TGAImage& rgbColorBuffer, b3AlignedObjectArray<float>& depthBuffer, 
    b3AlignedObjectArray<int>* segmentationMaskBuffer, int objectIndex)
	: m_model(0),
	  m_rgbColorBuffer(rgbColorBuffer),
	  m_depthBuffer(depthBuffer),
	  m_segmentationMaskBufferPtr(segmentationMaskBuffer),
	  m_userData(0),
	  m_userIndex(-1),
	  m_objectIndex(objectIndex)
{
	Vec3f eye(1, 1, 3);
	Vec3f center(0, 0, 0);
	Vec3f up(0, 0, 1);
	m_lightDirWorld.setValue(0, 0, 0);
	m_lightColor.setValue(1, 1, 1);
	m_localScaling.setValue(1, 1, 1);
	m_modelMatrix = Matrixf::identity();
	m_lightAmbientCoeff = 0.6;
	m_lightDiffuseCoeff = 0.35;
	m_lightSpecularCoeff = 0.05;
}

void TinyRenderObjectData::loadModel(const char* fileName, CommonFileIOInterface* fileIO)
{
	//todo(erwincoumans) move the file loading out of here
	char relativeFileName[1024];
	if (!fileIO->findResourcePath(fileName, relativeFileName, 1024))
	{
		printf("Cannot find file %s\n", fileName);
	}
	else
	{
		m_model = new Model(relativeFileName);
	}
}

void TinyRenderObjectData::registerMeshShape(const float* vertices, int numVertices, const int* indices, int numIndices, const Colorf& rgbaColor,
											 TGAImage* textureImage)
{
	if (0 == m_model)
	{
		{
			B3_PROFILE("setColorRGBA");

			m_model = new Model();
			m_model->setColorRGBA(rgbaColor);
		}
		if (textureImage)
		{
			{
				B3_PROFILE("setDiffuseTextureFromData");
				m_model->setDiffuseTextureFromData(textureImage);
			}
		}
		else
		{
			/*char relativeFileName[1024];
			if (b3ResourcePath::findResourcePath("floor_diffuse.tga", relativeFileName, 1024))
			{
				m_model->loadDiffuseTexture(relativeFileName);
			}
             */
		}
		{
			B3_PROFILE("reserveMemory");
			m_model->reserveMemory(numVertices, numIndices);
		}
		{
			B3_PROFILE("addVertex");
			for (int i = 0; i < numVertices; i++)
			{
				m_model->addVertex(vertices[i * GLInstanceVertex::Length],
								   vertices[i * GLInstanceVertex::Length + 1],
								   vertices[i * GLInstanceVertex::Length + 2],
								   vertices[i * GLInstanceVertex::Length + 4],
								   vertices[i * GLInstanceVertex::Length + 5],
								   vertices[i * GLInstanceVertex::Length + 6],
								   vertices[i * GLInstanceVertex::Length + 7],
								   vertices[i * GLInstanceVertex::Length + 8]);
			}
		}
		{
			B3_PROFILE("addTriangle");
			for (int i = 0; i < numIndices; i += 3)
			{
				m_model->addTriangle(indices[i], indices[i], indices[i],
									 indices[i + 1], indices[i + 1], indices[i + 1],
									 indices[i + 2], indices[i + 2], indices[i + 2]);
			}
		}
	}
}

void TinyRenderObjectData::registerMesh2(b3AlignedObjectArray<b3Vector3>& vertices, b3AlignedObjectArray<b3Vector3>& normals,
    b3AlignedObjectArray<int>& indices, CommonFileIOInterface* fileIO)
{
	if (0 == m_model)
	{
		int numVertices = vertices.size();
		int numIndices = indices.size();

		m_model = new Model();
		char relativeFileName[1024];
		if (fileIO->findResourcePath("floor_diffuse.tga", relativeFileName, 1024))
		{
			m_model->loadDiffuseTexture(relativeFileName);
		}

		for (int i = 0; i < numVertices; i++)
		{
			m_model->addVertex(vertices[i].x,
							   vertices[i].y,
							   vertices[i].z,
							   normals[i].x,
							   normals[i].y,
							   normals[i].z,
							   0.5, 0.5);
		}
		for (int i = 0; i < numIndices; i += 3)
		{
			m_model->addTriangle(indices[i], indices[i], indices[i],
								 indices[i + 1], indices[i + 1], indices[i + 1],
								 indices[i + 2], indices[i + 2], indices[i + 2]);
		}
	}
}

void TinyRenderObjectData::createCube(float halfExtentsX, float halfExtentsY, float halfExtentsZ, CommonFileIOInterface* fileIO)
{
	b3BulletDefaultFileIO defaultFileIO;

	if (fileIO==0)
	{
		fileIO = &defaultFileIO;
	}
	m_model = new Model();

	char relativeFileName[1024];
	if (fileIO->findResourcePath("floor_diffuse.tga", relativeFileName, 1024))
	{
		m_model->loadDiffuseTexture(relativeFileName);
	}

	int strideInBytes = GLInstanceVertex::Length * sizeof(float);
	int numVertices = sizeof(cube_vertices_textured) / strideInBytes;
	int numIndices = sizeof(cube_indices) / sizeof(int);

	for (int i = 0; i < numVertices; i++)
	{
		m_model->addVertex(halfExtentsX * cube_vertices_textured[i * GLInstanceVertex::Length],
						   halfExtentsY * cube_vertices_textured[i * GLInstanceVertex::Length + 1],
						   halfExtentsZ * cube_vertices_textured[i * GLInstanceVertex::Length + 2],
						   cube_vertices_textured[i * GLInstanceVertex::Length + 4],
						   cube_vertices_textured[i * GLInstanceVertex::Length + 5],
						   cube_vertices_textured[i * GLInstanceVertex::Length + 6],
						   cube_vertices_textured[i * GLInstanceVertex::Length + 7],
						   cube_vertices_textured[i * GLInstanceVertex::Length + 8]);
	}
	for (int i = 0; i < numIndices; i += 3)
	{
		m_model->addTriangle(cube_indices[i], cube_indices[i], cube_indices[i],
							 cube_indices[i + 1], cube_indices[i + 1], cube_indices[i + 1],
							 cube_indices[i + 2], cube_indices[i + 2], cube_indices[i + 2]);
	}
}

TinyRenderObjectData::~TinyRenderObjectData()
{
	delete m_model;
}

static bool equals(const Vec4f& vA, const Vec4f& vB)
{
	return false;
}

static void clipEdge(const mat<4, 3, float>& triangleIn, int vertexIndexA, int vertexIndexB, b3AlignedObjectArray<Vec4f>& vertices)
{
	Vec4f v0New = triangleIn.col(vertexIndexA);
	Vec4f v1New = triangleIn.col(vertexIndexB);

	bool v0Inside = v0New[3] > 0.f && v0New[2] > -v0New[3];
	bool v1Inside = v1New[3] > 0.f && v1New[2] > -v1New[3];

	if (v0Inside && v1Inside)
	{
	}
	else if (v0Inside || v1Inside)
	{
		float d0 = v0New[2] + v0New[3];
		float d1 = v1New[2] + v1New[3];
		float factor = 1.0 / (d1 - d0);
		Vec4f newVertex = (v0New * d1 - v1New * d0) * factor;
		if (v0Inside)
		{
			v1New = newVertex;
		}
		else
		{
			v0New = newVertex;
		}
	}
	else
	{
		return;
	}

	if (vertices.size() == 0 || !(equals(vertices[vertices.size() - 1], v0New)))
	{
		vertices.push_back(v0New);
	}

	vertices.push_back(v1New);
}

static bool clipTriangleAgainstNearplane(const mat<4, 3, float>& triangleIn, b3AlignedObjectArray<mat<4, 3, float> >& clippedTrianglesOut)
{
	//discard triangle if all vertices are behind near-plane
	if (triangleIn[3][0] < 0 && triangleIn[3][1] < 0 && triangleIn[3][2] < 0)
	{
		return true;
	}

	//accept triangle if all vertices are in front of the near-plane
	if (triangleIn[3][0] >= 0 && triangleIn[3][1] >= 0 && triangleIn[3][2] >= 0)
	{
		clippedTrianglesOut.push_back(triangleIn);
		return false;
	}

	Vec4f vtxCache[5];

	b3AlignedObjectArray<Vec4f> vertices;
	vertices.initializeFromBuffer(vtxCache, 0, 5);
	clipEdge(triangleIn, 0, 1, vertices);
	clipEdge(triangleIn, 1, 2, vertices);
	clipEdge(triangleIn, 2, 0, vertices);

	if (vertices.size() < 3)
		return true;

	if (equals(vertices[0], vertices[vertices.size() - 1]))
	{
		vertices.pop_back();
	}

	//create a fan of triangles
	for (int i = 1; i < vertices.size() - 1; i++)
	{
		mat<4, 3, float>& vtx = clippedTrianglesOut.expand();
		vtx.set_col(0, vertices[0]);
		vtx.set_col(1, vertices[i]);
		vtx.set_col(2, vertices[i + 1]);
	}
	return true;
}

void TinyRenderer::renderObject(TinyRenderObjectData& renderData)
{
	B3_PROFILE("renderObject");
	int width = renderData.m_rgbColorBuffer.get_width();
	int height = renderData.m_rgbColorBuffer.get_height();

	Vec3f light_dir_local = Vec3f(renderData.m_lightDirWorld[0], renderData.m_lightDirWorld[1], renderData.m_lightDirWorld[2]);
	Vec3f light_color = Vec3f(renderData.m_lightColor[0], renderData.m_lightColor[1], renderData.m_lightColor[2]);
	float light_distance = renderData.m_lightDistance;
	Model* model = renderData.m_model;
	if (0 == model)
		return;
	//discard invisible objects (zero alpha)
	if (model->getColorRGBA()[3] == 0)
		return;

	renderData.m_viewportMatrix = TinyHelper::viewport(0, 0, width, height);

	b3AlignedObjectArray<float>& zbuffer = renderData.m_depthBuffer;
	b3AlignedObjectArray<float>* shadowBufferPtr = renderData.m_shadowBuffer;
	int* segmentationMaskBufferPtr = (renderData.m_segmentationMaskBufferPtr && renderData.m_segmentationMaskBufferPtr->size()) ? &renderData.m_segmentationMaskBufferPtr->at(0) : 0;

	TGAImage& frame = renderData.m_rgbColorBuffer;

	{
		// light target is set to be the origin, and the up direction is set to be vertical up.
		Matrixf lightViewMatrix = TinyHelper::lookat(light_dir_local * light_distance, Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0));
		Matrixf lightModelViewMatrix = lightViewMatrix * renderData.m_modelMatrix;
		Matrixf modelViewMatrix = renderData.m_viewMatrix * renderData.m_modelMatrix;
		Vec3f localScaling(renderData.m_localScaling[0], renderData.m_localScaling[1], renderData.m_localScaling[2]);
		Matrixf viewMatrixInv = renderData.m_viewMatrix.invert();
		b3Vector3 P(viewMatrixInv[0][3], viewMatrixInv[1][3], viewMatrixInv[2][3]);

		Shader shader(model, light_dir_local, light_color, modelViewMatrix, lightModelViewMatrix, renderData.m_projectionMatrix, renderData.m_modelMatrix, renderData.m_viewportMatrix, 
            localScaling, model->getColorRGBA(), width, height, shadowBufferPtr, renderData.m_lightAmbientCoeff, renderData.m_lightDiffuseCoeff, renderData.m_lightSpecularCoeff);

		{
			B3_PROFILE("face");

			for (int i = 0; i < model->nfaces(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					shader.vertex(i, j);
				}

				// backface culling
				b3Vector3 v0(shader.world_tri.col(0)[0], shader.world_tri.col(0)[1], shader.world_tri.col(0)[2]);
				b3Vector3 v1(shader.world_tri.col(1)[0], shader.world_tri.col(1)[1], shader.world_tri.col(1)[2]);
				b3Vector3 v2(shader.world_tri.col(2)[0], shader.world_tri.col(2)[1], shader.world_tri.col(2)[2]);
				b3Vector3 N = (v1 - v0).cross(v2 - v0);
				if ((v0 - P).dot(N) >= 0)
					continue;

				mat<4, 3, float> stackTris[3];

				b3AlignedObjectArray<mat<4, 3, float> > clippedTriangles;
				clippedTriangles.initializeFromBuffer(stackTris, 0, 3);

				bool hasClipped = clipTriangleAgainstNearplane(shader.varying_tri, clippedTriangles);

				if (hasClipped)
				{
					for (int t = 0; t < clippedTriangles.size(); t++)
					{
                        TinyHelper::triangleClipped(clippedTriangles[t], shader.varying_tri, shader, frame, &zbuffer[0], 
                            segmentationMaskBufferPtr, renderData.m_viewportMatrix, renderData.m_objectIndex + ((renderData.m_linkIndex + 1) << 24));
					}
				}
				else
				{
                    TinyHelper::triangle(shader.varying_tri, shader, frame, &zbuffer[0], 
                        segmentationMaskBufferPtr, renderData.m_viewportMatrix, renderData.m_objectIndex + ((renderData.m_linkIndex + 1) << 24));
				}
			}
		}
	}
}

void TinyRenderer::renderObjectDepth(TinyRenderObjectData& renderData)
{
	int width = renderData.m_rgbColorBuffer.get_width();
	int height = renderData.m_rgbColorBuffer.get_height();

	Vec3f light_dir_local = Vec3f(renderData.m_lightDirWorld[0], renderData.m_lightDirWorld[1], renderData.m_lightDirWorld[2]);
	float light_distance = renderData.m_lightDistance;
	Model* model = renderData.m_model;
	if (0 == model)
		return;

	renderData.m_viewportMatrix = TinyHelper::viewport(0, 0, width, height);

	float* shadowBufferPtr = (renderData.m_shadowBuffer && renderData.m_shadowBuffer->size()) ? &renderData.m_shadowBuffer->at(0) : 0;
	int* segmentationMaskBufferPtr = 0;

	TGAImage depthFrame(width, height, TGAImage::RGB);

	{
		// light target is set to be the origin, and the up direction is set to be vertical up.
		Matrixf lightViewMatrix = TinyHelper::lookat(light_dir_local * light_distance, Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0));
		Matrixf lightModelViewMatrix = lightViewMatrix * renderData.m_modelMatrix;
		Matrixf lightViewProjectionMatrix = renderData.m_projectionMatrix;
		Vec3f localScaling(renderData.m_localScaling[0], renderData.m_localScaling[1], renderData.m_localScaling[2]);

		DepthShader shader(model, lightModelViewMatrix, lightViewProjectionMatrix, renderData.m_modelMatrix, localScaling, light_distance);
		for (int i = 0; i < model->nfaces(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				shader.vertex(i, j);
			}

			mat<4, 3, float> stackTris[3];

			b3AlignedObjectArray<mat<4, 3, float> > clippedTriangles;
			clippedTriangles.initializeFromBuffer(stackTris, 0, 3);

			bool hasClipped = clipTriangleAgainstNearplane(shader.varying_tri, clippedTriangles);

			if (hasClipped)
			{
				for (int t = 0; t < clippedTriangles.size(); t++)
				{
                    TinyHelper::triangleClipped(clippedTriangles[t], shader.varying_tri, shader, depthFrame, 
                        shadowBufferPtr, segmentationMaskBufferPtr, renderData.m_viewportMatrix, renderData.m_objectIndex);
				}
			}
			else
			{
                TinyHelper::triangle(shader.varying_tri, shader, depthFrame, 
                    shadowBufferPtr, segmentationMaskBufferPtr, renderData.m_viewportMatrix, renderData.m_objectIndex);
			}
		}
	}
}
