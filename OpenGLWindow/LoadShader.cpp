#include "LoadShader.h"
#include "OpenGLInclude.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Load the shader from the source text
void gltLoadShaderSrc(const char *szShaderSrc, GLuint shader)
{
	GLchar *fsStringPtr[1];

	fsStringPtr[0] = (GLchar *)szShaderSrc;
	glShaderSource(shader, 1, (const GLchar **)fsStringPtr, NULL);
}

GLuint gltLoadShaderPair(const char *szVertexProg, const char *szFragmentProg)
{
	assert(glGetError() == GL_NO_ERROR);

	// Temporary Shader objects
	GLuint hVertexShader;
	GLuint hFragmentShader;
	GLuint hReturn = 0;
	GLint testVal;

	// Create shader objects
	hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	gltLoadShaderSrc(szVertexProg, hVertexShader);
	gltLoadShaderSrc(szFragmentProg, hFragmentShader);

	// Compile them
	glCompileShader(hVertexShader);
	assert(glGetError() == GL_NO_ERROR);

	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE)
	{
		char temp[256] = "";
		glGetShaderInfoLog(hVertexShader, 256, NULL, temp);
		fprintf(stderr, "Compile failed:\n%s\n", temp);
		assert(0);
		return 0;
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint)0;
	}

	assert(glGetError() == GL_NO_ERROR);

	glCompileShader(hFragmentShader);
	assert(glGetError() == GL_NO_ERROR);

	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE)
	{
		char temp[256] = "";
		glGetShaderInfoLog(hFragmentShader, 256, NULL, temp);
		fprintf(stderr, "Compile failed:\n%s\n", temp);
		assert(0);
		exit(EXIT_FAILURE);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint)0;
	}

	assert(glGetError() == GL_NO_ERROR);

	// Check for errors

	// Link them - assuming it works...
	hReturn = glCreateProgram();
	glAttachShader(hReturn, hVertexShader);
	glAttachShader(hReturn, hFragmentShader);

	glLinkProgram(hReturn);

	// These are no longer needed
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);

	// Make sure link worked too
	glGetProgramiv(hReturn, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE)
	{
		GLsizei maxLen = 4096;
		GLchar infoLog[4096];
		GLsizei actualLen;

		glGetProgramInfoLog(hReturn,
							maxLen,
							&actualLen,
							infoLog);

		printf("Warning/Error in GLSL shader:\n");
		printf("%s\n", infoLog);
		glDeleteProgram(hReturn);
		return (GLuint)0;
	}

	return hReturn;
}

 
 



void checkError(const char* functionName)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        fprintf(stderr, "GL error 0x%X detected in %s\n", error, functionName);
    }
}

void ProgramBase::create(const char* vs, const char* fs)
{
    VertexShader = vs;
    FragmentShader = fs;
    Shader = gltLoadShaderPair(vs, fs);
    ModelViewMatrix = getLoc("ModelViewMatrix");
    ProjectionMatrix = getLoc("ProjectionMatrix");

}

int ProgramBase::getLoc(const char* str)
{
    return glGetUniformLocation(Shader, str);
}


int ProgramBase::getAttribLoc(const char* str)
{
    return glGetAttribLocation(Shader, str);
}

void ProgramBase::link()
{
    glLinkProgram(Shader);
}

void ProgramBase::use()
{
    glUseProgram(Shader);
}


void ProgramBase::unuse()
{
    glUseProgram(0);
}

void ProgramBase::setUniform(int un, float val)
{
    glUniform1f(un, val);
}

void ProgramBase::setUniform(int un, float v1, float v2)
{
    glUniform2f(un, v1, v2);
}


void ProgramBase::setUniform(int un, float v1, float v2, float v3)
{
    glUniform3f(un, v1, v2, v3);
}

void ProgramBase::setUniform(int un, float v1, float v2, float v3, float v4)
{
    glUniform4f(un, v1, v2, v3, v4);
}

void ProgramBase::setUniform(int un, const float*vn, unsigned n)
{ 
    glUniform1fv(un, n, vn); 
}


void ProgramBase::setUniform(int un, int val)
{
    glUniform1i(un, val);
}

void ProgramBase::setUniform(int un, int v1, int v2)
{
    glUniform2i(un, v1, v2);
}


void ProgramBase::setUniform(int un, int v1, int v2, int v3)
{
    glUniform3i(un, v1, v2, v3);
}

void ProgramBase::setUniform(int un, int v1, int v2, int v3, int v4)
{
    glUniform4i(un, v1, v2, v3, v4);
}

void ProgramBase::setUniform(int un, const int*vn, unsigned n)
{
    glUniform1iv(un, n, vn);
}


void ProgramBase::setUniform(int un, const Vec2f& vn)
{
    glUniform2fv(un, 1, vn.u);
}


void ProgramBase::setUniform(int un, const Vec3f& vn)
{
    glUniform3fv(un, 1, vn.u);
}


void ProgramBase::setUniform(int un, const Vec4f& vn)
{
    glUniform4fv(un, 1, vn.u);
}


void ProgramBase::setUniform(int un, const Vec2i& vn)
{
    glUniform2iv(un, 1, vn.u);
}


void ProgramBase::setUniform(int un, const Vec3i& vn)
{
    glUniform3iv(un, 1, vn.u);
}


void ProgramBase::setUniform(int un, const Vec4i& vn)
{
    glUniform4iv(un, 1, vn.u);
}

void ProgramBase::setUniformMatrix4(int un, const Matrixf& mat)
{
    glUniformMatrix4fv(un, 1, false, mat.ptr());
}


void ProgramBase::setUniformMatrix4(int un, const float* mat)
{
    glUniformMatrix4fv(un, 1, false, mat);
}

void ProgramBase::setVertexAttrib(int attr, int len, int stride, int div, const void * size)
{
    glEnableVertexAttribArray(attr);
    glVertexAttribPointer(attr, len, GL_FLOAT, GL_FALSE, stride, size);
    checkError("glVertexAttribPointer");
    if (div >= 0)
    {
        glVertexAttribDivisor(attr, div);
        checkError("glVertexAttribDivisor");
    }

}

void BufferObject::create(int sz)
{
    glGenVertexArrays(1, &VertexArrayObject);
    glBindVertexArray(VertexArrayObject);

    glGenBuffers(1, &VertexBufferObject);
    glGenBuffers(1, &IndexVbo);

    if (sz)
    {//??
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sz, 0, GL_DYNAMIC_DRAW);
    }

    glBindVertexArray(0);
}


void BufferObject::clear()
{
    glDeleteVertexArrays(1, &VertexArrayObject);
    glDeleteBuffers(1, &VertexBufferObject);
    glDeleteBuffers(1, &IndexVbo);
}

void BufferObject::toData(int numvertices, const void* vertices)
{

    glBufferData(GL_ARRAY_BUFFER, numvertices, nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numvertices, vertices);
}

void BufferObject::setAttrib()
{

}

void BufferObject::toElement(int numIndices, const void* indices, bool bdynamic)
{

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(int), nullptr, bdynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numIndices * sizeof(int), indices);
}


void BufferObject::Bind()
{
    glBindVertexArray(VertexArrayObject);
    checkError("glBindVertexArray");
    
    if (VertexBufferObject)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
        checkError("glBindBuffer");
    }
    if (IndexVbo)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVbo);
    }
}

void BufferObject::unBind()
{
    glBindVertexArray(0);
   
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //b3Assert(glGetError() == GL_NO_ERROR);

}
 
void TextureObject::setBorder(float* clr)
{
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}
void TextureObject::setParam(TexParamE type)
{
    if (type == TEX_LINEAR_MIPMAP)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (type == TEX_LINEAR)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else 
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

}

void TextureObject::toData2(TexColorE clr, TexBitE bit)
{
    if (clr == TEX_RGB)
    {
        if (bit == TEX_8)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
    }
    else if (clr == TEX_RGBA)
    {
        if (bit == TEX_8)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
    }
    else if (clr == TEX_GRAY)
    {
        if (bit == TEX_8)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        }
    }
    else if (clr == TEX_DEPTH)
    {
        if (bit == TEX_16F)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
        }
        else if (bit == TEX_32F)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
        }
    }
}
