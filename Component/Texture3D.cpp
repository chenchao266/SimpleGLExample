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
 
#include <Texture3D.h>
#include <Image.h>
 

#include <string.h>
 
Texture3D::Texture3D():
            _textureWidth(0),
            _textureHeight(0),
            _textureDepth(0),
            _numMipmapLevels(0)
{
}


Texture3D::Texture3D(Image* image):
            _textureWidth(0),
            _textureHeight(0),
            _textureDepth(0),
            _numMipmapLevels(0)
{
    setImage(image);
}

Texture3D::Texture3D(const Texture3D& text):
            Texture(text),
            _textureWidth(text._textureWidth),
            _textureHeight(text._textureHeight),
            _textureDepth(text._textureDepth),
            _numMipmapLevels(text._numMipmapLevels) 
{
    setImage((text._image.get()));
}

Texture3D::~Texture3D()
{
    setImage(NULL);
}

 
void Texture3D::setImage(Image* image)
{
    if (_image.get() == image) return;
 
    // delete old texture objects.
  
    _image.reset( image);
 
}

void Texture3D::computeRequiredTextureDimensions(State& state, const Image& image,GLsizei& inwidth, GLsizei& inheight,GLsizei& indepth, GLsizei& numMipmapLevels) const
{
    GLint maxTexture3DSize = 0;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &maxTexture3DSize);

    int width,height,depth;

    if( !_resizeNonPowerOfTwoHint )
    {
        width = image.s();
        height = image.t();
        depth = image.r();
    }
    else
    {
        width = Image::computeNearestPowerOfTwo(image.s()-2*_borderWidth)+2*_borderWidth;
        height = Image::computeNearestPowerOfTwo(image.t()-2*_borderWidth)+2*_borderWidth;
        depth = Image::computeNearestPowerOfTwo(image.r()-2*_borderWidth)+2*_borderWidth;
    }

    // cap the size to what the graphics hardware can handle.
    if (width>maxTexture3DSize) width = maxTexture3DSize;
    if (height>maxTexture3DSize) height = maxTexture3DSize;
    if (depth>maxTexture3DSize) depth = maxTexture3DSize;

    inwidth = width;
    inheight = height;
    indepth = depth;

    bool useHardwareMipMapGeneration = !image.isMipmap();// && _useHardwareMipMapGeneration;

    if( _min_filter == LINEAR || _min_filter == NEAREST || useHardwareMipMapGeneration )
    {
        numMipmapLevels = 1;
    }
    else if( image.isMipmap() )
    {
        numMipmapLevels = image.getNumMipmapLevels();
    }
    else
    {
        numMipmapLevels = 0;
        for( ; (width || height || depth) ;++numMipmapLevels)
        {

            if (width == 0)
                width = 1;
            if (height == 0)
                height = 1;
            if (depth == 0)
                depth = 1;

            width >>= 1;
            height >>= 1;
            depth >>= 1;
        }
    }
}

 
void Texture3D::computeInternalFormat() const
{
    if (_image) computeInternalFormatWithImage(*_image);
    else computeInternalFormatType();
}

void Texture3D::applyTexImage3D(GLenum target, Image* image, State& state, GLsizei& inwidth, GLsizei& inheight, GLsizei& indepth, GLsizei& numMipmapLevels) const
{
    // if we don't have a valid image we can't create a texture!
    if (!image || !image->data())
        return;

    // get the contextID (user defined ID of 0 upwards) for the
    // current OpenGL context.
    const unsigned int contextID = 0;
 
    GLint maxTexture3DSize = 0;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &maxTexture3DSize);

    // compute the internal texture format, this set the _internalFormat to an appropriate value.
    computeInternalFormat();

    // select the internalFormat required for the texture.
    bool compressed = isCompressedInternalFormat(_internalFormat);
    bool compressed_image = isCompressedInternalFormat((GLenum)image->getPixelFormat());

    if (compressed)
    {
        //OSG_WARN<<"Warning::cannot currently use compressed format with 3D textures."<<std::endl;
        //return;
    }

    //Rescale if resize hint is set or NPOT not supported or dimensions exceed max size
    if( _resizeNonPowerOfTwoHint || false
        || inwidth > maxTexture3DSize
        || inheight > maxTexture3DSize
        || indepth > maxTexture3DSize )
        image->ensureValidSizeForTexturing(maxTexture3DSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT,image->getPacking());
#ifndef BT_USE_EGL
    glPixelStorei(GL_UNPACK_ROW_LENGTH,image->getRowLength());
#endif

    bool useHardwareMipMapGeneration = !image->isMipmap();// && _useHardwareMipMapGeneration;

    if( _min_filter == LINEAR || _min_filter == NEAREST || useHardwareMipMapGeneration )
    {
        bool hardwareMipMapOn = false;
        if (_min_filter != LINEAR && _min_filter != NEAREST)
        {
            if (useHardwareMipMapGeneration) glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP,GL_TRUE);
            hardwareMipMapOn = true;
        }

        numMipmapLevels = 1;

        if (!compressed_image)
        {
            glTexImage3D( target, 0, _internalFormat,
                                      inwidth, inheight, indepth,
                                      _borderWidth,
                                      (GLenum)image->getPixelFormat(),
                                      (GLenum)image->getDataType(),
                                      image->data() );
        }
        else if (1)
        {
            // OSG_WARN<<"glCompressedTexImage3D "<<inwidth<<", "<<inheight<<", "<<indepth<<std::endl;
            numMipmapLevels = 1;

            GLint blockSize, size;
            getCompressedSize(_internalFormat, inwidth, inheight, indepth, blockSize,size);

            glCompressedTexImage3D(target, 0, _internalFormat,
                inwidth, inheight, indepth,
                _borderWidth,
                size,
                image->data());
        }

        if (hardwareMipMapOn) glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP,GL_FALSE);
    }
    else
    { 

    }

    inwidth  = image->s();
    inheight = image->t();
    indepth  = image->r();

}

void Texture3D::copyTexSubImage3D(State& state, int xoffset, int yoffset, int zoffset, int x, int y, int width, int height )
{
    const unsigned int contextID = 0;
 
    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject != 0)
    {
        textureObject->bind();

        applyTexParameters(GL_TEXTURE_3D,state);
        glCopyTexSubImage3D( GL_TEXTURE_3D, 0, xoffset,yoffset,zoffset, x, y, width, height);
          
 
    }
    else
    {
        //OSG_WARN<<"Warning: Texture3D::copyTexSubImage3D(..) failed, cannot not copy to a non existent texture."<<std::endl;
    }
}

void Texture3D::allocateMipmap(State& state) const
{
    const unsigned int contextID = 0;

    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject && _textureWidth != 0 && _textureHeight != 0 && _textureDepth != 0)
    {
 
        // bind texture
        textureObject->bind();

        // compute number of mipmap levels
        int width = _textureWidth;
        int height = _textureHeight;
        int depth = _textureDepth;
        int numMipmapLevels = Image::computeNumberOfMipmapLevels(width, height, depth);

        // we do not reallocate the level 0, since it was already allocated
        width >>= 1;
        height >>= 1;
        depth >>= 1;

        for( GLsizei k = 1; k < numMipmapLevels  && (width || height || depth); k++)
        {
            if (width == 0)
                width = 1;
            if (height == 0)
                height = 1;
            if (depth == 0)
                depth = 1;

            glTexImage3D( GL_TEXTURE_3D, k, _internalFormat,
                     width, height, depth, _borderWidth,
                     _sourceFormat ? _sourceFormat : _internalFormat,
                     _sourceType ? _sourceType : GL_UNSIGNED_BYTE, NULL);

            width >>= 1;
            height >>= 1;
            depth >>= 1;
        }

  
    }
}
