﻿/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
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


#include <algorithm>
#include <string.h>
#include <stdlib.h>

#include "dxtctool.h"
#include "Image.h"
#include "Texture2D.h"
#include "Bullet3Common/b3MinMax.h"
#include "GeometryShape.h"
#include "TextureCubeMap.h"



 
Image::DataIterator::DataIterator(const Image* image):
    _image(image),
    _rowNum(0),
    _imageNum(0),
    _mipmapNum(0),
    _currentPtr(0),
    _currentSize(0)
{
    assign();
}

Image::DataIterator::DataIterator(const DataIterator& ri):
    _image(ri._image),
    _rowNum(ri._rowNum),
    _imageNum(ri._imageNum),
    _mipmapNum(ri._mipmapNum),
    _currentPtr(0),
    _currentSize(0)
{
    assign();
}

void Image::DataIterator::operator ++ ()
{
    if (!_image || _image->isDataContiguous())
    {
        // for contiguous image data we never need more than one block of data
        _currentPtr = 0;
        _currentSize = 0;
        return;
    }

    if (_image->isMipmap())
    {
        // advance to next row
        ++_rowNum;

        if (_rowNum>=_image->t())
        {
            // moved over end of current image so move to next
            _rowNum = 0;
            ++_imageNum;

            if (_imageNum>=_image->r())
            {
                // move to next mipmap
                _imageNum = 0;
                ++_mipmapNum;

                if (_mipmapNum>=_image->getNumMipmapLevels())
                {
                    _currentPtr = 0;
                    _currentSize = 0;
                    return;
                }
            }
        }
    }
    else
    {
        // advance to next row
        ++_rowNum;

        if (_rowNum>=_image->t())
        {
            // moved over end of current image so move to next
            _rowNum = 0;
            ++_imageNum;

            if (_imageNum>=_image->r())
            {
                // we've moved off the end of the Image so reset to null
                _currentPtr = 0;
                _currentSize = 0;
                return;
            }
        }
    }

    assign();
}

void Image::DataIterator::assign()
{
    ////OSG_NOTICE<<"DataIterator::assign A"<<std::endl;
    if (!_image)
    {
        _currentPtr = 0;
        _currentSize = 0;
        return;
    }

    ////OSG_NOTICE<<"DataIterator::assign B"<<std::endl;

    if (_image->isDataContiguous())
    {
        _currentPtr = _image->data();
        _currentSize = _image->getTotalSizeInBytesIncludingMipmaps();

        ////OSG_NOTICE<<"   _currentPtr="<<(void*)_currentPtr<<std::endl;
        ////OSG_NOTICE<<"   _currentSize="<<_currentSize<<std::endl;

        return;
    }

    ////OSG_NOTICE<<"DataIterator::assign C"<<std::endl;

    if (_image->isMipmap())
    {
        ////OSG_NOTICE<<"DataIterator::assign D"<<std::endl;

        if (_mipmapNum>=_image->getNumMipmapLevels())
        {
            _currentPtr = 0;
            _currentSize = 0;
            return;
        }
        const unsigned char* ptr = _image->getMipmapData(_mipmapNum);

        int rowLength = _image->getRowLength()>>_mipmapNum;
        if (rowLength==0) rowLength = 1;

        int imageHeight = _image->t()>>_mipmapNum;
        if (imageHeight==0) imageHeight = 1;

        unsigned int rowWidthInBytes = Image::computeRowWidthInBytes(rowLength,_image->getPixelFormat(),_image->getDataType(),_image->getPacking());
        unsigned int imageSizeInBytes = rowWidthInBytes*imageHeight;

        _currentPtr = ptr + rowWidthInBytes*_rowNum + imageSizeInBytes*_imageNum;
        _currentSize = rowWidthInBytes;
    }
    else
    {
        ////OSG_NOTICE<<"DataIterator::assign E"<<std::endl;

        if (_imageNum>=_image->r() || _rowNum>=_image->t())
        {
            _currentPtr = 0;
            _currentSize = 0;
            return;
        }

        ////OSG_NOTICE<<"DataIterator::assign F"<<std::endl;

        _currentPtr = _image->data(0, _rowNum, _imageNum);
        _currentSize = _image->getRowSizeInBytes();
        return;
    }
}


Image::Image():
   
    _fileName(""),
    _writeHint(NO_PREFERENCE),
    _origin(BOTTOM_LEFT),
    _s(0), _t(0), _r(0),
    _rowLength(0),
    _internalTextureFormat(0),
    _pixelFormat(0),
    _dataType(0),
    _packing(4),
    _pixelAspectRatio(1.0),
    _allocationMode(USE_NEW_DELETE),
    _data(0L)
    
{
    //setDataVariance(GL_STATIC_DRAW);
}

Image::Image(const Image& image ):
     
    _fileName(image._fileName),
    _writeHint(image._writeHint),
    _origin(image._origin),
    _s(image._s), _t(image._t), _r(image._r),
    _rowLength(0),
    _internalTextureFormat(image._internalTextureFormat),
    _pixelFormat(image._pixelFormat),
    _dataType(image._dataType),
    _packing(image._packing),
    _pixelAspectRatio(image._pixelAspectRatio),
    _allocationMode(USE_NEW_DELETE),
    _data(0L),
    _mipmapData(image._mipmapData)
 
{
    if (image._data)
    {
        unsigned int size = image.getTotalSizeInBytesIncludingMipmaps();
        setData(new unsigned char [size],USE_NEW_DELETE);
        if (unsigned char* dest_ptr = _data)
        {
            for(DataIterator itr(&image); itr.valid(); ++itr)
            {
                memcpy(dest_ptr, itr.data(), itr.size());
                dest_ptr += itr.size();
            }
        }
        else
        {
            ////OSG_WARN<<"Warning: Image::Image(const Image&, const CopyOp&) out of memory, no image copy made."<<std::endl;
        }
    }
}

Image::~Image()
{
    deallocateData();
}

void Image::deallocateData()
{
    if (_data) {
        if (_allocationMode==USE_NEW_DELETE) delete [] _data;
        else if (_allocationMode==USE_MALLOC_FREE) ::free(_data);
        _data = 0;
    }
}
 
void Image::setFileName(const std::string& fileName)
{
    _fileName = fileName;
}

void Image::setData(unsigned char* data, AllocationMode mode)
{
    deallocateData();
    _data = data;
    _allocationMode = mode;
    dirty();
}


bool Image::isPackedType(GLenum type)
{
    switch(type)
    {
        case(GL_UNSIGNED_BYTE_3_3_2):
        case(GL_UNSIGNED_BYTE_2_3_3_REV):
        case(GL_UNSIGNED_SHORT_5_6_5):
        case(GL_UNSIGNED_SHORT_5_6_5_REV):
        case(GL_UNSIGNED_SHORT_4_4_4_4):
        case(GL_UNSIGNED_SHORT_4_4_4_4_REV):
        case(GL_UNSIGNED_SHORT_5_5_5_1):
        case(GL_UNSIGNED_SHORT_1_5_5_5_REV):
        case(GL_UNSIGNED_INT_8_8_8_8):
        case(GL_UNSIGNED_INT_8_8_8_8_REV):
        case(GL_UNSIGNED_INT_10_10_10_2):
        case(GL_UNSIGNED_INT_2_10_10_10_REV): return true;
        default: return false;
    }
}


GLenum Image::computePixelFormat(GLenum format)
{
    switch(format)
    {
        case(GL_ALPHA16F):
        case(GL_ALPHA32F):
            return GL_ALPHA;

        case(GL_LUMINANCE16F):
        case(GL_LUMINANCE32F):
            return GL_LUMINANCE;

        case(GL_INTENSITY16F):
        case(GL_INTENSITY32F):
            return GL_INTENSITY;

        case(GL_LUMINANCE_ALPHA16F):
        case(GL_LUMINANCE_ALPHA32F):
            return GL_LUMINANCE_ALPHA;

        case (GL_R16F):
        case (GL_R32F):
        case (GL_R8):
        case (GL_R8_SNORM):
        case (GL_R16):
        case (GL_R16_SNORM):
            return GL_RED;

        case (GL_R8I):
        case (GL_R8UI):
        case (GL_R16I):
        case (GL_R16UI):
        case (GL_R32I):
        case (GL_R32UI):
            return GL_RED_INTEGER;

        case (GL_RG16F):
        case (GL_RG32F):
        case (GL_RG8):
        case (GL_RG8_SNORM):
        case (GL_RG16):
        case (GL_RG16_SNORM):
            return GL_RG;

        case (GL_RG8I):
        case (GL_RG8UI):
        case (GL_RG16I):
        case (GL_RG16UI):
        case (GL_RG32I):
        case (GL_RG32UI):
            return GL_RG_INTEGER;

        case(GL_RGB32F):
        case(GL_RGB16F):
        case(GL_R3_G3_B2):
        case(GL_RGB4):
        case(GL_RGB5):
        case(GL_RGB8):
        case(GL_RGB8_SNORM):
        case(GL_RGB10):
        case(GL_RGB12):
        case(GL_SRGB8):
            return GL_RGB;

        case(GL_RGBA8):
        case(GL_RGBA16):
        case(GL_RGBA32F):
        case(GL_RGBA16F):
        case(GL_RGBA8_SNORM):
        case(GL_RGB10_A2):
        case(GL_RGBA12):
        case(GL_SRGB8_ALPHA8):
            return GL_RGBA;

        case(GL_ALPHA8I):
        case(GL_ALPHA16I):
        case(GL_ALPHA32I):
        case(GL_ALPHA8UI):
        case(GL_ALPHA16UI):
        case(GL_ALPHA32UI):
            return GL_ALPHA_INTEGER;

        case(GL_LUMINANCE8I):
        case(GL_LUMINANCE16I):
        case(GL_LUMINANCE32I):
        case(GL_LUMINANCE8UI):
        case(GL_LUMINANCE16UI):
        case(GL_LUMINANCE32UI):
            return GL_LUMINANCE_INTEGER;

        case(GL_INTENSITY8I):
        case(GL_INTENSITY16I):
        case(GL_INTENSITY32I):
        case(GL_INTENSITY8UI):
        case(GL_INTENSITY16UI):
        case(GL_INTENSITY32UI):
            ////OSG_WARN<<"Image::computePixelFormat("<<std::hex<<format<<std::dec<<") intensity pixel format is not correctly specified, so assume GL_LUMINANCE_INTEGER."<<std::endl;
            return GL_LUMINANCE_INTEGER;

        case(GL_LUMINANCE_ALPHA8I):
        case(GL_LUMINANCE_ALPHA16I):
        case(GL_LUMINANCE_ALPHA32I):
        case(GL_LUMINANCE_ALPHA8UI):
        case(GL_LUMINANCE_ALPHA16UI):
        case(GL_LUMINANCE_ALPHA32UI):
            return GL_LUMINANCE_ALPHA_INTEGER;

        case(GL_RGB32I):
        case(GL_RGB16I):
        case(GL_RGB8I):
        case(GL_RGB32UI):
        case(GL_RGB16UI):
        case(GL_RGB8UI):
            return GL_RGB_INTEGER;

        case(GL_RGBA32I):
        case(GL_RGBA16I):
        case(GL_RGBA8I):
        case(GL_RGBA32UI):
        case(GL_RGBA16UI):
        case(GL_RGBA8UI):
            return GL_RGBA_INTEGER;

        case(GL_DEPTH_COMPONENT16):
        case(GL_DEPTH_COMPONENT24):
        case(GL_DEPTH_COMPONENT32):
        case(GL_DEPTH_COMPONENT32F):
        case(GL_DEPTH_COMPONENT32F_NV):
            return GL_DEPTH_COMPONENT;

        default:
            return format;
    }
}

GLenum Image::computeFormatDataType(GLenum pixelFormat)
{
    switch (pixelFormat)
    {
        case GL_R32F:
        case GL_RG32F:
        case GL_LUMINANCE32F:
        case GL_LUMINANCE16F:
        case GL_LUMINANCE_ALPHA32F:
        case GL_LUMINANCE_ALPHA16F:
        case GL_RGB32F:
        case GL_RGB16F:
        case GL_RGBA32F:
        case GL_RGBA16F: return GL_FLOAT;

        case GL_RGBA32UI:
        case GL_RGB32UI:
        case GL_LUMINANCE32UI:
        case GL_LUMINANCE_ALPHA32UI: return GL_UNSIGNED_INT;

        case GL_RGB16UI:
        case GL_RGBA16UI:
        case GL_LUMINANCE16UI:
        case GL_LUMINANCE_ALPHA16UI: return GL_UNSIGNED_SHORT;

        case GL_RGBA8UI:
        case GL_RGB8UI:
        case GL_LUMINANCE8UI:
        case GL_LUMINANCE_ALPHA8UI:  return GL_UNSIGNED_BYTE;

        case GL_RGBA32I:
        case GL_RGB32I:
        case GL_LUMINANCE32I:
        case GL_LUMINANCE_ALPHA32I: return GL_INT;

        case GL_RGBA16I:
        case GL_RGB16I:
        case GL_LUMINANCE16I:
        case GL_LUMINANCE_ALPHA16I: return GL_SHORT;

        case GL_RGB8I:
        case GL_RGBA8I:
        case GL_LUMINANCE8I:
        case GL_LUMINANCE_ALPHA8I: return GL_BYTE;

        case GL_RGBA:
        case GL_RGB:
        case GL_RED:
        case GL_RG:
        case GL_LUMINANCE:
        case GL_LUMINANCE_ALPHA:
        case GL_ALPHA: return GL_UNSIGNED_BYTE;

        default:
        {
            ////OSG_WARN<<"error computeFormatType = "<<std::hex<<pixelFormat<<std::dec<<std::endl;
            return 0;
        }
    }
}

unsigned int Image::computeNumComponents(GLenum pixelFormat)
{
    switch(pixelFormat)
    {
        case(GL_COMPRESSED_RGB_S3TC_DXT1): return 3;
        case(GL_COMPRESSED_RGBA_S3TC_DXT1): return 4;
        case(GL_COMPRESSED_RGBA_S3TC_DXT3): return 4;
        case(GL_COMPRESSED_RGBA_S3TC_DXT5): return 4;
        case(GL_COMPRESSED_SIGNED_RED_RGTC1): return 1;
        case(GL_COMPRESSED_RED_RGTC1):   return 1;
        case(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2): return 2;
        case(GL_COMPRESSED_RED_GREEN_RGTC2): return 2;
        case(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG): return 3;
        case(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG): return 3;
        case(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG): return 4;
        case(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG): return 4;
        case(GL_ETC1_RGB8_OES): return 3;
        case(GL_COMPRESSED_RGB8_ETC2): return 3;
        case(GL_COMPRESSED_SRGB8_ETC2): return 3;
        case(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2): return 4;
        case(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2): return 4;
        case(GL_COMPRESSED_RGBA8_ETC2_EAC): return 4;
        case(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC): return 4;
        case(GL_COMPRESSED_R11_EAC): return 1;
        case(GL_COMPRESSED_SIGNED_R11_EAC): return 1;
        case(GL_COMPRESSED_RG11_EAC): return 2;
        case(GL_COMPRESSED_SIGNED_RG11_EAC): return 2;
        case(GL_COLOR_INDEX): return 1;
        case(GL_STENCIL_INDEX): return 1;
        case(GL_DEPTH_COMPONENT): return 1;
        case(GL_DEPTH_COMPONENT16): return 1;
        case(GL_DEPTH_COMPONENT24): return 1;
        case(GL_DEPTH_COMPONENT32): return 1;
        case(GL_DEPTH_COMPONENT32F): return 1;
        case(GL_DEPTH_COMPONENT32F_NV): return 1;
        case(GL_RED): return 1;
        case(GL_GREEN): return 1;
        case(GL_BLUE): return 1;
        case(GL_ALPHA): return 1;
        case(GL_ALPHA8I): return 1;
        case(GL_ALPHA8UI): return 1;
        case(GL_ALPHA16I): return 1;
        case(GL_ALPHA16UI): return 1;
        case(GL_ALPHA32I): return 1;
        case(GL_ALPHA32UI): return 1;
        case(GL_ALPHA16F): return 1;
        case(GL_ALPHA32F): return 1;
        case(GL_R16F): return 1;
        case(GL_R32F): return 1;
        case(GL_R8): return 1;
        case(GL_R8_SNORM): return 1;
        case(GL_R16): return 1;
        case(GL_R16_SNORM): return 1;
        case(GL_R8I): return 1;
        case(GL_R8UI): return 1;
        case(GL_R16I): return 1;
        case(GL_R16UI): return 1;
        case(GL_R32I): return 1;
        case(GL_R32UI): return 1;
        case(GL_RG): return 2;
        case(GL_RG16F): return 2;
        case(GL_RG32F): return 2;
        case(GL_RG8): return 2;
        case(GL_RG8_SNORM): return 2;
        case(GL_RG16): return 2;
        case(GL_RG16_SNORM): return 2;
        case(GL_RG8I): return 2;
        case(GL_RG8UI): return 2;
        case(GL_RG16I): return 2;
        case(GL_RG16UI): return 2;
        case(GL_RG32I): return 2;
        case(GL_RG32UI): return 2;
        case(GL_RGB): return 3;
        case(GL_BGR): return 3;
        case(GL_RGB8I): return 3;
        case(GL_RGB8UI): return 3;
        case(GL_RGB16I): return 3;
        case(GL_RGB16UI): return 3;
        case(GL_RGB32I): return 3;
        case(GL_RGB32UI): return 3;
        case(GL_RGB16F): return 3;
        case(GL_RGB32F): return 3;
        case(GL_RGBA16F): return 4;
        case(GL_RGBA32F): return 4;
        case(GL_RGBA): return 4;
        case(GL_BGRA): return 4;
        case(GL_RGBA8): return 4;
        case(GL_LUMINANCE): return 1;
        case(GL_LUMINANCE4): return 1;
        case(GL_LUMINANCE8): return 1;
        case(GL_LUMINANCE12): return 1;
        case(GL_LUMINANCE16): return 1;
        case(GL_LUMINANCE8I): return 1;
        case(GL_LUMINANCE8UI): return 1;
        case(GL_LUMINANCE16I): return 1;
        case(GL_LUMINANCE16UI): return 1;
        case(GL_LUMINANCE32I): return 1;
        case(GL_LUMINANCE32UI): return 1;
        case(GL_LUMINANCE16F): return 1;
        case(GL_LUMINANCE32F): return 1;
        case(GL_LUMINANCE4_ALPHA4): return 2;
        case(GL_LUMINANCE6_ALPHA2): return 2;
        case(GL_LUMINANCE8_ALPHA8): return 2;
        case(GL_LUMINANCE12_ALPHA4): return 2;
        case(GL_LUMINANCE12_ALPHA12): return 2;
        case(GL_LUMINANCE16_ALPHA16): return 2;
        case(GL_INTENSITY): return 1;
        case(GL_INTENSITY4): return 1;
        case(GL_INTENSITY8): return 1;
        case(GL_INTENSITY12): return 1;
        case(GL_INTENSITY16): return 1;
        case(GL_INTENSITY8UI): return 1;
        case(GL_INTENSITY8I): return 1;
        case(GL_INTENSITY16I): return 1;
        case(GL_INTENSITY16UI): return 1;
        case(GL_INTENSITY32I): return 1;
        case(GL_INTENSITY32UI): return 1;
        case(GL_INTENSITY16F): return 1;
        case(GL_INTENSITY32F): return 1;
        case(GL_LUMINANCE_ALPHA): return 2;
        case(GL_LUMINANCE_ALPHA8I): return 2;
        case(GL_LUMINANCE_ALPHA8UI): return 2;
        case(GL_LUMINANCE_ALPHA16I): return 2;
        case(GL_LUMINANCE_ALPHA16UI): return 2;
        case(GL_LUMINANCE_ALPHA32I): return 2;
        case(GL_LUMINANCE_ALPHA32UI): return 2;
        case(GL_LUMINANCE_ALPHA16F): return 2;
        case(GL_LUMINANCE_ALPHA32F): return 2;
        case(GL_HILO_NV): return 2;
        case(GL_DSDT_NV): return 2;
        case(GL_DSDT_MAG_NV): return 3;
        case(GL_DSDT_MAG_VIB_NV): return 4;
        case(GL_RED_INTEGER): return 1;
        case(GL_GREEN_INTEGER): return 1;
        case(GL_BLUE_INTEGER): return 1;
        case(GL_ALPHA_INTEGER): return 1;
        case(GL_RGB_INTEGER): return 3;
        case(GL_RGBA_INTEGER): return 4;
        case(GL_BGR_INTEGER): return 3;
        case(GL_BGRA_INTEGER): return 4;
        case(GL_LUMINANCE_INTEGER): return 1;
        case(GL_LUMINANCE_ALPHA_INTEGER): return 2;
        case(GL_SRGB8) : return 3;
        case(GL_SRGB8_ALPHA8) : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_4x4_KHR)           : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_5x4_KHR)           : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_5x5_KHR)           : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_6x5_KHR)           : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_6x6_KHR)           : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_8x5_KHR)           : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_8x6_KHR)           : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_8x8_KHR)           : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_10x5_KHR)          : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_10x6_KHR)          : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_10x8_KHR)          : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_10x10_KHR)         : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_12x10_KHR)         : return 4;
        case (GL_COMPRESSED_RGBA_ASTC_12x12_KHR)         : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR)   : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR)   : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR)   : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR)   : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR)   : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR)   : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR)   : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR)   : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR)  : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR)  : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR)  : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR) : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR) : return 4;
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR) : return 4;
        default:
        {
            ////OSG_WARN<<"error pixelFormat = "<<std::hex<<pixelFormat<<std::dec<<std::endl;
            return 0;
        }
    }
}


unsigned int Image::computePixelSizeInBits(GLenum format,GLenum type)
{

    switch(format)
    {
        case(GL_COMPRESSED_RGB_S3TC_DXT1): return 4;
        case(GL_COMPRESSED_RGBA_S3TC_DXT1): return 4;
        case(GL_COMPRESSED_RGBA_S3TC_DXT3): return 8;
        case(GL_COMPRESSED_RGBA_S3TC_DXT5): return 8;
        case(GL_COMPRESSED_SIGNED_RED_RGTC1): return 4;
        case(GL_COMPRESSED_RED_RGTC1):   return 4;
        case(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2): return 8;
        case(GL_COMPRESSED_RED_GREEN_RGTC2): return 8;
        case(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG): return 4;
        case(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG): return 2;
        case(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG): return 4;
        case(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG): return 2;
        case(GL_ETC1_RGB8_OES): return 4;
        case(GL_COMPRESSED_RGB8_ETC2): return 4;
        case(GL_COMPRESSED_SRGB8_ETC2): return 4;
        case(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2): return 4;
        case(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2): return 4;
        case(GL_COMPRESSED_RGBA8_ETC2_EAC): return 8;
        case(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC): return 8;
        case(GL_COMPRESSED_R11_EAC): return 4;
        case(GL_COMPRESSED_SIGNED_R11_EAC): return 4;
        case(GL_COMPRESSED_RG11_EAC): return 8;
        case(GL_COMPRESSED_SIGNED_RG11_EAC): return 8;
        default: break;
    }

    // note, haven't yet added proper handling of the ARB GL_COMPRESSRED_* pathways
    // yet, no clear size for these since its probably implementation dependent
    // which raises the question of how to actually query for these sizes...
    // will need to revisit this issue, for now just report an error.
    // this is possible a bit of mute point though as since the ARB compressed formats
    // aren't yet used for storing images to disk, so its likely that users wont have
    // Image's for pixel formats set the ARB compressed formats, just using these
    // compressed formats as internal texture modes.  This is very much speculation though
    // if get the below error then its time to revist this issue :-)
    // Robert Osfield, Jan 2005.
    switch(format)
    {
        case(GL_COMPRESSED_ALPHA):
        case(GL_COMPRESSED_LUMINANCE):
        case(GL_COMPRESSED_LUMINANCE_ALPHA):
        case(GL_COMPRESSED_INTENSITY):
        case(GL_COMPRESSED_RGB):
        case(GL_COMPRESSED_RGBA):
            ////OSG_WARN<<"Image::computePixelSizeInBits(format,type) : cannot compute correct size of compressed format ("<<format<<") returning 0."<<std::endl;
            return 0;
        default: break;
    }
    switch (format)
    {//handle GL_KHR_texture_compression_astc_hdr
        case (GL_COMPRESSED_RGBA_ASTC_4x4_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_5x4_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_5x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_6x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_6x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x8_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x8_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x10_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_12x10_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_12x12_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR) :
        {
            Vec3i footprint = computeBlockFootprint(format);
            unsigned int pixelsPerBlock = footprint.x * footprint.y;
            unsigned int bitsPerBlock = computeBlockSize(format, 0);//16 x 8 = 128
            unsigned int bitsPerPixel = bitsPerBlock / pixelsPerBlock;
            if (bitsPerBlock == bitsPerPixel * pixelsPerBlock) {
                ////OSG_WARN << "Image::computePixelSizeInBits(format,type) : bits per pixel (" << bitsPerPixel << ") is not an integer for GL_KHR_texture_compression_astc_hdr sizes other than 4x4 and 8x8." << std::endl;
                return bitsPerPixel;
            } else {
                ////OSG_WARN << "Image::computePixelSizeInBits(format,type) : bits per pixel (" << bitsPerBlock << "/" << pixelsPerBlock << ") is not an integer for GL_KHR_texture_compression_astc_hdr size" << footprint.x  << "x" << footprint.y << "." << std::endl;
            }
            return 0;
        }
        default: break;
    }

    switch(format)
    {
        case(GL_LUMINANCE4): return 4;
        case(GL_LUMINANCE8): return 8;
        case(GL_LUMINANCE12): return 12;
        case(GL_LUMINANCE16): return 16;
        case(GL_LUMINANCE4_ALPHA4): return 8;
        case(GL_LUMINANCE6_ALPHA2): return 8;
        case(GL_LUMINANCE8_ALPHA8): return 16;
        case(GL_LUMINANCE12_ALPHA4): return 16;
        case(GL_LUMINANCE12_ALPHA12): return 24;
        case(GL_LUMINANCE16_ALPHA16): return 32;
        case(GL_INTENSITY4): return 4;
        case(GL_INTENSITY8): return 8;
        case(GL_INTENSITY12): return 12;
        case(GL_INTENSITY16): return 16;
        default: break;
    }

    switch(type)
    {

        case(GL_BITMAP): return computeNumComponents(format);

        case(GL_BYTE):
        case(GL_UNSIGNED_BYTE): return 8*computeNumComponents(format);

        case(GL_HALF_FLOAT):
        case(GL_SHORT):
        case(GL_UNSIGNED_SHORT): return 16*computeNumComponents(format);

        case(GL_INT):
        case(GL_UNSIGNED_INT):
        case(GL_FLOAT): return 32*computeNumComponents(format);


        case(GL_UNSIGNED_BYTE_3_3_2):
        case(GL_UNSIGNED_BYTE_2_3_3_REV): return 8;

        case(GL_UNSIGNED_SHORT_5_6_5):
        case(GL_UNSIGNED_SHORT_5_6_5_REV):
        case(GL_UNSIGNED_SHORT_4_4_4_4):
        case(GL_UNSIGNED_SHORT_4_4_4_4_REV):
        case(GL_UNSIGNED_SHORT_5_5_5_1):
        case(GL_UNSIGNED_SHORT_1_5_5_5_REV): return 16;

        case(GL_UNSIGNED_INT_8_8_8_8):
        case(GL_UNSIGNED_INT_8_8_8_8_REV):
        case(GL_UNSIGNED_INT_10_10_10_2):
        case(GL_UNSIGNED_INT_2_10_10_10_REV): return 32;
        default:
        {
            ////OSG_WARN<<"error type = "<<type<<std::endl;
            return 0;
        }
    }

}

Vec3i Image::computeBlockFootprint(GLenum pixelFormat)
{
    switch (pixelFormat)
    {
        case(GL_COMPRESSED_RGB_S3TC_DXT1) :
        case(GL_COMPRESSED_RGBA_S3TC_DXT1) :
        case(GL_COMPRESSED_RGBA_S3TC_DXT3) :
        case(GL_COMPRESSED_RGBA_S3TC_DXT5) :
            return Vec3i(4,4,4);//opengl 3d dxt: r value means (max)4 consecutive blocks in r direction packed into a slab.

        case(GL_COMPRESSED_SIGNED_RED_RGTC1) :
        case(GL_COMPRESSED_RED_RGTC1) :
        case(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2) :
        case(GL_COMPRESSED_RED_GREEN_RGTC2) :
        case(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG) :
        case(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG) :
        case(GL_ETC1_RGB8_OES) :
        case(GL_COMPRESSED_RGB8_ETC2) :
        case(GL_COMPRESSED_SRGB8_ETC2) :
        case(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2) :
        case(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2) :
        case(GL_COMPRESSED_RGBA8_ETC2_EAC) :
        case(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC) :
        case(GL_COMPRESSED_R11_EAC) :
        case(GL_COMPRESSED_SIGNED_R11_EAC) :
        case(GL_COMPRESSED_RG11_EAC) :
        case(GL_COMPRESSED_SIGNED_RG11_EAC) :
            return Vec3i(4, 4, 1);//not sure about r
        case(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG) :
        case(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG) :
            return Vec3i(8, 4, 1);//no 3d texture support in pvrtc at all
        case (GL_COMPRESSED_RGBA_ASTC_4x4_KHR) : return Vec3i(4, 4, 1);
        case (GL_COMPRESSED_RGBA_ASTC_5x4_KHR) : return Vec3i(5, 4, 1);
        case (GL_COMPRESSED_RGBA_ASTC_5x5_KHR) : return Vec3i(5, 5, 1);
        case (GL_COMPRESSED_RGBA_ASTC_6x5_KHR) : return Vec3i(6, 5, 1);
        case (GL_COMPRESSED_RGBA_ASTC_6x6_KHR) : return Vec3i(6, 6, 1);
        case (GL_COMPRESSED_RGBA_ASTC_8x5_KHR) : return Vec3i(8, 5, 1);
        case (GL_COMPRESSED_RGBA_ASTC_8x6_KHR) : return Vec3i(8, 6, 1);
        case (GL_COMPRESSED_RGBA_ASTC_8x8_KHR) : return Vec3i(8, 8, 1);
        case (GL_COMPRESSED_RGBA_ASTC_10x5_KHR) : return Vec3i(10, 5, 1);
        case (GL_COMPRESSED_RGBA_ASTC_10x6_KHR) : return Vec3i(10, 6, 1);
        case (GL_COMPRESSED_RGBA_ASTC_10x8_KHR) : return Vec3i(10, 8, 1);
        case (GL_COMPRESSED_RGBA_ASTC_10x10_KHR) : return Vec3i(10, 10, 1);
        case (GL_COMPRESSED_RGBA_ASTC_12x10_KHR) : return Vec3i(12, 10, 1);
        case (GL_COMPRESSED_RGBA_ASTC_12x12_KHR) : return Vec3i(12, 12, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR) : return Vec3i(4, 4, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR) : return Vec3i(5, 4, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR) : return Vec3i(5, 5, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR) : return Vec3i(6, 5, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR) : return Vec3i(6, 6, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR) : return Vec3i(8, 5, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR) : return Vec3i(8, 6, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR) : return Vec3i(8, 8, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR) : return Vec3i(10, 5, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR) : return Vec3i(10, 6, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR) : return Vec3i(10, 8, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR) : return Vec3i(10, 10, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR) : return Vec3i(12, 10, 1);
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR) : return Vec3i(12, 12, 1);

        default:
            break;
    }
    return Vec3i(1,1,1);
}
//returns the max(size of a 2D block in bytes,packing)
unsigned int Image::computeBlockSize(GLenum pixelFormat, GLenum packing)
{
    switch(pixelFormat)
    {
        case(GL_COMPRESSED_RGB_S3TC_DXT1):
        case(GL_COMPRESSED_RGBA_S3TC_DXT1):
            return b3Max(8u,packing); // block size of 8
        case(GL_COMPRESSED_RGBA_S3TC_DXT3):
        case(GL_COMPRESSED_RGBA_S3TC_DXT5):
        case(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG):
        case(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG):
        case(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG):
        case(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG):
        case(GL_ETC1_RGB8_OES):
            return b3Max(16u,packing); // block size of 16
        case(GL_COMPRESSED_SIGNED_RED_RGTC1):
        case(GL_COMPRESSED_RED_RGTC1):
            return b3Max(8u,packing); // block size of 8
            break;
        case(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2):
        case(GL_COMPRESSED_RED_GREEN_RGTC2):
            return b3Max(16u,packing); // block size of 16

        case(GL_COMPRESSED_RGB8_ETC2):
        case(GL_COMPRESSED_SRGB8_ETC2):
        case(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2):
        case(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2):
        case(GL_COMPRESSED_R11_EAC):
        case(GL_COMPRESSED_SIGNED_R11_EAC):
            return b3Max(8u,packing); // block size of 8

        case(GL_COMPRESSED_RGBA8_ETC2_EAC):
        case(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC):
        case(GL_COMPRESSED_RG11_EAC):
        case(GL_COMPRESSED_SIGNED_RG11_EAC):
            return b3Max(16u,packing); // block size of 16
        case (GL_COMPRESSED_RGBA_ASTC_4x4_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_5x4_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_5x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_6x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_6x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x8_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x8_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x10_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_12x10_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_12x12_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR) :
            return b3Max(16u, packing); // block size of 16
        default:
            break;
    }
    return packing;
}

unsigned int Image::computeRowWidthInBytes(int width,GLenum pixelFormat,GLenum type,int packing)
{
    unsigned int pixelSize = computePixelSizeInBits(pixelFormat,type);
    int widthInBits = width*pixelSize;
    int packingInBits = packing!=0 ? packing*8 : 8;
    //OSG_INFO << "width="<<width<<" pixelSize="<<pixelSize<<"  width in bit="<<widthInBits<<" packingInBits="<<packingInBits<<" widthInBits%packingInBits="<<widthInBits%packingInBits<<std::endl;
    return (widthInBits/packingInBits + ((widthInBits%packingInBits)?1:0))*packing;
}

unsigned int Image::computeImageSizeInBytes(int width,int height, int depth, GLenum pixelFormat,GLenum type,int packing, int slice_packing, int image_packing)
{
    if (width<=0 || height<=0 || depth<=0) return 0;

    int blockSize = computeBlockSize(pixelFormat, 0);
    if (blockSize > 0) {
        Vec3i footprint = computeBlockFootprint(pixelFormat);
        width = (width + footprint.x - 1) / footprint.x;
        height = (height + footprint.y - 1) / footprint.y;
        unsigned int size = blockSize * width;
        size = roudUpToMultiple(size, packing);
        size *= height;
        size = roudUpToMultiple(size, slice_packing);
        size *= depth;
        size = roudUpToMultiple(size, image_packing);
        return size;
    }

    // compute size of one row
    unsigned int size = Image::computeRowWidthInBytes( width, pixelFormat, type, packing );

    // now compute size of slice
    size *= height;
    size += slice_packing - 1;
    size -= size % slice_packing;

    // compute size of whole image
    size *= depth;
    size += image_packing - 1;
    size -= size % image_packing;

    return b3Max( size, computeBlockSize(pixelFormat, packing) );
}

int Image::roudUpToMultiple(int s, int pack) {
    if (pack < 2) return s;
    s += pack - 1;
    s -= s % pack;
    return s;
}

int Image::computeNearestPowerOfTwo(int s,float bias)
{
    if ((s & (s-1))!=0)
    {
        // it isn't so lets find the closest power of two.
        // yes, logf and powf are slow, but this code should
        // only be called during scene graph initialization,
        // if at all, so not critical in the greater scheme.
        float p2 = logf((float)s)/logf(2.0f);
        float rounded_p2 = floorf(p2+bias);
        s = (int)(powf(2.0f,rounded_p2));
    }
    return s;
}

int Image::computeNumberOfMipmapLevels(int s,int t, int r)
{
    int w = b3Max(s, t);
    w = b3Max(w, r);

    int n = 0;
    while (w >>= 1)
        ++n;
    return n+1;
}

bool Image::isCompressed() const
{
    switch(_pixelFormat)
    {
        case(GL_COMPRESSED_ALPHA):
        case(GL_COMPRESSED_INTENSITY):
        case(GL_COMPRESSED_LUMINANCE_ALPHA):
        case(GL_COMPRESSED_LUMINANCE):
        case(GL_COMPRESSED_RGBA):
        case(GL_COMPRESSED_RGB):
        case(GL_COMPRESSED_RGB_S3TC_DXT1):
        case(GL_COMPRESSED_RGBA_S3TC_DXT1):
        case(GL_COMPRESSED_RGBA_S3TC_DXT3):
        case(GL_COMPRESSED_RGBA_S3TC_DXT5):
        case(GL_COMPRESSED_SIGNED_RED_RGTC1):
        case(GL_COMPRESSED_RED_RGTC1):
        case(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2):
        case(GL_COMPRESSED_RED_GREEN_RGTC2):
        case(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG):
        case(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG):
        case(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG):
        case(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG):
        case(GL_ETC1_RGB8_OES):
        case(GL_COMPRESSED_RGB8_ETC2):
        case(GL_COMPRESSED_SRGB8_ETC2):
        case(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2):
        case(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2):
        case(GL_COMPRESSED_RGBA8_ETC2_EAC):
        case(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC):
        case(GL_COMPRESSED_R11_EAC):
        case(GL_COMPRESSED_SIGNED_R11_EAC):
        case(GL_COMPRESSED_RG11_EAC):
        case(GL_COMPRESSED_SIGNED_RG11_EAC):
        case (GL_COMPRESSED_RGBA_ASTC_4x4_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_5x4_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_5x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_6x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_6x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_8x8_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x5_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x6_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x8_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_10x10_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_12x10_KHR) :
        case (GL_COMPRESSED_RGBA_ASTC_12x12_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR) :
        case (GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR) :
            return true;
        default:
            return false;
    }
}

unsigned int Image::getTotalSizeInBytesIncludingMipmaps() const
{
    if (_mipmapData.empty())
    {
        // no mips so just return size of main image
        return getTotalSizeInBytes();
    }

    int s = _s;
    int t = _t;
    int r = _r;
    unsigned int totalSize = 0;
    for(unsigned int i=0;i<_mipmapData.size()+1;++i)
    {
        totalSize += computeImageSizeInBytes(s, t, r, _pixelFormat, _dataType, _packing);

        s >>= 1;
        t >>= 1;
        r >>= 1;

        if (s<1) s=1;
        if (t<1) t=1;
        if (r<1) r=1;
   }

   return totalSize;
}

void Image::setRowLength(int length)
{
    _rowLength = length;
 
}

void Image::setInternalTextureFormat(GLint internalFormat)
{
    // won't do any sanity checking right now, leave it to
    // OpenGL to make the call.
    _internalTextureFormat = internalFormat;
}

void Image::setPixelFormat(GLenum pixelFormat)
{
    _pixelFormat = pixelFormat;
}

void Image::setDataType(GLenum dataType)
{
    if (_dataType==dataType) return; // do nothing if the same.

    if (_dataType==0)
    {
        // setting the datatype for the first time
        _dataType = dataType;
    }
    else
    {
        ////OSG_WARN<<"Image::setDataType(..) - warning, attempt to reset the data type not permitted."<<std::endl;
    }
}


void Image::allocateImage(int s,int t,int r,
                        GLenum format,GLenum type,
                        int packing)
{
    _mipmapData.clear();

    bool callback_needed(false);

    unsigned int previousTotalSize = 0;

    if (_data) previousTotalSize = computeRowWidthInBytes(_s,_pixelFormat,_dataType,_packing)*_t*_r;

    unsigned int newTotalSize = computeRowWidthInBytes(s,format,type,packing)*t*r;

    if (newTotalSize!=previousTotalSize)
    {
        if (newTotalSize)
            setData(new unsigned char [newTotalSize],USE_NEW_DELETE);
        else
            deallocateData(); // and sets it to NULL.
    }

    if (_data)
    {
        callback_needed = (_s != s) || (_t != t) || (_r != r);
        _s = s;
        _t = t;
        _r = r;
        _pixelFormat = format;
        _dataType = type;
        _packing = packing;
        _rowLength = 0;

        // preserve internalTextureFormat if already set, otherwise
        // use the pixelFormat as the source for the format.
        if (_internalTextureFormat==0) _internalTextureFormat = format;
    }
    else
    {
        callback_needed = (_s != 0) || (_t != 0) || (_r != 0);

        // failed to allocate memory, for now, will simply set values to 0.
        _s = 0;
        _t = 0;
        _r = 0;
        _pixelFormat = 0;
        _dataType = 0;
        _packing = 0;
        _rowLength = 0;

        // commenting out reset of _internalTextureFormat as we are changing
        // policy so that allocateImage honours previous settings of _internalTextureFormat.
        //_internalTextureFormat = 0;
    }

 
    dirty();
}

void Image::setImage(int s,int t,int r,
                     GLint internalTextureFormat,
                     GLenum format,GLenum type,
                     unsigned char *data,
                     AllocationMode mode,
                     int packing,
                     int rowLength)
{
    _mipmapData.clear();

    bool callback_needed = (_s != s) || (_t != t) || (_r != r);

    _s = s;
    _t = t;
    _r = r;

    _internalTextureFormat = internalTextureFormat;
    _pixelFormat    = format;
    _dataType       = type;

    setData(data,mode);

    _packing = packing;
    _rowLength = rowLength;

    dirty();

   
}

void Image::readPixels(int x,int y,int width,int height,
                       GLenum format, GLenum type, int packing)
{
    allocateImage(width,height,1,format,type, packing);

    glPixelStorei(GL_PACK_ALIGNMENT,_packing);
    glPixelStorei(GL_PACK_ROW_LENGTH,_rowLength);

    glReadPixels(x,y,width,height,format,type,_data);
}


void Image::readImageFromCurrentTexture(unsigned int contextID, bool copyMipMapsIfAvailable, GLenum type, unsigned int face)
{
#ifndef BT_USE_EGL
    // //OSG_NOTICE<<"Image::readImageFromCurrentTexture()"<<std::endl;
     
    GLboolean binding1D = GL_FALSE, binding2D = GL_FALSE, bindingRect = GL_FALSE, binding3D = GL_FALSE, binding2DArray = GL_FALSE, bindingCubeMap = GL_FALSE;

    glGetBooleanv(GL_TEXTURE_BINDING_1D, &binding1D);
    glGetBooleanv(GL_TEXTURE_BINDING_2D, &binding2D);
    glGetBooleanv(GL_TEXTURE_BINDING_RECTANGLE, &bindingRect);
    glGetBooleanv(GL_TEXTURE_BINDING_3D, &binding3D);
    glGetBooleanv(GL_TEXTURE_BINDING_CUBE_MAP, &bindingCubeMap);

    if (true)//Texture2DArraySupported
    {
        glGetBooleanv(GL_TEXTURE_BINDING_2D_ARRAY, &binding2DArray);
    }

    GLenum textureMode = binding1D ? GL_TEXTURE_1D : binding2D ? GL_TEXTURE_2D : bindingRect ? GL_TEXTURE_RECTANGLE : binding3D ? GL_TEXTURE_3D : binding2DArray ? GL_TEXTURE_2D_ARRAY : 0;
    if (bindingCubeMap)
    {
        switch (face)
        {
            case TextureCubeMap::POSITIVE_X:
                textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                break;
            case TextureCubeMap::NEGATIVE_X:
                textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
                break;
            case TextureCubeMap::POSITIVE_Y:
                textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
                break;
            case TextureCubeMap::NEGATIVE_Y:
                textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
                break;
            case TextureCubeMap::POSITIVE_Z:
                textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
                break;
            case TextureCubeMap::NEGATIVE_Z:
                textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
                break;
        }
    }

    if (textureMode==0) return;

    GLint internalformat;
    GLint width;
    GLint height;
    GLint depth;
    GLint packing;
    GLint rowLength;

    GLint numMipMaps = 0;
    if (copyMipMapsIfAvailable)
    {
        for(;numMipMaps<20;++numMipMaps)
        {
            glGetTexLevelParameteriv(textureMode, numMipMaps, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(textureMode, numMipMaps, GL_TEXTURE_HEIGHT, &height);
            glGetTexLevelParameteriv(textureMode, numMipMaps, GL_TEXTURE_DEPTH, &depth);
            // //OSG_NOTICE<<"   numMipMaps="<<numMipMaps<<" width="<<width<<" height="<<height<<" depth="<<depth<<std::endl;
            if (width==0 || height==0 || depth==0) break;
        }
    }
    else
    {
        numMipMaps = 1;
    }

    // //OSG_NOTICE<<"Image::readImageFromCurrentTexture() : numMipMaps = "<<numMipMaps<<std::endl;


    GLint compressed = 0;

    if (textureMode==GL_TEXTURE_2D)
    {
        if (true)
        {
            glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_COMPRESSED,&compressed);
        }
    }
    else if (textureMode==GL_TEXTURE_3D)
    {
        if (true)
        {
            glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_COMPRESSED,&compressed);
        }
    }
    else if (textureMode==GL_TEXTURE_2D_ARRAY)
    {
        if (true)
        {
            glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_COMPRESSED,&compressed);
        }
    }
    else if(bindingCubeMap)
    {
        if (true)
        {
            glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_COMPRESSED,&compressed);
        }
    }



    /* if the compression has been successful */
    if (compressed == GL_TRUE)
    {

        MipmapDataType mipMapData;

        unsigned int total_size = 0;
        GLint i;
        for(i=0;i<numMipMaps;++i)
        {
            if (i>0) mipMapData.push_back(total_size);

            GLint compressed_size;
            glGetTexLevelParameteriv(textureMode, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size);

            total_size += compressed_size;
        }


        unsigned char* data = new unsigned char[total_size];
        if (!data)
        {
            ////OSG_WARN<<"Warning: Image::readImageFromCurrentTexture(..) out of memory, no image read."<<std::endl;
            return;
        }

        deallocateData(); // and sets it to NULL.

        glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
        glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_DEPTH, &depth);
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &packing);
        glPixelStorei(GL_PACK_ALIGNMENT, packing);
        glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowLength);
        glPixelStorei(GL_PACK_ROW_LENGTH, rowLength);

        _data = data;
        _s = width;
        _t = height;
        _r = depth;

        _pixelFormat = internalformat;
        _dataType = type;
        _internalTextureFormat = internalformat;
        _mipmapData = mipMapData;
        _allocationMode=USE_NEW_DELETE;
        _packing = packing;
        _rowLength = rowLength;

        for(i=0;i<numMipMaps;++i)
        {
            glGetCompressedTexImage(textureMode, i, getMipmapData(i));
        }

        dirty();

    }
    else
    {
        MipmapDataType mipMapData;

        // Get the internal texture format and packing value from OpenGL,
        // instead of using possibly outdated values from the class.
        glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &packing);
        glPixelStorei(GL_PACK_ALIGNMENT, packing);
        glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowLength);
        glPixelStorei(GL_PACK_ROW_LENGTH, rowLength);

        unsigned int total_size = 0;
        GLint i;
        for(i=0;i<numMipMaps;++i)
        {
            if (i>0) mipMapData.push_back(total_size);

            glGetTexLevelParameteriv(textureMode, i, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(textureMode, i, GL_TEXTURE_HEIGHT, &height);
            glGetTexLevelParameteriv(textureMode, i, GL_TEXTURE_DEPTH, &depth);

            unsigned int level_size = computeRowWidthInBytes(width,internalformat,type,packing)*height*depth;

            total_size += level_size;
        }


        unsigned char* data = new unsigned char[total_size];
        if (!data)
        {
            ////OSG_WARN<<"Warning: Image::readImageFromCurrentTexture(..) out of memory, no image read."<<std::endl;
            return;
        }

        deallocateData(); // and sets it to NULL.

        glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(textureMode, 0, GL_TEXTURE_DEPTH, &depth);

        _data = data;
        _s = width;
        _t = height;
        _r = depth;

        _pixelFormat = computePixelFormat(internalformat);
        _dataType = type;
        _internalTextureFormat = internalformat;
        _mipmapData = mipMapData;
        _allocationMode=USE_NEW_DELETE;
        _packing = packing;
        _rowLength = rowLength;

        for(i=0;i<numMipMaps;++i)
        {
            glGetTexImage(textureMode,i,_pixelFormat,_dataType,getMipmapData(i));
        }

        dirty();
    }
#else
    ////OSG_NOTICE<<"Warning: Image::readImageFromCurrentTexture() not supported."<<std::endl;
#endif
}

void Image::swap(Image& rhs)
{
    std::swap(_fileName, rhs._fileName);
    std::swap(_writeHint, rhs._writeHint);

    std::swap(_origin, rhs._origin);

    std::swap(_s, rhs._s); std::swap(_t, rhs._t); std::swap(_r, rhs._r);
    std::swap(_rowLength, rhs._rowLength);
    std::swap(_internalTextureFormat, rhs._internalTextureFormat);
    std::swap(_pixelFormat, rhs._pixelFormat);
    std::swap(_dataType, rhs._dataType);
    std::swap(_packing, rhs._packing);
    std::swap(_pixelAspectRatio, rhs._pixelAspectRatio);

    std::swap(_allocationMode, rhs._allocationMode);
    std::swap(_data, rhs._data);

    std::swap(_mipmapData, rhs._mipmapData);
     
}

 
void Image::flipHorizontal()
{
    if (_data==NULL)
    {
        ////OSG_WARN << "Error Image::flipHorizontal() did not succeed : cannot flip NULL image."<<std::endl;
        return;
    }

    unsigned int elemSize = getPixelSizeInBits()/8;

    if (_mipmapData.empty())
    {
        unsigned int rowStepInBytes = getRowStepInBytes();
        unsigned int imageStepInBytes = getImageStepInBytes();

        for(int r=0;r<_r;++r)
        {
            for (int t=0; t<_t; ++t)
            {
                unsigned char* rowData = _data + t*rowStepInBytes + r*imageStepInBytes;
                unsigned char* left  = rowData ;
                unsigned char* right = rowData + ((_s-1)*getPixelSizeInBits())/8;

                while (left < right)
                {
                    char tmp[32];  // max elem size is four floats
                    memcpy(tmp, left, elemSize);
                    memcpy(left, right, elemSize);
                    memcpy(right, tmp, elemSize);
                    left  += elemSize;
                    right -= elemSize;
                }
            }
        }
    }
    else
    {
        ////OSG_WARN << "Error Image::flipHorizontal() did not succeed : cannot flip mipmapped image."<<std::endl;
        return;
    }

    dirty();
}

void flipImageVertical(unsigned char* top, unsigned char* bottom, unsigned int rowSize, unsigned int rowStep)
{
    while(top<bottom)
    {
        unsigned char* t = top;
        unsigned char* b = bottom;
        for(unsigned int i=0;i<rowSize;++i, ++t,++b)
        {
            unsigned char temp=*t;
            *t = *b;
            *b = temp;
        }
        top += rowStep;
        bottom -= rowStep;
    }
}


void Image::flipVertical()
{
    if (_data==NULL)
    {
        ////OSG_WARN << "Error Image::flipVertical() do not succeed : cannot flip NULL image."<<std::endl;
        return;
    }

    if (!_mipmapData.empty() && _r>1)
    {
        ////OSG_WARN << "Error Image::flipVertical() do not succeed : flipping of mipmap 3d textures not yet supported."<<std::endl;
        return;
    }

    unsigned int rowSize = getRowSizeInBytes();
    unsigned int rowStep = getRowStepInBytes();

    const bool dxtc(dxtc_tool::isDXTC(_pixelFormat));
    if (_mipmapData.empty())
    {
        // no mipmaps,
        // so we can safely handle 3d textures
        for(int r=0;r<_r;++r)
        {
            if (dxtc)
            {
                if (!dxtc_tool::VerticalFlip(_s,_t,_pixelFormat,data(0,0,r)))
                {
                    ////OSG_NOTICE << "Notice Image::flipVertical(): Vertical flip do not succeed" << std::endl;
                }
            }
            else
            {
                if (isCompressed())
                {   ; }
                    ////OSG_NOTICE << "Notice Image::flipVertical(): image is compressed but normal v-flip is used" << std::endl;
                // its not a compressed image, so implement flip oursleves.
                unsigned char* top = data(0,0,r);
                unsigned char* bottom = top + (_t-1)*rowStep;

                flipImageVertical(top, bottom, rowSize, rowStep);
            }
        }
    }
    else if (_r==1)
    {
        if (dxtc)
        {
            if (!dxtc_tool::VerticalFlip(_s,_t,_pixelFormat,_data))
            {
                ////OSG_NOTICE << "Notice Image::flipVertical(): Vertical flip do not succeed" << std::endl;
            }
        }
        else
        {
            if (isCompressed())
            {   ; }
            // //OSG_NOTICE << "Notice Image::flipVertical(): image is compressed but normal v-flip is used" << std::endl;
            // its not a compressed image, so implement flip oursleves.
            unsigned char* top = data(0,0,0);
            unsigned char* bottom = top + (_t-1)*rowStep;

            flipImageVertical(top, bottom, rowSize, rowStep);
        }

        int s = _s;
        int t = _t;
        //int r = _r;

        for(unsigned int i=0;i<_mipmapData.size() && _mipmapData[i];++i)
        {
            s >>= 1;
            t >>= 1;
            if (s==0) s=1;
            if (t==0) t=1;
            if (dxtc)
            {
                if (!dxtc_tool::VerticalFlip(s,t,_pixelFormat,_data+_mipmapData[i]))
                {
                    ////OSG_NOTICE << "Notice Image::flipVertical(): Vertical flip did not succeed" << std::endl;
                }
            }
            else
            {
                // it's not a compressed image, so implement flip ourselves.
                unsigned int mipRowSize = computeRowWidthInBytes(s, _pixelFormat, _dataType, _packing);
                unsigned int mipRowStep = mipRowSize;
                unsigned char* top = _data+_mipmapData[i];
                unsigned char* bottom = top + (t-1)*mipRowStep;

                flipImageVertical(top, bottom, mipRowSize, mipRowStep);
            }
       }
    }

    dirty();
}

void Image::flipDepth()
{
    if (_data==NULL)
    {
        ////OSG_WARN << "Error Image::flipVertical() do not succeed : cannot flip NULL image."<<std::endl;
        return;
    }

    if (_r==1)
    {
        return;
    }

    if (!_mipmapData.empty() && _r>1)
    {
        ////OSG_WARN << "Error Image::flipVertical() do not succeed : flipping of mipmap 3d textures not yet supported."<<std::endl;
        return;
    }

    unsigned int sizeOfRow = getRowSizeInBytes();

    int r_front = 0;
    int r_back = _r-1;
    for(; r_front<r_back; ++r_front,--r_back)
    {
        for(int row=0; row<_t; ++row)
        {
            unsigned char* front = data(0, row, r_front);
            unsigned char* back = data(0, row, r_back);
            for(unsigned int i=0; i<sizeOfRow; ++i, ++front, ++back)
            {
                std::swap(*front, *back);
            }
        }
    }
}


void Image::ensureValidSizeForTexturing(GLint maxTextureSize)
{
    int new_s = computeNearestPowerOfTwo(_s);
    int new_t = computeNearestPowerOfTwo(_t);

    if (new_s>maxTextureSize) new_s = maxTextureSize;
    if (new_t>maxTextureSize) new_t = maxTextureSize;

    if (new_s!=_s || new_t!=_t)
    {
 
    }
}

bool Image::supportsTextureSubloading() const
{
    switch(_internalTextureFormat)
    {
        case GL_ETC1_RGB8_OES:
        case(GL_COMPRESSED_RGB8_ETC2):
        case(GL_COMPRESSED_SRGB8_ETC2):
        case(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2):
        case(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2):
        case(GL_COMPRESSED_RGBA8_ETC2_EAC):
        case(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC):
        case(GL_COMPRESSED_R11_EAC):
        case(GL_COMPRESSED_SIGNED_R11_EAC):
        case(GL_COMPRESSED_RG11_EAC):
        case(GL_COMPRESSED_SIGNED_RG11_EAC):
            return false;
        default:
            return true;
    }
}


template <typename T>
bool _findLowerAlphaValueInRow(unsigned int num, T* data,T value, unsigned int delta)
{
    for(unsigned int i=0;i<num;++i)
    {
        if (*data<value) return true;
        data += delta;
    }
    return false;
}

template <typename T>
bool _maskedFindLowerAlphaValueInRow(unsigned int num, T* data,T value, T mask, unsigned int delta)
{
    for(unsigned int i=0;i<num;++i)
    {
        if ((*data & mask)<value) return true;
        data += delta;
    }
    return false;
}

bool Image::isImageTranslucent() const
{
    unsigned int offset = 0;
    unsigned int delta = 1;
    switch(_pixelFormat)
    {
        case(GL_ALPHA):
            offset = 0;
            delta = 1;
            break;
        case(GL_LUMINANCE_ALPHA):
            offset = 1;
            delta = 2;
            break;
        case(GL_RGBA):
            offset = 3;
            delta = 4;
            break;
        case(GL_BGRA):
            offset = 3;
            delta = 4;
            break;
        case(GL_RGB):
            return false;
        case(GL_BGR):
            return false;
        case(GL_COMPRESSED_RGB_S3TC_DXT1):
            return false;
        case(GL_COMPRESSED_RGBA_S3TC_DXT1):
        case(GL_COMPRESSED_RGBA_S3TC_DXT3):
        case(GL_COMPRESSED_RGBA_S3TC_DXT5):
            return dxtc_tool::isCompressedImageTranslucent(_s, _t, _pixelFormat, _data);
        default:
            return false;
    }

    for(int ir=0;ir<r();++ir)
    {
        for(int it=0;it<t();++it)
        {
            const unsigned char* d = data(0,it,ir);
            switch(_dataType)
            {
                case(GL_BYTE):
                    if (_findLowerAlphaValueInRow(s(), (char*)d +offset, (char)127, delta))
                        return true;
                    break;
                case(GL_UNSIGNED_BYTE):
                    if (_findLowerAlphaValueInRow(s(), (unsigned char*)d + offset, (unsigned char)255, delta))
                        return true;
                    break;
                case(GL_SHORT):
                    if (_findLowerAlphaValueInRow(s(), (short*)d + offset, (short)32767, delta))
                        return true;
                    break;
                case(GL_UNSIGNED_SHORT):
                    if (_findLowerAlphaValueInRow(s(), (unsigned short*)d + offset, (unsigned short)65535, delta))
                        return true;
                    break;
                case(GL_INT):
                    if (_findLowerAlphaValueInRow(s(), (int*)d + offset, (int)2147483647, delta))
                        return true;
                    break;
                case(GL_UNSIGNED_INT):
                    if (_findLowerAlphaValueInRow(s(), (unsigned int*)d + offset, 4294967295u, delta))
                        return true;
                    break;
                case(GL_FLOAT):
                    if (_findLowerAlphaValueInRow(s(), (float*)d + offset, 1.0f, delta))
                        return true;
                    break;
                case(GL_UNSIGNED_SHORT_5_5_5_1):
                    if (_maskedFindLowerAlphaValueInRow(s(), (unsigned short*)d,
                                                        (unsigned short)0x0001,
                                                        (unsigned short)0x0001, 1))
                        return true;
                    break;
                case(GL_UNSIGNED_SHORT_1_5_5_5_REV):
                    if (_maskedFindLowerAlphaValueInRow(s(), (unsigned short*)d,
                                                        (unsigned short)0x8000,
                                                        (unsigned short)0x8000, 1))
                        return true;
                    break;
                case(GL_UNSIGNED_SHORT_4_4_4_4):
                    if (_maskedFindLowerAlphaValueInRow(s(), (unsigned short*)d,
                                                        (unsigned short)0x000f,
                                                        (unsigned short)0x000f, 1))
                        return true;
                    break;
                case(GL_UNSIGNED_SHORT_4_4_4_4_REV):
                    if (_maskedFindLowerAlphaValueInRow(s(), (unsigned short*)d,
                                                        (unsigned short)0xf000,
                                                        (unsigned short)0xf000, 1))
                        return true;
                    break;
                case(GL_UNSIGNED_INT_10_10_10_2):
                    if (_maskedFindLowerAlphaValueInRow(s(), (unsigned int*)d,
                                                        0x00000003u,
                                                        0x00000003u, 1))
                        return true;
                    break;
                case(GL_UNSIGNED_INT_2_10_10_10_REV):
                    if (_maskedFindLowerAlphaValueInRow(s(), (unsigned int*)d,
                                                        0xc0000000u,
                                                        0xc0000000u, 1))
                        return true;
                    break;
                case(GL_HALF_FLOAT):
                    if (_findLowerAlphaValueInRow(s(), (unsigned short*)d + offset,
                                                  (unsigned short)0x3c00, delta))
                        return true;
                    break;
            }
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
 

GeometryShape* createGeodeForImage(Image* image,float s,float t)
{
    if (image)
    {
        if (s>0 && t>0)
        {

            float y = 1.0;
            float x = y*(s/t);

            float texcoord_y_b = (image->getOrigin() == Image::BOTTOM_LEFT) ? 0.0f : 1.0f;
            float texcoord_y_t = (image->getOrigin() == Image::BOTTOM_LEFT) ? 1.0f : 0.0f;

            // set up the texture.

 
            Texture2D* texture = new Texture2D;
            texture->setFilter(Texture::MIN_FILTER,Texture::LINEAR);
            texture->setFilter(Texture::MAG_FILTER,Texture::LINEAR);
            texture->setResizeNonPowerOfTwoHint(false);
            float texcoord_x = 1.0f;
 
            texture->setImage(image);

            // set up the drawstate.
 

            // set up the geoset.                unsigned int rowSize = computeRowWidthInBytes(s,_pixelFormat,_dataType,_packing);

            GeometryShape* geom = new GeometryShape;
  
            Vec3Array* coords = new Vec3Array(4);
            (*coords)[0] = Vec3f(-x,0.0f,y);
            (*coords)[1] = Vec3f(-x,0.0f,-y);
            (*coords)[2] = Vec3f(x,0.0f,-y);
            (*coords)[3] = Vec3f(x,0.0f,y);
            geom->setVertexArray(coords);

            Vec2Array* tcoords = new Vec2Array(4);
            (*tcoords)[0] = Vec2f(0.0f*texcoord_x,texcoord_y_t);
            (*tcoords)[1] = Vec2f(0.0f*texcoord_x,texcoord_y_b);
            (*tcoords)[2] = Vec2f(1.0f*texcoord_x,texcoord_y_b);
            (*tcoords)[3] = Vec2f(1.0f*texcoord_x,texcoord_y_t);
            geom->setTexCoordArray(0,tcoords, Array::BIND_PER_VERTEX);

            Vec4Array* colours = new Vec4Array(1);
            (*colours)[0] = Vec4f(1.0f,1.0f,1.0,1.0f);
            geom->setColorArray(colours, Array::BIND_OVERALL);

            geom->addPrimitiveSet(new DrawElementsUShort(DrawElements::QUADS,0,4));
             
            return geom;

        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

template <typename T>
Vec4f _readColor(GLenum pixelFormat, T* data,float scale)
{
    switch(pixelFormat)
    {
        case(GL_DEPTH_COMPONENT):   //intentionally fall through and execute the code for GL_LUMINANCE
        case(GL_LUMINANCE):         { float l = float(*data++)*scale; return Vec4f(l, l, l, 1.0f); }
        case(GL_ALPHA):             { float a = float(*data++)*scale; return Vec4f(1.0f, 1.0f, 1.0f, a); }
        case(GL_RED):               { float r = float(*data++)*scale; return Vec4f(r, 1.0f, 1.0f, 1.0f); }
        case(GL_RG):                { float r = float(*data++)*scale; float g = float(*data++)*scale; return Vec4f(r, g, 1.0f, 1.0f); }
        case(GL_LUMINANCE_ALPHA):   { float l = float(*data++)*scale; float a = float(*data++)*scale; return Vec4f(l,l,l,a); }
        case(GL_RGB):               { float r = float(*data++)*scale; float g = float(*data++)*scale; float b = float(*data++)*scale; return Vec4f(r,g,b,1.0f); }
        case(GL_RGBA):              { float r = float(*data++)*scale; float g = float(*data++)*scale; float b = float(*data++)*scale; float a = float(*data++)*scale; return Vec4f(r,g,b,a); }
        case(GL_BGR):               { float b = float(*data++)*scale; float g = float(*data++)*scale; float r = float(*data++)*scale; return Vec4f(r,g,b,1.0f); }
        case(GL_BGRA):              { float b = float(*data++)*scale; float g = float(*data++)*scale; float r = float(*data++)*scale; float a = float(*data++)*scale; return Vec4f(r,g,b,a); }
    }
    return Vec4f(1.0f,1.0f,1.0f,1.0f);
}

Vec4f Image::getColor(unsigned int s,unsigned t,unsigned r) const
{
    if (isCompressed())
    {
        if (dxtc_tool::isDXTC(_pixelFormat)) {
            unsigned char color[4];
            if (dxtc_tool::CompressedImageGetColor(color, s, t, r, _s, _t, _r, _pixelFormat, _data)) {
                return Vec4f(((float)color[0]) / 255.0f, ((float)color[1]) / 255.0f, ((float)color[2]) / 255.0f, ((float)color[3]) / 255.0f );
            }
        }
    }
    else
    {
        const unsigned char* ptr = data(s,t,r);
        switch(_dataType)
        {
            case(GL_BYTE):              return _readColor(_pixelFormat, (char*)ptr,             1.0f/128.0f);
            case(GL_UNSIGNED_BYTE):     return _readColor(_pixelFormat, (unsigned char*)ptr,    1.0f/255.0f);
            case(GL_SHORT):             return _readColor(_pixelFormat, (short*)ptr,            1.0f/32768.0f);
            case(GL_UNSIGNED_SHORT):    return _readColor(_pixelFormat, (unsigned short*)ptr,   1.0f/65535.0f);
            case(GL_INT):               return _readColor(_pixelFormat, (int*)ptr,              1.0f/2147483648.0f);
            case(GL_UNSIGNED_INT):      return _readColor(_pixelFormat, (unsigned int*)ptr,     1.0f/4294967295.0f);
            case(GL_FLOAT):             return _readColor(_pixelFormat, (float*)ptr,            1.0f);
            case(GL_DOUBLE):            return _readColor(_pixelFormat, (double*)ptr,           1.0f);
        }
    }
    return Vec4f(1.0f,1.0f,1.0f,1.0f);
}

Vec4f Image::getColor(const Vec3f& texcoord) const
{
      int s = int(texcoord.x*float(_s - 1)); b3Clamp(s, 0, _s - 1);
      int t = int(texcoord.y*float(_t - 1)); b3Clamp(t, 0, _t - 1);
      int r = int(texcoord.z*float(_r - 1)); b3Clamp(r, 0, _r - 1);
    ////OSG_NOTICE<<"getColor("<<texcoord<<")="<<getColor(s,t,r)<<std::endl;
    return getColor(s,t,r);
}


template <typename T>
void _writeColor(GLenum pixelFormat, T* data, float scale, const Vec4f& c)
{
    switch(pixelFormat)
    {
    case(GL_DEPTH_COMPONENT):   //intentionally fall through and execute the code for GL_LUMINANCE
    case(GL_LUMINANCE):         { (*data++) = (T)(c[0] * scale); } break;
    case(GL_ALPHA):             { (*data++) = (T)(c[3] * scale); } break;
    case(GL_LUMINANCE_ALPHA):   { (*data++) = (T)(c[0] * scale);  (*data++) = (T)(c[3] * scale); } break;
    case(GL_RGB):               { (*data++) = (T)(c[0] *scale); (*data++) = (T)(c[1] *scale); (*data++) = (T)(c[2] *scale);} break;
    case(GL_RGBA):              { (*data++) = (T)(c[0] *scale); (*data++) = (T)(c[1] *scale); (*data++) = (T)(c[2] *scale); (*data++) = (T)(c[3] *scale);} break;
    case(GL_BGR):               { (*data++) = (T)(c[2] *scale); (*data++) = (T)(c[1] *scale); (*data++) = (T)(c[0] *scale);} break;
    case(GL_BGRA):              { (*data++) = (T)(c[2] *scale); (*data++) = (T)(c[1] *scale); (*data++) = (T)(c[0] *scale); (*data++) = (T)(c[3] *scale);} break;
    }

}


void Image::setColor( const Vec4f& color, unsigned int s, unsigned int t/*=0*/, unsigned int r/*=0*/ )
{
    unsigned char* ptr = data(s,t,r);

    switch(getDataType())
    {
    case(GL_BYTE):              return _writeColor(getPixelFormat(), (char*)ptr,             128.0f, color);
    case(GL_UNSIGNED_BYTE):     return _writeColor(getPixelFormat(), (unsigned char*)ptr,    255.0f, color);
    case(GL_SHORT):             return _writeColor(getPixelFormat(), (short*)ptr,            32768.0f, color);
    case(GL_UNSIGNED_SHORT):    return _writeColor(getPixelFormat(), (unsigned short*)ptr,   65535.0f, color);
    case(GL_INT):               return _writeColor(getPixelFormat(), (int*)ptr,              2147483648.0f, color);
    case(GL_UNSIGNED_INT):      return _writeColor(getPixelFormat(), (unsigned int*)ptr,     4294967295.0f, color);
    case(GL_FLOAT):             return _writeColor(getPixelFormat(), (float*)ptr,            1.0f, color);
    case(GL_DOUBLE):            return _writeColor(getPixelFormat(), (double*)ptr,           1.0f, color);
    }
}

void Image::setColor( const Vec4f& color, const Vec3f& texcoord )
{
    int s = int(texcoord.x*float(_s - 1)); b3Clamp(s, 0, _s - 1);
    int t = int(texcoord.y*float(_t - 1)); b3Clamp(t, 0, _t - 1);
    int r = int(texcoord.z*float(_r - 1)); b3Clamp(r, 0, _r - 1);

    return setColor(color, s,t,r);
}
 