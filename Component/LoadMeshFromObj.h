#ifndef LOAD_MESH_FROM_OBJ_H
#define LOAD_MESH_FROM_OBJ_H

struct GLInstanceGraphicsShape;

#include "tiny_obj_loader.h"

int b3IsFileCachingEnabled();
void b3EnableFileCaching(int enable);

std::string LoadFromCachedOrFromObj(
	tinyobj::attrib_t& attribute,
	std::vector<tinyobj::shape_t>& shapes,  // [output]
	const char* filename,
	const char* mtl_basepath,
	  CommonFileIOInterface* fileIO);

GLInstanceGraphicsShape* LoadMeshFromObj(const char* relativeFileName, const char* materialPrefixPath, 
    CommonFileIOInterface* fileIO);



#endif  //LOAD_MESH_FROM_OBJ_H
