#ifndef B3_IMPORT_MESH_UTILITY_H
#define B3_IMPORT_MESH_UTILITY_H

#include <string>
class TGAImage;

enum b3ImportMeshDataFlags
{
	B3_IMPORT_MESH_HAS_RGBA_COLOR=1,
	B3_IMPORT_MESH_HAS_SPECULAR_COLOR=2,
};
struct CommonFileIOInterface;
struct b3ImportMeshData
{
	struct GLInstanceGraphicsShape* m_gfxShape;

	TGAImage* m_texture;  //in 3 component 8-bit RGB data
	bool m_isCached;
 
	float m_rgbaColor[4];
    float m_specularColor[4];
	int m_flags;

	b3ImportMeshData()
		:m_gfxShape(0),
        m_texture(0),
		m_isCached(false), 
		m_flags(0)
	{
	}

};

class b3ImportMeshUtility
{
public:
	static bool loadAndRegisterMeshFromFileInternal(const std::string& fileName, b3ImportMeshData& meshData,  
        CommonFileIOInterface* fileIO);
};

#endif  //B3_IMPORT_MESH_UTILITY_H
