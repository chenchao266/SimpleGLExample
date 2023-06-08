#ifndef TINY_RENDERER_H
#define TINY_RENDERER_H

#include "Bullet3Common/geometry.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "Bullet3Common/b3Vector3.h"
 

#include "tgaimage.h"
struct CommonFileIOInterface;
struct TinyRenderObjectData
{
	//Camera
	Matrixf m_viewMatrix;
	Matrixf m_projectionMatrix;
	Matrixf m_viewportMatrix;
	b3Vector3 m_localScaling;
	b3Vector3 m_lightDirWorld;
	b3Vector3 m_lightColor;
	float m_lightDistance;
	float m_lightAmbientCoeff;
	float m_lightDiffuseCoeff;
	float m_lightSpecularCoeff;

	//Model (vertices, indices, textures, shader)
	Matrixf m_modelMatrix;
	class Model* m_model;
	//class IShader* m_shader; todo(erwincoumans) expose the shader, for now we use a default shader

	//Output

	TGAImage& m_rgbColorBuffer;
	b3AlignedObjectArray<float>& m_depthBuffer;              //required, hence a reference
	b3AlignedObjectArray<float>* m_shadowBuffer;             //optional, hence a pointer
	b3AlignedObjectArray<int>* m_segmentationMaskBufferPtr;  //optional, hence a pointer

	TinyRenderObjectData(TGAImage& rgbColorBuffer, b3AlignedObjectArray<float>& depthBuffer);
	TinyRenderObjectData(TGAImage& rgbColorBuffer, b3AlignedObjectArray<float>& depthBuffer, b3AlignedObjectArray<int>* segmentationMaskBuffer, int objectIndex);
	TinyRenderObjectData(TGAImage& rgbColorBuffer, b3AlignedObjectArray<float>& depthBuffer, b3AlignedObjectArray<float>* shadowBuffer);
	TinyRenderObjectData(TGAImage& rgbColorBuffer, b3AlignedObjectArray<float>& depthBuffer, b3AlignedObjectArray<float>* shadowBuffer, 
        b3AlignedObjectArray<int>* segmentationMaskBuffer, int objectIndex, int linkIndex);
	virtual ~TinyRenderObjectData();

	void loadModel(const char* fileName,   CommonFileIOInterface* fileIO = nullptr);
	void createCube(float HalfExtentsX, float HalfExtentsY, float HalfExtentsZ,   CommonFileIOInterface* fileIO=0);
	void registerMeshShape(const float* vertices, int numVertices, const int* indices, int numIndices, const Colorf& rgbaColor,
						   TGAImage* texture = 0);

	void registerMesh2(b3AlignedObjectArray<b3Vector3>& vertices, b3AlignedObjectArray<b3Vector3>& normals, b3AlignedObjectArray<int>& indices, 
          CommonFileIOInterface* fileIO);

	void* m_userData;
	int m_userIndex;
	int m_objectIndex;
	int m_linkIndex;
};

class TinyRenderer
{
public:
	static void renderObjectDepth(TinyRenderObjectData& renderData);
	static void renderObject(TinyRenderObjectData& renderData);
};

#endif  // TINY_RENDERER_Hbla
