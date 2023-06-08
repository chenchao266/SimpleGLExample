#ifndef PRIM_INTERNAL_DATA
#define PRIM_INTERNAL_DATA

#include "LoadShader.h"
//#include "OpenGLInclude.h"

struct PrimInternalData : public ProgramBase
{
 
	int m_positionUniform;
	int m_colourAttribute;
	int m_positionAttribute;
	int m_textureAttribute;

    BufferObject m_buf1;
    BufferObject m_buf2;
     
	unsigned m_texturehandle;
};

#endif  //PRIM_INTERNAL_DATA
