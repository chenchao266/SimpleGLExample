﻿#include "b3ImportMeshUtility.h"

#include <vector>
#include "tiny_obj_loader.h"
#include "Bullet3Common/b3Vector3.h"
#include "Wavefront2GLInstanceGraphicsShape.h"
#include "b3ResourcePath.h"
#include "Bullet3Common/b3FileUtils.h"
 
#include "LoadMeshFromObj.h"
#include "Bullet3Common/b3HashMap.h"
#include "CommonInterfaces/CommonFileIOInterface.h"
#include "OpenGLWindow/tgaimage.h"
 
#include "stb_image/stb_image.h"
 
//#include "stb_image_write.h"
 
struct CachedTextureResult
{
	std::string m_textureName;
    TGAImage image;
	CachedTextureResult()
        : image(3)
	{
	}
};

static b3HashMap<b3HashString, CachedTextureResult> gCachedTextureResults;
struct CachedTextureManager
{
	CachedTextureManager()
	{
	}
	virtual ~CachedTextureManager()
	{
		for (int i = 0; i < gCachedTextureResults.size(); i++)
		{
			CachedTextureResult* res = gCachedTextureResults.getAtIndex(i);
			if (res)
			{
                ;
			}
		}
	}
};
static CachedTextureManager sTexCacheMgr;

bool b3ImportMeshUtility::loadAndRegisterMeshFromFileInternal(const std::string& fileName, b3ImportMeshData& meshData, 
    CommonFileIOInterface* fileIO)
{
	B3_PROFILE("loadAndRegisterMeshFromFileInternal");
	meshData.m_gfxShape = 0;
	meshData.m_texture = 0;
	meshData.m_flags = 0;
	meshData.m_isCached = false;

	char relativeFileName[1024];
	if (fileIO->findResourcePath(fileName.c_str(), relativeFileName, 1024))
	{
		char pathPrefix[1024];

		b3FileUtils::extractPath(relativeFileName, pathPrefix, 1024);
		b3Vector3 shift(0, 0, 0);

		std::vector<tinyobj::shape_t> shapes;
		tinyobj::attrib_t attribute;
		{
			B3_PROFILE("tinyobj::LoadObj");
			std::string err = LoadFromCachedOrFromObj(attribute, shapes, relativeFileName, pathPrefix, fileIO);
			//std::string err = tinyobj::LoadObj(shapes, relativeFileName, pathPrefix);
		}

		GLInstanceGraphicsShape* gfxShape = btgCreateGraphicsShapeFromWavefrontObj(attribute, shapes);
		{
			B3_PROFILE("Load Texture");
			//int textureIndex = -1;
			//try to load some texture
			for (int i = 0; meshData.m_texture == 0 && i < shapes.size(); i++)
			{
				const tinyobj::shape_t& shape = shapes[i];
				meshData.m_rgbaColor[0] = shape.material.diffuse[0];
				meshData.m_rgbaColor[1] = shape.material.diffuse[1];
				meshData.m_rgbaColor[2] = shape.material.diffuse[2];
				meshData.m_rgbaColor[3] = shape.material.transparency;
				meshData.m_flags |= B3_IMPORT_MESH_HAS_RGBA_COLOR;

				meshData.m_specularColor[0] = shape.material.specular[0];
				meshData.m_specularColor[1] = shape.material.specular[1];
				meshData.m_specularColor[2] = shape.material.specular[2];
				meshData.m_specularColor[3] = 1;
				meshData.m_flags |= B3_IMPORT_MESH_HAS_SPECULAR_COLOR;
				
				if (shape.material.diffuse_texname.length() > 0)
				{
                    TGAImage* image = nullptr;
					const char* filename = shape.material.diffuse_texname.c_str();
		 

					const char* prefix[] = {pathPrefix, "./", "./data/", "../data/", "../../data/", "../../../data/", "../../../../data/"};
					int numprefix = sizeof(prefix) / sizeof(const char*);

					for (int i = 0; !image && i < numprefix; i++)
					{
						char relativeFileName[1024];
						sprintf(relativeFileName, "%s%s", prefix[i], filename);
						char relativeFileName2[1024];
						if (fileIO->findResourcePath(relativeFileName, relativeFileName2, 1024))
						{
							if (b3IsFileCachingEnabled())
							{
								CachedTextureResult* texture = gCachedTextureResults[relativeFileName];
								if (texture)
								{
									image = &texture->image;  
									meshData.m_texture = image;
									meshData.m_isCached = true;
								}
							}

							if (image == 0)
							{
                                image = new TGAImage(3);
                                image->read_tga_file(filename, fileIO);
								meshData.m_texture = image;

								if (image)
								{ 
									if (b3IsFileCachingEnabled())
									{
										CachedTextureResult result;
										result.m_textureName = relativeFileName;									 
										result.image.swap(*image);
                                        delete image;
                                        image = nullptr;
                                      
										meshData.m_isCached = true;
										gCachedTextureResults.insert(relativeFileName, result); 
                                        meshData.m_texture = &(gCachedTextureResults[relativeFileName]->image);
									}
								}
								else
								{
									b3Warning("Unsupported texture image format [%s]\n", relativeFileName);

									break;
								}
							}
						}
						else
						{
							b3Warning("not found [%s]\n", relativeFileName);
						}
					}
				}
			}
		}
		meshData.m_gfxShape = gfxShape;
		return true;
	}
	else
	{
		b3Warning("Cannot find %s\n", fileName.c_str());
	}

	return false;
}
