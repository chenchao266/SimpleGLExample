#ifndef _LOAD_SHADER_H
#define _LOAD_SHADER_H

//#include "OpenGLInclude.h"
#include "Bullet3Common/geometry.h"
#include "Bullet3Common/PrimVertex.h"
    unsigned gltLoadShaderPair(const char *szVertexProg, const char *szFragmentProg);

    void checkError(const char* functionName);

    struct BufferObject
    {
        unsigned VertexBufferObject = 0;
        unsigned VertexArrayObject = 0;
        unsigned IndexVbo = 0;
        void create(int sz);
        void clear();
        void toData(int numvertices, const void* vertices);
        void setAttrib();
        void toElement(int numIndices, const void* indices, bool bdynamic = true);
        void Bind();
        void unBind();
    };
    enum TexParamE {
        TEX_LINEAR_MIPMAP,
        TEX_LINEAR,
        TEX_NEAREST,
    };

    enum TexColorE {
        TEX_RGB,
        TEX_RGBA,
        TEX_GRAY,
        TEX_DEPTH,
    };

    enum TexBitE {
        TEX_8,
        TEX_16,
        TEX_32,
        TEX_16F,
        TEX_32F,
    };

    struct TextureObject
    { 
        unsigned texId;
        unsigned width;
        unsigned height;
        void* data;
        void create(unsigned w, unsigned h);
        void setBorder(float* clr);
        void setParam(TexParamE type);
        void toData2(TexColorE clr, TexBitE bit);
        void bind();
        void unbind();
    };
    struct ProgramBase 
    {
        const char* VertexShader, *FragmentShader;
        unsigned Shader;
        int ModelViewMatrix = 0;
        int ProjectionMatrix = 0;
        void create(const char* vs, const char* fs);
        int getLoc(const char*);
        int getAttribLoc(const char*);
 
        void link();
        void use();
        static void unuse();
        void setUniform(int, float);
        void setUniform(int, float, float);
        void setUniform(int, float, float, float);
        void setUniform(int, float, float, float, float);
        void setUniform(int, int);
        void setUniform(int, int, int);
        void setUniform(int, int, int, int);
        void setUniform(int, int, int, int, int);


        void setUniform(int, const float*, unsigned n);
        void setUniform(int, const int*, unsigned n);
        void setUniform(int, const Vec2f&);
        void setUniform(int, const Vec3f&);
        void setUniform(int, const Vec4f&);
        void setUniform(int, const Vec2i&);
        void setUniform(int, const Vec3i&);
        void setUniform(int, const Vec4i&);
        void setUniformMatrix4(int, const Matrixf&);
        void setUniformMatrix4(int, const float*);

        static void setVertexAttrib(int attr, int len, int stride, int div, const void * size);

        virtual ~ProgramBase() {}
    };
#endif  //_LOAD_SHADER_H
