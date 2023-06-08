#ifndef _GL_Vertex_H
#define _GL_Vertex_H

#include "Bullet3Common/geometry.h"
#include <stdint.h>
enum VertexContentE{
    VERTEX_POSITION = 1,
    VERTEX_COLOR = 2,
    VERTEX_TEX = 4,
    VERTEX_NORMAL = 8,
};
struct PrimVertex
{
    const static int flag = VERTEX_POSITION | VERTEX_COLOR | VERTEX_TEX;
    PrimVertex()
    {
    }
	PrimVertex(const Vec4f& p, const Vec4f& c, const Vec2f& u)
		: position(p),
		  colour(c),
		  uv(u)
	{
	}
 

	Vec4f position;
	Vec4f colour;
	Vec2f uv;
    const static int Length = 10;
};

struct GLInstanceVertex
{
    const static int flag = VERTEX_POSITION | VERTEX_NORMAL | VERTEX_TEX;
    Vec4f xyzw;
    Vec3f normal;
    Vec2f uv;
    const static int Length = 9;
};


struct PointerCaster
{
    union {
        int64_t m_baseIndex;
        void* m_pointer;
    };

    PointerCaster()
        : m_pointer(0)
    {
    }
};
#endif  // 
