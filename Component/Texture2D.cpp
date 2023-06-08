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

 
#include <Texture2D.h>
 
 

Texture2D::Texture2D():
            _textureWidth(0),
            _textureHeight(0),
            _numMipmapLevels(0)
{
    //setUseHardwareMipMapGeneration(true);
}

Texture2D::Texture2D(Image* image):
            _textureWidth(0),
            _textureHeight(0),
            _numMipmapLevels(0)
{
    //setUseHardwareMipMapGeneration(true);
    setImage(image);
}

Texture2D::Texture2D(const Texture2D& text):
            Texture(text),
            _textureWidth(text._textureWidth),
            _textureHeight(text._textureHeight),
            _numMipmapLevels(text._numMipmapLevels) 
{
    setImage((text._image.get()));
}

Texture2D::~Texture2D()
{
    setImage(NULL);
}

 
void Texture2D::setImage(Image* image)
{
    if (_image.get() == image) return;
 
    _image.reset(image);
 
}

bool Texture2D::textureObjectValid(State& state) const
{
    TextureObject* textureObject = getTextureObject(0);
    if (!textureObject) return false;

    // return true if image isn't assigned as we won't be override the value.
    if (!_image) return true;

    // compute the internal texture format, this set the _internalFormat to an appropriate value.
    computeInternalFormat();

    GLsizei new_width, new_height, new_numMipmapLevels;

    // compute the dimensions of the texture.
    computeRequiredTextureDimensions(state, *_image, new_width, new_height, new_numMipmapLevels);

    return textureObject->match(GL_TEXTURE_2D, new_numMipmapLevels, _internalFormat, new_width, new_height, 1, _borderWidth);
}


 
void Texture2D::computeInternalFormat() const
{
    if (_image) computeInternalFormatWithImage(*_image);
    else computeInternalFormatType();
}

void Texture2D::copyTexImage2D(State& state, int x, int y, int width, int height )
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

    bool needHardwareMipMap = (_min_filter != LINEAR && _min_filter != NEAREST);
 
    if (needHardwareMipMap)
    { 
        if (!true)
        {
            // have to switch off mip mapping
            //OSG_NOTICE<<"Warning: Texture2D::copyTexImage2D(,,,,) switch off mip mapping as hardware support not available."<<std::endl;
            _min_filter = LINEAR;
        }
    }

    _textureWidth = width;
    _textureHeight = height;

    _numMipmapLevels = 1;
    if (needHardwareMipMap)
    {
        for(int s=1; s<width || s<height; s <<= 1, ++_numMipmapLevels) {}
    }

    textureObject = generateAndAssignTextureObject(contextID,GL_TEXTURE_2D,_numMipmapLevels,_internalFormat,_textureWidth,_textureHeight,1,0);

    textureObject->bind();

    applyTexParameters(GL_TEXTURE_2D,state);


    GenerateMipmapMode mipmapResult = mipmapBeforeTexImage(state);

    glCopyTexImage2D( GL_TEXTURE_2D, 0, _internalFormat, x, y, width, height, 0 );

    mipmapAfterTexImage(state, mipmapResult);

    textureObject->setAllocated(true);

    // inform state that this texture is the current one bound.
     
}

void Texture2D::copyTexSubImage2D(State& state, int xoffset, int yoffset, int x, int y, int width, int height )
{
    const unsigned int contextID = 0;

    if (_internalFormat==0) _internalFormat=GL_RGBA;

    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject)
    {
        // we have a valid image
        textureObject->bind();

        applyTexParameters(GL_TEXTURE_2D,state);

        bool needHardwareMipMap = (_min_filter != LINEAR && _min_filter != NEAREST);
   
        if (needHardwareMipMap)
        { 
            if (!true)
            {
                // have to switch off mip mapping
                //OSG_NOTICE<<"Warning: Texture2D::copyTexImage2D(,,,,) switch off mip mapping as hardware support not available."<<std::endl;
                _min_filter = LINEAR;
            }
        }

        GenerateMipmapMode mipmapResult = mipmapBeforeTexImage(state);

        glCopyTexSubImage2D( GL_TEXTURE_2D, 0, xoffset, yoffset, x, y, width, height);

        mipmapAfterTexImage(state, mipmapResult);

        // inform state that this texture is the current one bound.
       

    }
    else
    {
        // no texture object already exsits for this context so need to
        // create it upfront - simply call copyTexImage2D.
        copyTexImage2D(state,x,y,width,height);
    }
}

void Texture2D::allocateMipmap(State& state) const
{
    const unsigned int contextID = 0;

    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (textureObject && _textureWidth != 0 && _textureHeight != 0)
    {
        // bind texture
        textureObject->bind();

        // compute number of mipmap levels
        int width = _textureWidth;
        int height = _textureHeight;
        int numMipmapLevels = Image::computeNumberOfMipmapLevels(width, height);

        // we do not reallocate the level 0, since it was already allocated
        width >>= 1;
        height >>= 1;

        for( GLsizei k = 1; k < numMipmapLevels  && (width || height); k++)
        {
            if (width == 0)
                width = 1;
            if (height == 0)
                height = 1;

            glTexImage2D( GL_TEXTURE_2D, k, _internalFormat,
                     width, height, _borderWidth,
                     _sourceFormat ? _sourceFormat : _internalFormat,
                     _sourceType ? _sourceType : GL_UNSIGNED_BYTE, NULL);

            width >>= 1;
            height >>= 1;
        }

        // inform state that this texture is the current one bound.
       
    }
}

