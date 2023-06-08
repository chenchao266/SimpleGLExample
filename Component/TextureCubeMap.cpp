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
 
 
#include <Image.h>
#include <TextureCubeMap.h>
 
static GLenum faceTarget[6] =
{
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};


TextureCubeMap::TextureCubeMap():
            _textureWidth(0),
            _textureHeight(0),
            _numMipmapLevels(0)
{
    //setUseHardwareMipMapGeneration(false);
}

TextureCubeMap::TextureCubeMap(const TextureCubeMap& text):
            Texture(text),
            _textureWidth(text._textureWidth),
            _textureHeight(text._textureHeight),
            _numMipmapLevels(text._numMipmapLevels) 
{
    setImage(0, (text._images[0].get()));
    setImage(1, (text._images[1].get()));
    setImage(2, (text._images[2].get()));
    setImage(3, (text._images[3].get()));
    setImage(4, (text._images[4].get()));
    setImage(5, (text._images[5].get()));
}

TextureCubeMap::~TextureCubeMap()
{
    setImage(0, NULL);
    setImage(1, NULL);
    setImage(2, NULL);
    setImage(3, NULL);
    setImage(4, NULL);
    setImage(5, NULL);
}

 
void TextureCubeMap::setImage(unsigned int face, Image* image)
{
    if (_images[face].get() == image) return;
  
    _images[face].reset(image);
   
}

Image* TextureCubeMap::getImage(unsigned int face)
{
    return _images[face].get();
}

const Image* TextureCubeMap::getImage(unsigned int face) const
{
    return _images[face].get();
}

bool TextureCubeMap::imagesValid() const
{
    for (int n=0; n<6; n++)
    {
        if (!_images[n] || !_images[n]->data())
            return false;
    }
    return true;
}

void TextureCubeMap::computeInternalFormat() const
{
    if (imagesValid()) computeInternalFormatWithImage(*_images[0]);
    else computeInternalFormatType();
}
 
void TextureCubeMap::copyTexSubImageCubeMap(State& state, int face, int xoffset, int yoffset, int x, int y, int width, int height )
{
    const unsigned int contextID = 0;
 

    if (_internalFormat==0) _internalFormat=GL_RGBA;

    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    if (!textureObject)
    {

        if (_textureWidth==0) _textureWidth = width;
        if (_textureHeight==0) _textureHeight = height;

        // create texture object.
        //apply(state);

        textureObject = getTextureObject(contextID);

        if (!textureObject)
        {
            // failed to create texture object
            //OSG_NOTICE<<"Warning : failed to create TextureCubeMap texture obeject, copyTexSubImageCubeMap abandoned."<<std::endl;
            return;
        }

    }

    GLenum target = faceTarget[face];

    if (textureObject)
    {
        // we have a valid image
        textureObject->bind();

        applyTexParameters(GL_TEXTURE_CUBE_MAP, state);

        bool needHardwareMipMap = (_min_filter != LINEAR && _min_filter != NEAREST);
       
        if (needHardwareMipMap)
        { 
            if (!true)
            {
                // have to switch off mip mapping
                //OSG_NOTICE<<"Warning: TextureCubeMap::copyTexImage2D(,,,,) switch off mip mapping as hardware support not available."<<std::endl;
                _min_filter = LINEAR;
            }
        }

        GenerateMipmapMode mipmapResult = mipmapBeforeTexImage(state);

        glCopyTexSubImage2D( target , 0, xoffset, yoffset, x, y, width, height);

        mipmapAfterTexImage(state, mipmapResult);

        // inform state that this texture is the current one bound.
   

    }
}

void TextureCubeMap::allocateMipmap(State& state) const
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

            for (int n=0; n<6; n++)
            {
                glTexImage2D( faceTarget[n], k, _internalFormat,
                            width, height, _borderWidth,
                            _sourceFormat ? _sourceFormat : _internalFormat,
                            _sourceType ? _sourceType : GL_UNSIGNED_BYTE,
                            0);
            }

            width >>= 1;
            height >>= 1;
        }

        // inform state that this texture is the current one bound.
    
    }
}
