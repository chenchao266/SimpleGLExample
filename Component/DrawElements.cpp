#include "DrawElements.h"


unsigned int DrawElements::getNumPrimitives() const
{
    switch (_mode)
    {
    case(POINTS): return getNumIndices();
    case(LINES): return getNumIndices() / 2;
    case(TRIANGLES): return getNumIndices() / 3;
    case(QUADS): return getNumIndices() / 4;
    case(LINE_STRIP):
    case(LINE_LOOP):
    case(TRIANGLE_STRIP):
    case(TRIANGLE_FAN):
    case(QUAD_STRIP):
    case(PATCHES):
    case(POLYGON): return (getNumIndices() > 0) ? 1 : 0;
    }
    return 0;
}


