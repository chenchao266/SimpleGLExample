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

 
#include <TextureRectangle.h>
 
 
 
 

TextureRectangle::TextureRectangle():
    _textureWidth(0),
    _textureHeight(0)
{
    setWrap(WRAP_S, CLAMP);
    setWrap(WRAP_T, CLAMP);

    setFilter(MIN_FILTER, LINEAR);
    setFilter(MAG_FILTER, LINEAR);
}

TextureRectangle::TextureRectangle(Image* image):
            _textureWidth(0),
            _textureHeight(0)
{
    setWrap(WRAP_S, CLAMP);
    setWrap(WRAP_T, CLAMP);

    setFilter(MIN_FILTER, LINEAR);
    setFilter(MAG_FILTER, LINEAR);

    setImage(image);
}

TextureRectangle::TextureRectangle(const TextureRectangle& text):
    Texture(text),
    _textureWidth(text._textureWidth),
    _textureHeight(text._textureHeight) 
{
    setImage((text._image.get()));
}

TextureRectangle::~TextureRectangle()
{
    setImage(NULL);
}

 
void TextureRectangle::setImage(Image* image)
{
    if (_image.get() == image) return;
     
 
    _image.reset(image);

 
}

 
void TextureRectangle::applyTexImage_load(GLenum target, Image* image, State& state, GLsizei& inwidth, GLsizei& inheight) const
{
    // if we don't have a valid image we can't create a texture!
    if (!image || !image->data())
        return;

    // get the contextID (user defined ID of 0 upwards) for the
    // current OpenGL context.
    const unsigned int contextID = 0;
  
    // compute the internal texture format, sets _internalFormat.
    computeInternalFormat();

    glPixelStorei(GL_UNPACK_ALIGNMENT, image->getPacking());
#ifndef BT_USE_EGL
    glPixelStorei(GL_UNPACK_ROW_LENGTH,image->getRowLength());
#endif
    bool   isClientStorageSupported = false;// "GL_APPLE_client_storage";
    bool useClientStorage = isClientStorageSupported && getClientStorageHint();
    if (useClientStorage)
    {
 
    }

    const unsigned char* dataPtr = image->data();
    GLuint pbo = 1;// image->getOrCreateGLBufferObject(contextID);
    glGenBuffers(1, &pbo);
    if (pbo)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
        ///dataPtr = reinterpret_cast<unsigned char*>(pbo->getOffset(image->getBufferIndex()));
    }

    if(isCompressedInternalFormat(_internalFormat) )
    {
            glCompressedTexImage2D(target, 0, _internalFormat,
          image->s(), image->t(), 0,
          image->getImageSizeInBytes(),
          dataPtr);
    }
    else
    {
        glTexImage2D(target, 0, _internalFormat,
          image->s(), image->t(), 0,
          (GLenum)image->getPixelFormat(),
          (GLenum)image->getDataType(),
          dataPtr );
    }


    if (pbo)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

    inwidth = image->s();
    inheight = image->t();

    if (useClientStorage)
    {
 
    }
}

void TextureRectangle::applyTexImage_subload(GLenum target, Image* image, State& state, GLsizei& inwidth, GLsizei& inheight, GLint& inInternalFormat) const
{
    // if we don't have a valid image we can't create a texture!
    if (!image || !image->data())
        return;

    if (image->s()!=inwidth || image->t()!=inheight || image->getInternalTextureFormat()!=inInternalFormat)
    {
        applyTexImage_load(target, image, state, inwidth, inheight);
        return;
    }


    // get the contextID (user defined ID of 0 upwards) for the
    // current OpenGL context.
    const unsigned int contextID = 0;
  
    // compute the internal texture format, sets _internalFormat.
    computeInternalFormat();

    glPixelStorei(GL_UNPACK_ALIGNMENT, image->getPacking());
    unsigned int rowLength = image->getRowLength();

 
    const unsigned char* dataPtr = image->data();
    GLuint pbo = 1;//image->getOrCreateGLBufferObject(contextID);
    glGenBuffers(1, &pbo);
    if (pbo)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
        ///dataPtr = reinterpret_cast<unsigned char*>(pbo->getOffset(image->getBufferIndex()));
        rowLength = 0;
 
    }

#ifndef BT_USE_EGL
    glPixelStorei(GL_UNPACK_ROW_LENGTH,rowLength);
#endif

    if(isCompressedInternalFormat(_internalFormat))
    {
        glCompressedTexSubImage2D(target, 0,
          0,0,
          image->s(), image->t(),
          (GLenum)image->getPixelFormat(),
          (GLenum)image->getDataType(),
          dataPtr);
    }
    else
    {
        glTexSubImage2D(target, 0,
          0,0,
          image->s(), image->t(),
          (GLenum)image->getPixelFormat(),
          (GLenum)image->getDataType(),
          dataPtr);
    }

    if (pbo)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

 
}

void TextureRectangle::computeInternalFormat() const
{
    if (_image) computeInternalFormatWithImage(*_image);
    else computeInternalFormatType();
}

void TextureRectangle::copyTexImage2D(State& state, int x, int y, int width, int height )
{
    const unsigned int contextID = 0;

    if (_internalFormat==0) _internalFormat=GL_RGBA;

    // get the globj for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject)
    {
        if (width==(int)_textureWidth && height==(int)_textureHeight)
        {
            // we have a valid texture object which is the right size
            // so lets play clever and use copyTexSubImage2D instead.
            // this allows use to reuse the texture object and avoid
            // expensive memory allocations.
            copyTexSubImage2D(state,0 ,0, x, y, width, height);
            return;
        }
        // the relevant texture object is not of the right size so
        // needs to been deleted
        // remove previously bound textures.
 
        // note, dirtyTextureObject() dirties all the texture objects for
        // this texture, is this right?  Perhaps we should dirty just the
        // one for this context.  Note sure yet will leave till later.
        // RO July 2001.
    }


    // remove any previously assigned images as these are nolonger valid.
    _image = NULL;

    // switch off mip-mapping.
    //
    textureObject = generateAndAssignTextureObject(contextID,GL_TEXTURE_RECTANGLE);

    textureObject->bind();

    applyTexParameters(GL_TEXTURE_RECTANGLE,state);
     
    glCopyTexImage2D( GL_TEXTURE_RECTANGLE, 0, _internalFormat, x, y, width, height, 0 );
     
    _textureWidth = width;
    _textureHeight = height;
//    _numMipmapLevels = 1;

    textureObject->setAllocated(1,_internalFormat,_textureWidth,_textureHeight,1,0);


    // inform state that this texture is the current one bound.
 
}

void TextureRectangle::copyTexSubImage2D(State& state, int xoffset, int yoffset, int x, int y, int width, int height )
{
    const unsigned int contextID = 0;

    if (_internalFormat==0) _internalFormat=GL_RGBA;

    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject)
    {
        // we have a valid image
        textureObject->bind();

        applyTexParameters(GL_TEXTURE_RECTANGLE,state);
         
        glCopyTexSubImage2D( GL_TEXTURE_RECTANGLE, 0, xoffset, yoffset, x, y, width, height);
         
        // inform state that this texture is the current one bound.
  

    }
    else
    {
        // no texture object already exsits for this context so need to
        // create it upfront - simply call copyTexImage2D.
        copyTexImage2D(state,x,y,width,height);
    }
}

void TextureRectangle::allocateMipmap(State&) const
{
    //OSG_NOTICE<<"Warning: TextureRectangle::allocateMipmap(State&) called eroneously, GL_TEXTURE_RECTANGLE does not support mipmapping."<<std::endl;
}

