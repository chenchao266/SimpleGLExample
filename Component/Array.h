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

#ifndef OSG_ARRAY
#define OSG_ARRAY 1

#include <Bullet3Common/MixinVector.h>
#include <Bullet3Common/geometry.h>
#include <memory>
#include "OpenGLWindow/OpenGLInclude.h"
class   Array //: public BufferData
{

    public:

        /// The type of data stored in this array.
        enum Type
        {
            ArrayType = 0,

            ByteArrayType     = 1,
            ShortArrayType    = 2,
            IntArrayType      = 3,

            UByteArrayType    = 4,
            UShortArrayType   = 5,
            UIntArrayType     = 6,

            FloatArrayType    = 7,
            DoubleArrayType   = 8,

            Vec2bArrayType    = 9,
            Vec3bArrayType    = 10,
            Vec4bArrayType    = 11,

            Vec2sArrayType    = 12,
            Vec3sArrayType    = 13,
            Vec4sArrayType    = 14,

            Vec2iArrayType    = 15,
            Vec3iArrayType    = 16,
            Vec4iArrayType    = 17,

            Vec2ubArrayType   = 18,
            Vec3ubArrayType   = 19,
            Vec4ubArrayType   = 20,

            Vec2usArrayType   = 21,
            Vec3usArrayType   = 22,
            Vec4usArrayType   = 23,

            Vec2uiArrayType   = 24,
            Vec3uiArrayType   = 25,
            Vec4uiArrayType   = 26,

            Vec2ArrayType     = 27,
            Vec3ArrayType     = 28,
            Vec4ArrayType     = 29,

            Vec2dArrayType    = 30,
            Vec3dArrayType    = 31,
            Vec4dArrayType    = 32,

            MatrixArrayType   = 33,
            MatrixdArrayType  = 34,

            QuatArrayType     = 35,

            UInt64ArrayType   = 36,
            Int64ArrayType    = 37,

            LastArrayType     = 37
                // If new array types are added, update this and
                // update Array::className() in src/osg/Array.cpp.
                // Array::Type values are from ArrayType to
                // LastArrayType, inclusive.
        };

        /// The scope of applicability of the values in this array
        enum Binding
        {
            BIND_UNDEFINED=-1,
            BIND_OFF=0,
            BIND_OVERALL=1,
            BIND_PER_PRIMITIVE_SET=2,
            BIND_PER_VERTEX=4
        };

        Array(Type arrayType=ArrayType,int dataSize=0, int dataType=0, Binding binding=BIND_UNDEFINED):
            _arrayType(arrayType),
            _dataSize(dataSize),
            _dataType(dataType),
            _binding(binding),
            _normalize(false),
            _preserveDataType(false) {}

       Array(const Array& array):
 
            _arrayType(array._arrayType),
            _dataSize(array._dataSize),
            _dataType(array._dataType),
            _binding(array._binding),
            _normalize(array._normalize),
            _preserveDataType(array._preserveDataType) {}
        
        /** Return -1 if lhs element is less than rhs element, 0 if equal,
          * 1 if lhs element is greater than rhs element. */
        virtual int compare(unsigned int lhs,unsigned int rhs) const = 0;

        Type getType() const { return _arrayType; }
        int getDataSize() const { return _dataSize; }
        int getDataType() const { return _dataType; }
         
        virtual unsigned int getElementSize() const = 0;
        virtual const void* getDataPointer() const = 0;
        virtual const void* getDataPointer(unsigned int index) const = 0;
        virtual unsigned int getTotalDataSize() const = 0;
        virtual unsigned int getNumElements() const = 0;
        virtual void reserveArray(unsigned int num) = 0;
        virtual void resizeArray(unsigned int num) = 0;


        /** Specify how this array should be passed to OpenGL.*/
        void setBinding(Binding binding) { _binding = binding; }

        /** Get how this array should be passed to OpenGL.*/
        Binding getBinding() const { return _binding; }


        /** Specify whether the array data should be normalized by OpenGL.*/
        void setNormalize(bool normalize) { _normalize = normalize; }

        /** Get whether the array data should be normalized by OpenGL.*/
        bool getNormalize() const { return _normalize; }


        /** Set hint to ask that the array data is passed via integer or double, or normal setVertexAttribPointer function.*/
        void setPreserveDataType(bool preserve) { _preserveDataType = preserve; }

        /** Get hint to ask that the array data is passed via integer or double, or normal setVertexAttribPointer function.*/
        bool getPreserveDataType() const { return _preserveDataType; }


        /** Frees unused space on this vector - i.e. the difference between size() and max_size() of the underlying vector.*/
        virtual void trim() {}

        virtual ~Array() {}

    protected:

        Type    _arrayType;
        int   _dataSize;
        int  _dataType;
        Binding _binding;
        bool    _normalize;
        bool    _preserveDataType;
};

/** convenience function for getting the binding of array via a ptr that may be null.*/
inline Array::Binding getBinding(const Array* array) { return array ? array->getBinding() : Array::BIND_OFF; }

/** convenience function for getting the binding of array via a ptr that may be null.*/
inline bool getNormalize(const Array* array) { return array ? array->getNormalize() : false; }


/// A concrete array holding elements of type T.
template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
class TemplateArray : public Array, public MixinVector<T>
{
    public:

        TemplateArray(Binding binding=BIND_UNDEFINED) : Array(ARRAYTYPE,DataSize,DataType, binding) {}

        TemplateArray(const TemplateArray& ta):
            Array(ta),
            MixinVector<T>(ta) {}

        TemplateArray(unsigned int no) :
            Array(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(no) {}

        TemplateArray(unsigned int no,const T* ptr) :
            Array(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(ptr,ptr+no) {}

        TemplateArray(Binding binding, unsigned int no) :
            Array(ARRAYTYPE,DataSize,DataType, binding),
            MixinVector<T>(no) {}

        TemplateArray(Binding binding, unsigned int no,const T* ptr) :
            Array(ARRAYTYPE,DataSize,DataType, binding),
            MixinVector<T>(ptr,ptr+no) {}

        template <class InputIterator>
        TemplateArray(InputIterator first,InputIterator last) :
            Array(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(first,last) {}

        TemplateArray& operator = (const TemplateArray& array)
        {
            if (this==&array) return *this;
            this->assign(array.begin(),array.end());
            return *this;
        } 
        virtual int compare(unsigned int lhs,unsigned int rhs) const
        {
            const T& elem_lhs = (*this)[lhs];
            const T& elem_rhs = (*this)[rhs];
            if (elem_lhs<elem_rhs) return -1;
            if (elem_rhs<elem_lhs) return 1;
            return 0;
        }

        /** Frees unused space on this vector - i.e. the difference between size() and max_size() of the underlying vector.*/
        virtual void trim()
        {
            MixinVector<T>( *this ).swap( *this );
        }

        virtual unsigned int    getElementSize() const { return sizeof(ElementDataType); }
        virtual const void*   getDataPointer() const { if (!this->empty()) return &this->front(); else return 0; }
        virtual const void*   getDataPointer(unsigned int index) const { if (!this->empty()) return &((*this)[index]); else return 0; }
        virtual unsigned int    getTotalDataSize() const { return static_cast<unsigned int>(this->size()*sizeof(ElementDataType)); }
        virtual unsigned int    getNumElements() const { return static_cast<unsigned int>(this->size()); }
        virtual void reserveArray(unsigned int num) { this->reserve(num); }
        virtual void resizeArray(unsigned int num) { this->resize(num); }

        typedef T ElementDataType; // expose T
         
        virtual ~TemplateArray() {}
};

class   IndexArray : public Array
{

    public:

        IndexArray(Type arrayType=ArrayType,int dataSize=0, int dataType=0):
            Array(arrayType,dataSize,dataType) {}

        IndexArray(const Array& array):
            Array(array) {}
         
        virtual unsigned int index(unsigned int pos) const = 0;
         
        virtual ~IndexArray() {}
};

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
class TemplateIndexArray : public IndexArray, public MixinVector<T>
{
    public:

        TemplateIndexArray() : IndexArray(ARRAYTYPE,DataSize,DataType) {}

        TemplateIndexArray(const TemplateIndexArray& ta):
            IndexArray(ta),
            MixinVector<T>(ta) {}

        TemplateIndexArray(unsigned int no) :
            IndexArray(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(no) {}

        TemplateIndexArray(unsigned int no,T* ptr) :
            IndexArray(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(ptr,ptr+no) {}

        template <class InputIterator>
        TemplateIndexArray(InputIterator first,InputIterator last) :
            IndexArray(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(first,last) {}

        TemplateIndexArray& operator = (const TemplateIndexArray& array)
        {
            if (this==&array) return *this;
            this->assign(array.begin(),array.end());
            return *this;
        }
 
        virtual int compare(unsigned int lhs,unsigned int rhs) const
        {
            const T& elem_lhs = (*this)[lhs];
            const T& elem_rhs = (*this)[rhs];
            if (elem_lhs<elem_rhs) return -1;
            if (elem_rhs<elem_lhs) return 1;
            return 0;
        }

        /** Frees unused space on this vector - i.e. the difference between size() and max_size() of the underlying vector.*/
        virtual void trim()
        {
            MixinVector<T>( *this ).swap( *this );
        }

        virtual unsigned int getElementSize() const { return sizeof(ElementDataType); }
        virtual const void*   getDataPointer() const { if (!this->empty()) return &this->front(); else return 0; }
        virtual const void*   getDataPointer(unsigned int index) const { if (!this->empty()) return &((*this)[index]); else return 0; }
        virtual unsigned int    getTotalDataSize() const { return static_cast<unsigned int>(this->size()*sizeof(T)); }
        virtual unsigned int    getNumElements() const { return static_cast<unsigned int>(this->size()); }
        virtual void reserveArray(unsigned int num) { this->reserve(num); }
        virtual void resizeArray(unsigned int num) { this->resize(num); }

        virtual unsigned int    index(unsigned int pos) const { return (*this)[pos]; }

        typedef T ElementDataType; // expose T
         
        virtual ~TemplateIndexArray() {}
};

// The predefined array types

typedef TemplateIndexArray<GLbyte,Array::ByteArrayType,1,GL_BYTE>               ByteArray;
typedef TemplateIndexArray<GLshort,Array::ShortArrayType,1,GL_SHORT>            ShortArray;
typedef TemplateIndexArray<GLint,Array::IntArrayType,1,GL_INT>                  IntArray;

typedef TemplateIndexArray<GLubyte,Array::UByteArrayType,1,GL_UNSIGNED_BYTE>    UByteArray;
typedef TemplateIndexArray<GLushort,Array::UShortArrayType,1,GL_UNSIGNED_SHORT> UShortArray;
typedef TemplateIndexArray<GLuint,Array::UIntArrayType,1,GL_UNSIGNED_INT>       UIntArray;

typedef TemplateArray<float,Array::FloatArrayType,1,GL_FLOAT>                 FloatArray;
typedef TemplateArray<double,Array::DoubleArrayType,1,GL_DOUBLE>              DoubleArray;
  
typedef TemplateArray<Vec2f,Array::Vec2ArrayType,2,GL_FLOAT>                     Vec2Array;
typedef TemplateArray<Vec3f,Array::Vec3ArrayType,3,GL_FLOAT>                     Vec3Array;
typedef TemplateArray<Vec4f,Array::Vec4ArrayType,4,GL_FLOAT>                     Vec4Array;
 

#endif
