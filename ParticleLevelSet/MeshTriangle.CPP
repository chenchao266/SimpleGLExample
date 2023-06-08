#include "glad/gl.h"
//#include "common.h"
#include "MeshTriangle.h"
 
 
	//glVertex3dv(data);
 
    //glNormal3dv(data);
 
    //glColor3dv(data);
 
MeshTriangle::MeshTriangle ()
{
}

MeshTriangle::MeshTriangle (unsigned int v0, unsigned int v1, unsigned int v2)
{
	indices[0] = v0;
	indices[1] = v1;
	indices[2] = v2;
}

MeshTriangle::~MeshTriangle ()
{
}

unsigned int& MeshTriangle::operator [] (int index)
{
	return indices[index];
}

std::ostream& operator << (std::ostream& out, const MeshTriangle& toPrint)
{
	out << toPrint.indices[0] << ", " 
		<< toPrint.indices[1] << ", " 
		<< toPrint.indices[2];
	return out;
}