/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "btStridingMeshInterface.h"
#include "Bullet3Common/btSerializer.h"

btStridingMeshInterface::~btStridingMeshInterface()
{
}

void btStridingMeshInterface::InternalProcessAllTriangles(btInternalTriangleIndexCallback* callback, const b3Vector3& aabbMin, const b3Vector3& aabbMax) const
{
	(void)aabbMin;
	(void)aabbMax;
	int numtotalphysicsverts = 0;
	int part, graphicssubparts = getNumSubParts();
	const unsigned char* vertexbase;
	const unsigned char* indexbase;
	int indexstride;
	PHY_ScalarType type;
	PHY_ScalarType gfxindextype;
	int stride, numverts, numtriangles;
	int gfxindex;
	b3Vector3 triangle[3];

	b3Vector3 meshScaling = getScaling();

	///if the number of parts is big, the performance might drop due to the innerloop switch on indextype
	for (part = 0; part < graphicssubparts; part++)
	{
		getLockedReadOnlyVertexIndexBase(&vertexbase, numverts, type, stride, &indexbase, indexstride, numtriangles, gfxindextype, part);
		numtotalphysicsverts += numtriangles * 3;  //upper bound

		///unlike that developers want to pass in double-precision meshes in single-precision Bullet build
		///so disable this feature by default
		///see patch http://code.google.com/p/bullet/issues/detail?id=213

		switch (type)
		{
			case PHY_FLOAT:
			{
				float* graphicsbase;

				switch (gfxindextype)
				{
					case PHY_INTEGER:
					{
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned int* tri_indices = (unsigned int*)(indexbase + gfxindex * indexstride);
							graphicsbase = (float*)(vertexbase + tri_indices[0] * stride);
							triangle[0].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (float*)(vertexbase + tri_indices[1] * stride);
							triangle[1].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (float*)(vertexbase + tri_indices[2] * stride);
							triangle[2].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							callback->internalProcessTriangleIndex(triangle, part, gfxindex);
						}
						break;
					}
					case PHY_SHORT:
					{
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned short int* tri_indices = (unsigned short int*)(indexbase + gfxindex * indexstride);
							graphicsbase = (float*)(vertexbase + tri_indices[0] * stride);
							triangle[0].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (float*)(vertexbase + tri_indices[1] * stride);
							triangle[1].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (float*)(vertexbase + tri_indices[2] * stride);
							triangle[2].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							callback->internalProcessTriangleIndex(triangle, part, gfxindex);
						}
						break;
					}
					case PHY_UCHAR:
					{
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned char* tri_indices = (unsigned char*)(indexbase + gfxindex * indexstride);
							graphicsbase = (float*)(vertexbase + tri_indices[0] * stride);
							triangle[0].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (float*)(vertexbase + tri_indices[1] * stride);
							triangle[1].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (float*)(vertexbase + tri_indices[2] * stride);
							triangle[2].setValue(graphicsbase[0] * meshScaling.getX(), graphicsbase[1] * meshScaling.getY(), graphicsbase[2] * meshScaling.getZ());
							callback->internalProcessTriangleIndex(triangle, part, gfxindex);
						}
						break;
					}
					default:
						b3Assert((gfxindextype == PHY_INTEGER) || (gfxindextype == PHY_SHORT));
				}
				break;
			}

			case PHY_DOUBLE:
			{
				double* graphicsbase;

				switch (gfxindextype)
				{
					case PHY_INTEGER:
					{
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned int* tri_indices = (unsigned int*)(indexbase + gfxindex * indexstride);
							graphicsbase = (double*)(vertexbase + tri_indices[0] * stride);
							triangle[0].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (double*)(vertexbase + tri_indices[1] * stride);
							triangle[1].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (double*)(vertexbase + tri_indices[2] * stride);
							triangle[2].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							callback->internalProcessTriangleIndex(triangle, part, gfxindex);
						}
						break;
					}
					case PHY_SHORT:
					{
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned short int* tri_indices = (unsigned short int*)(indexbase + gfxindex * indexstride);
							graphicsbase = (double*)(vertexbase + tri_indices[0] * stride);
							triangle[0].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (double*)(vertexbase + tri_indices[1] * stride);
							triangle[1].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (double*)(vertexbase + tri_indices[2] * stride);
							triangle[2].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							callback->internalProcessTriangleIndex(triangle, part, gfxindex);
						}
						break;
					}
					case PHY_UCHAR:
					{
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned char* tri_indices = (unsigned char*)(indexbase + gfxindex * indexstride);
							graphicsbase = (double*)(vertexbase + tri_indices[0] * stride);
							triangle[0].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (double*)(vertexbase + tri_indices[1] * stride);
							triangle[1].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							graphicsbase = (double*)(vertexbase + tri_indices[2] * stride);
							triangle[2].setValue((b3Scalar)graphicsbase[0] * meshScaling.getX(), (b3Scalar)graphicsbase[1] * meshScaling.getY(), (b3Scalar)graphicsbase[2] * meshScaling.getZ());
							callback->internalProcessTriangleIndex(triangle, part, gfxindex);
						}
						break;
					}
					default:
						b3Assert((gfxindextype == PHY_INTEGER) || (gfxindextype == PHY_SHORT));
				}
				break;
			}
			default:
				b3Assert((type == PHY_FLOAT) || (type == PHY_DOUBLE));
		}

		unLockReadOnlyVertexBase(part);
	}
}

void btStridingMeshInterface::calculateAabbBruteForce(b3Vector3& aabbMin, b3Vector3& aabbMax)
{
	struct AabbCalculationCallback : public btInternalTriangleIndexCallback
	{
		b3Vector3 m_aabbMin;
		b3Vector3 m_aabbMax;

		AabbCalculationCallback()
		{
			m_aabbMin.setValue(b3Scalar(B3_LARGE_FLOAT), b3Scalar(B3_LARGE_FLOAT), b3Scalar(B3_LARGE_FLOAT));
			m_aabbMax.setValue(b3Scalar(-B3_LARGE_FLOAT), b3Scalar(-B3_LARGE_FLOAT), b3Scalar(-B3_LARGE_FLOAT));
		}

		virtual void internalProcessTriangleIndex(b3Vector3* triangle, int partId, int triangleIndex)
		{
			(void)partId;
			(void)triangleIndex;

			m_aabbMin.setMin(triangle[0]);
			m_aabbMax.setMax(triangle[0]);
			m_aabbMin.setMin(triangle[1]);
			m_aabbMax.setMax(triangle[1]);
			m_aabbMin.setMin(triangle[2]);
			m_aabbMax.setMax(triangle[2]);
		}
	};

	//first calculate the total aabb for all triangles
	AabbCalculationCallback aabbCallback;
	aabbMin.setValue(b3Scalar(-B3_LARGE_FLOAT), b3Scalar(-B3_LARGE_FLOAT), b3Scalar(-B3_LARGE_FLOAT));
	aabbMax.setValue(b3Scalar(B3_LARGE_FLOAT), b3Scalar(B3_LARGE_FLOAT), b3Scalar(B3_LARGE_FLOAT));
	InternalProcessAllTriangles(&aabbCallback, aabbMin, aabbMax);

	aabbMin = aabbCallback.m_aabbMin;
	aabbMax = aabbCallback.m_aabbMax;
}

///fills the dataBuffer and returns the struct name (and 0 on failure)
const char* btStridingMeshInterface::serialize(void* dataBuffer, btSerializer* serializer) const
{
	btStridingMeshInterfaceData* trimeshData = (btStridingMeshInterfaceData*)dataBuffer;

	trimeshData->m_numMeshParts = getNumSubParts();

	//void* uniquePtr = 0;

	trimeshData->m_meshPartsPtr = 0;

	if (trimeshData->m_numMeshParts)
	{
		btChunk* chunk = serializer->allocate(sizeof(btMeshPartData), trimeshData->m_numMeshParts);
		btMeshPartData* memPtr = (btMeshPartData*)chunk->m_oldPtr;
		trimeshData->m_meshPartsPtr = (btMeshPartData*)serializer->getUniquePointer(memPtr);

		//	int numtotalphysicsverts = 0;
		int part, graphicssubparts = getNumSubParts();
		const unsigned char* vertexbase;
		const unsigned char* indexbase;
		int indexstride;
		PHY_ScalarType type;
		PHY_ScalarType gfxindextype;
		int stride, numverts, numtriangles;
		int gfxindex;
		//	b3Vector3 triangle[3];

		//	b3Vector3 meshScaling = getScaling();

		///if the number of parts is big, the performance might drop due to the innerloop switch on indextype
		for (part = 0; part < graphicssubparts; part++, memPtr++)
		{
			getLockedReadOnlyVertexIndexBase(&vertexbase, numverts, type, stride, &indexbase, indexstride, numtriangles, gfxindextype, part);
			memPtr->m_numTriangles = numtriangles;  //indices = 3*numtriangles
			memPtr->m_numVertices = numverts;
			memPtr->m_indices16 = 0;
			memPtr->m_indices32 = 0;
			memPtr->m_3indices16 = 0;
			memPtr->m_3indices8 = 0;
			memPtr->m_vertices3f = 0;
			memPtr->m_vertices3d = 0;

			switch (gfxindextype)
			{
				case PHY_INTEGER:
				{
					int numindices = numtriangles * 3;

					if (numindices)
					{
						btChunk* chunk = serializer->allocate(sizeof(btIntIndexData), numindices);
						btIntIndexData* tmpIndices = (btIntIndexData*)chunk->m_oldPtr;
						memPtr->m_indices32 = (btIntIndexData*)serializer->getUniquePointer(tmpIndices);
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned int* tri_indices = (unsigned int*)(indexbase + gfxindex * indexstride);
							tmpIndices[gfxindex * 3].m_value = tri_indices[0];
							tmpIndices[gfxindex * 3 + 1].m_value = tri_indices[1];
							tmpIndices[gfxindex * 3 + 2].m_value = tri_indices[2];
						}
						serializer->finalizeChunk(chunk, "btIntIndexData", BT_ARRAY_CODE, (void*)chunk->m_oldPtr);
					}
					break;
				}
				case PHY_SHORT:
				{
					if (numtriangles)
					{
						btChunk* chunk = serializer->allocate(sizeof(btShortIntIndexTripletData), numtriangles);
						btShortIntIndexTripletData* tmpIndices = (btShortIntIndexTripletData*)chunk->m_oldPtr;
						memPtr->m_3indices16 = (btShortIntIndexTripletData*)serializer->getUniquePointer(tmpIndices);
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned short int* tri_indices = (unsigned short int*)(indexbase + gfxindex * indexstride);
							tmpIndices[gfxindex].m_values[0] = tri_indices[0];
							tmpIndices[gfxindex].m_values[1] = tri_indices[1];
							tmpIndices[gfxindex].m_values[2] = tri_indices[2];
							// Fill padding with zeros to appease msan.
							tmpIndices[gfxindex].m_pad[0] = 0;
							tmpIndices[gfxindex].m_pad[1] = 0;
						}
						serializer->finalizeChunk(chunk, "btShortIntIndexTripletData", BT_ARRAY_CODE, (void*)chunk->m_oldPtr);
					}
					break;
				}
				case PHY_UCHAR:
				{
					if (numtriangles)
					{
						btChunk* chunk = serializer->allocate(sizeof(btCharIndexTripletData), numtriangles);
						btCharIndexTripletData* tmpIndices = (btCharIndexTripletData*)chunk->m_oldPtr;
						memPtr->m_3indices8 = (btCharIndexTripletData*)serializer->getUniquePointer(tmpIndices);
						for (gfxindex = 0; gfxindex < numtriangles; gfxindex++)
						{
							unsigned char* tri_indices = (unsigned char*)(indexbase + gfxindex * indexstride);
							tmpIndices[gfxindex].m_values[0] = tri_indices[0];
							tmpIndices[gfxindex].m_values[1] = tri_indices[1];
							tmpIndices[gfxindex].m_values[2] = tri_indices[2];
							// Fill padding with zeros to appease msan.
							tmpIndices[gfxindex].m_pad = 0;
						}
						serializer->finalizeChunk(chunk, "btCharIndexTripletData", BT_ARRAY_CODE, (void*)chunk->m_oldPtr);
					}
					break;
				}
				default:
				{
					b3Assert(0);
					//unknown index type
				}
			}

			switch (type)
			{
				case PHY_FLOAT:
				{
					float* graphicsbase;

					if (numverts)
					{
						btChunk* chunk = serializer->allocate(sizeof(b3Vector3FloatData), numverts);
						b3Vector3FloatData* tmpVertices = (b3Vector3FloatData*)chunk->m_oldPtr;
						memPtr->m_vertices3f = (b3Vector3FloatData*)serializer->getUniquePointer(tmpVertices);
						for (int i = 0; i < numverts; i++)
						{
							graphicsbase = (float*)(vertexbase + i * stride);
							tmpVertices[i].m_floats[0] = graphicsbase[0];
							tmpVertices[i].m_floats[1] = graphicsbase[1];
							tmpVertices[i].m_floats[2] = graphicsbase[2];
						}
						serializer->finalizeChunk(chunk, "b3Vector3FloatData", BT_ARRAY_CODE, (void*)chunk->m_oldPtr);
					}
					break;
				}

				case PHY_DOUBLE:
				{
					if (numverts)
					{
						btChunk* chunk = serializer->allocate(sizeof(b3Vector3DoubleData), numverts);
						b3Vector3DoubleData* tmpVertices = (b3Vector3DoubleData*)chunk->m_oldPtr;
						memPtr->m_vertices3d = (b3Vector3DoubleData*)serializer->getUniquePointer(tmpVertices);
						for (int i = 0; i < numverts; i++)
						{
							double* graphicsbase = (double*)(vertexbase + i * stride);  //for now convert to float, might leave it at double
							tmpVertices[i].m_floats[0] = graphicsbase[0];
							tmpVertices[i].m_floats[1] = graphicsbase[1];
							tmpVertices[i].m_floats[2] = graphicsbase[2];
						}
						serializer->finalizeChunk(chunk, "b3Vector3DoubleData", BT_ARRAY_CODE, (void*)chunk->m_oldPtr);
					}
					break;
				}

				default:
					b3Assert((type == PHY_FLOAT) || (type == PHY_DOUBLE));
			}

			unLockReadOnlyVertexBase(part);
		}

		serializer->finalizeChunk(chunk, "btMeshPartData", BT_ARRAY_CODE, chunk->m_oldPtr);
	}

	// Fill padding with zeros to appease msan.
	memset(trimeshData->m_padding, 0, sizeof(trimeshData->m_padding));

	m_scaling.serializeFloat(trimeshData->m_scaling);
	return "btStridingMeshInterfaceData";
}
