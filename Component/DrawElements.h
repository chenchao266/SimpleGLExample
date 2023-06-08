/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#ifndef OSG_PRIMITIVESET
#define OSG_PRIMITIVESET 1
 
#include <Bullet3Common/MixinVector.h>
 
#include <vector>
#include <memory>
#include "OpenGLWindow/OpenGLInclude.h"
  
typedef MixinVector<GLushort> VectorGLushort;
typedef MixinVector<GLuint> VectorGLuint;
 
 
 
class State;
class DrawElements //: public BufferData
{
    public:

        enum Type
        {
            PrimitiveType,
            DrawArraysPrimitiveType,
            DrawArrayLengthsPrimitiveType,
            DrawElementsUBytePrimitiveType,
            DrawElementsUShortPrimitiveType,
            DrawElementsUIntPrimitiveType,
            MultiDrawArraysPrimitiveType,
            DrawArraysIndirectPrimitiveType,
            DrawElementsUByteIndirectPrimitiveType,
            DrawElementsUShortIndirectPrimitiveType,
            DrawElementsUIntIndirectPrimitiveType,
            MultiDrawArraysIndirectPrimitiveType,
            MultiDrawElementsUByteIndirectPrimitiveType,
            MultiDrawElementsUShortIndirectPrimitiveType,
            MultiDrawElementsUIntIndirectPrimitiveType
        };

        enum Mode
        {
            POINTS = GL_POINTS,
            LINES = GL_LINES,
            LINE_STRIP = GL_LINE_STRIP,
            LINE_LOOP = GL_LINE_LOOP,
            TRIANGLES = GL_TRIANGLES,
            TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
            TRIANGLE_FAN = GL_TRIANGLE_FAN,
            QUADS = GL_QUADS,
            QUAD_STRIP = GL_QUAD_STRIP,
            POLYGON = GL_POLYGON,
            LINES_ADJACENCY = GL_LINES_ADJACENCY,
            LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
            TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
            TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
            PATCHES = GL_PATCHES
        };

        DrawElements(Type primType=PrimitiveType, int mode=0, int numInstances=0):
            _primitiveType(primType),
            _numInstances(numInstances),
            _mode(mode) {}

        DrawElements(const DrawElements& prim):
            _primitiveType(prim._primitiveType),
            _numInstances(prim._numInstances),
            _mode(prim._mode) {}
  
        Type                    getType() const { return _primitiveType; }
         
        virtual const void*   getDataPointer() const { return 0; }
        virtual unsigned int    getTotalDataSize() const { return 0; }
        virtual bool            supportsBufferObject() const { return false; }
   
        void setNumInstances(int n) { _numInstances = n; }
        int getNumInstances() const { return _numInstances; }

        void setMode(int mode) { _mode = mode; }
        int getMode() const { return _mode; }

        virtual void draw(State& state, bool useVertexBufferObjects) const = 0;
          
        virtual unsigned int index(unsigned int pos) const = 0;
        virtual unsigned int getNumIndices() const = 0;
        virtual void offsetIndices(int offset) = 0;

        virtual unsigned int getNumPrimitives() const;

        virtual void computeRange() const {}
        virtual int getDataType() = 0;
        virtual void resizeElements(unsigned int numIndices) = 0;
        virtual void reserveElements(unsigned int numIndices) = 0;
        virtual void setElement(unsigned int, unsigned int) = 0;
        virtual unsigned int getElement(unsigned int) = 0;
        virtual void addElement(unsigned int) = 0;

       virtual ~DrawElements() {}
protected:

        Type            _primitiveType;
        int             _numInstances;
        int          _mode;
};

class   DrawElementsUShort : public DrawElements, public VectorGLushort
{
    public:

        typedef VectorGLushort vector_type;

        DrawElementsUShort(int mode=0):
            DrawElements(DrawElementsUShortPrimitiveType,mode) {}

        DrawElementsUShort(const DrawElementsUShort& array):
            DrawElements(array),
            vector_type(array) {}

        /**
         * \param mode One of DrawElements::Mode. Determines the type of primitives used.
         * \param no Number of intended elements. This will be the size of the underlying vector.
         * \param ptr Pointer to a GLushort to copy index data from.
         * \param numInstances When non zero passed as the number of draw instances to use re.
         */
        DrawElementsUShort(int mode, unsigned int no, const unsigned short* ptr, int numInstances=0) :
            DrawElements(DrawElementsUShortPrimitiveType,mode,numInstances),
            vector_type(ptr,ptr+no) {}

        /**
         * \param mode One of DrawElements::Mode. Determines the type of primitives used.
         * \param no Number of intended elements. This will be the size of the underlying vector.
         */
        DrawElementsUShort(int mode, unsigned int no) :
            DrawElements(DrawElementsUShortPrimitiveType,mode),
            vector_type(no) {}

        template <class InputIterator>
        DrawElementsUShort(int mode, InputIterator first,InputIterator last) :
            DrawElements(DrawElementsUShortPrimitiveType,mode),
            vector_type(first,last) {}
         

        virtual const void*   getDataPointer() const { return empty()?0:&front(); }
        virtual unsigned int    getTotalDataSize() const { return 2u*static_cast<unsigned int>(size()); }
        virtual bool            supportsBufferObject() const { return false; }

        virtual void draw(State& state, bool useVertexBufferObjects) const {};
         
        virtual unsigned int getNumIndices() const { return static_cast<unsigned int>(size()); }
        virtual unsigned int index(unsigned int pos) const { return (*this)[pos]; }
        virtual void offsetIndices(int offset) {};

        virtual int getDataType() { return GL_UNSIGNED_SHORT; }
        virtual void resizeElements(unsigned int numIndices) { resize(numIndices); }
        virtual void reserveElements(unsigned int numIndices) { reserve(numIndices); }
        virtual void setElement(unsigned int i, unsigned int v) { (*this)[i] = v; }
        virtual unsigned int getElement(unsigned int i) { return (*this)[i]; }
        virtual void addElement(unsigned int v) { push_back(GLushort(v)); }
 
        virtual ~DrawElementsUShort() {};
};

class   DrawElementsUInt : public DrawElements, public VectorGLuint
{
    public:

        typedef VectorGLuint vector_type;

        DrawElementsUInt(int mode=0):
            DrawElements(DrawElementsUIntPrimitiveType,mode) {}

        DrawElementsUInt(const DrawElementsUInt& array):
            DrawElements(array),
            vector_type(array) {}

        /**
         * \param mode One of DrawElements::Mode. Determines the type of primitives used.
         * \param no Number of intended elements. This will be the size of the underlying vector.
         * \param ptr Pointer to a GLuint to copy index data from.
         * \param numInstances When non zero passed as the number of draw instances to use re.
         */
        DrawElementsUInt(int mode, unsigned int no, const GLuint* ptr, int numInstances=0) :
            DrawElements(DrawElementsUIntPrimitiveType,mode,numInstances),
            vector_type(ptr,ptr+no) {}

        /**
         * \param mode One of DrawElements::Mode. Determines the type of primitives used.
         * \param no Number of intended elements. This will be the size of the underlying vector.
         */
        DrawElementsUInt(int mode, unsigned int no) :
            DrawElements(DrawElementsUIntPrimitiveType,mode),
            vector_type(no) {}

        template <class InputIterator>
        DrawElementsUInt(int mode, InputIterator first,InputIterator last) :
            DrawElements(DrawElementsUIntPrimitiveType,mode),
            vector_type(first,last) {}
         

        virtual const void*   getDataPointer() const { return empty()?0:&front(); }
        virtual unsigned int    getTotalDataSize() const { return 4u*static_cast<unsigned int>(size()); }
        virtual bool            supportsBufferObject() const { return false; }

        virtual void draw(State& state, bool useVertexBufferObjects) const {};
         
        virtual unsigned int getNumIndices() const { return static_cast<unsigned int>(size()); }
        virtual unsigned int index(unsigned int pos) const { return (*this)[pos]; }
        virtual void offsetIndices(int offset) {};

        virtual int getDataType() { return GL_UNSIGNED_INT; }
        virtual void resizeElements(unsigned int numIndices) { resize(numIndices); }
        virtual void reserveElements(unsigned int numIndices) { reserve(numIndices); }
        virtual void setElement(unsigned int i, unsigned int v) { (*this)[i] = v; }
        virtual unsigned int getElement(unsigned int i) { return (*this)[i]; }
        virtual void addElement(unsigned int v) { push_back(GLuint(v)); }
         
        virtual ~DrawElementsUInt() {};
};
 

#endif
