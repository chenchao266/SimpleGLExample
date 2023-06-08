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

#ifndef OSG_TEXTURECUBEMAP
#define OSG_TEXTURECUBEMAP 1

#include <Component/Texture.h>



/** TextureCubeMap state class which encapsulates OpenGL texture cubemap functionality. */
class  TextureCubeMap : public Texture
{

    public :

        TextureCubeMap();

        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        TextureCubeMap(const TextureCubeMap& cm);
         
        virtual GLenum getTextureTarget() const { return GL_TEXTURE_CUBE_MAP; }

        enum Face {
            POSITIVE_X=0,
            NEGATIVE_X=1,
            POSITIVE_Y=2,
            NEGATIVE_Y=3,
            POSITIVE_Z=4,
            NEGATIVE_Z=5
        };

        /** Set the texture image for specified face. */
        virtual void setImage(unsigned int face, Image* image);

        template<class T> void setImage(unsigned int face, const std::shared_ptr<T>& image) { setImage(face, image.get()); }

        /** Get the texture image for specified face. */
        virtual Image* getImage(unsigned int face);

        /** Get the const texture image for specified face. */
        virtual const Image* getImage(unsigned int face) const;

        /** Get the number of images that can be assigned to the Texture. */
        virtual unsigned int getNumImages() const { return 6; }


        /** return true if the texture image data has been modified and the associated GL texture object needs to be updated.*/
        virtual bool isDirty(unsigned int contextID) const
        {
            return  false;
        }

  
        /** Set the texture width and height. If width or height are zero then
          * the respective size value is calculated from the source image sizes.
        */
        inline void setTextureSize(int width, int height) const
        {
            _textureWidth = width;
            _textureHeight = height;
        }

        void setTextureWidth(int width) { _textureWidth=width; }
        void setTextureHeight(int height) { _textureHeight=height; }

        virtual int getTextureWidth() const { return _textureWidth; }
        virtual int getTextureHeight() const { return _textureHeight; }
        virtual int getTextureDepth() const { return 1; }
 
        /** Set the number of mip map levels the texture has been created with.
          * Should only be called within an Texuture::apply() and custom OpenGL texture load.
        */
        void setNumMipmapLevels(unsigned int num) const { _numMipmapLevels=num; }

        /** Get the number of mip map levels the texture has been created with. */
        unsigned int getNumMipmapLevels() const { return _numMipmapLevels; }

        /** Copies a two-dimensional texture subimage, as per
          * glCopyTexSubImage2D. Updates a portion of an existing OpenGL
          * texture object from the current OpenGL background framebuffer
          * contents at position \a x, \a y with width \a width and height
          * \a height. Loads framebuffer data into the texture using offsets
          * \a xoffset and \a yoffset. \a width and \a height must be powers
          * of two. */
        void copyTexSubImageCubeMap(State& state, int face, int xoffset, int yoffset, int x, int y, int width, int height );
         

        virtual ~TextureCubeMap();
protected:

        bool imagesValid() const;

        virtual void computeInternalFormat() const;
        void allocateMipmap(State& state) const;

        std::shared_ptr<Image> _images[6];

        // subloaded images can have different texture and image sizes.
        mutable GLsizei _textureWidth, _textureHeight;

        // number of mip map levels the texture has been created with,
        mutable GLsizei _numMipmapLevels;
  
};



#endif
