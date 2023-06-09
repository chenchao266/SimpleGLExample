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

#ifndef OSG_TEXTURE2D
#define OSG_TEXTURE2D 1

#include <Component/Texture.h>


/** Encapsulates OpenGL 2D texture functionality. Doesn't support cube maps,
  * so ignore \a face parameters.
*/
class  Texture2D : public Texture
{

    public :

        Texture2D();

        Texture2D(Image* image);

        template<class T> Texture2D(const std::shared_ptr<T>& image):
            _textureWidth(0),
            _textureHeight(0),
            _numMipmapLevels(0)
        {
            setUseHardwareMipMapGeneration(true);
            setImage(image.get());
        }

        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        Texture2D(const Texture2D& text);
         
        virtual GLenum getTextureTarget() const { return GL_TEXTURE_2D; }

        /** Sets the texture image. */
        void setImage(Image* image);

        template<class T> void setImage(const std::shared_ptr<T>& image) { setImage(image.get()); }

        /** Gets the texture image. */
        Image* getImage() { return _image.get(); }

        /** Gets the const texture image. */
        inline const Image* getImage() const { return _image.get(); }

        /** return true if the texture image data has been modified and the associated GL texture object needs to be updated.*/
        virtual bool isDirty(unsigned int contextID) const { return false; }
         
        /** Sets the texture image, ignoring face. */
        virtual void setImage(unsigned int, Image* image) { setImage(image); }

        template<class T> void setImage(unsigned int, const std::shared_ptr<T>& image) { setImage(image.get()); }

        /** Gets the texture image, ignoring face. */
        virtual Image* getImage(unsigned int) { return _image.get(); }

        /** Gets the const texture image, ignoring face. */
        virtual const Image* getImage(unsigned int) const { return _image.get(); }

        /** Gets the number of images that can be assigned to the Texture. */
        virtual unsigned int getNumImages() const { return 1; }


        /** Sets the texture width and height. If width or height are zero,
          * calculate the respective value from the source image size. */
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
 
        /** Helper function. Sets the number of mipmap levels created for this
          * texture. Should only be called within an Texture::apply(), or
          * during a custom OpenGL texture load. */
        void setNumMipmapLevels(unsigned int num) const { _numMipmapLevels=num; }

        /** Gets the number of mipmap levels created. */
        unsigned int getNumMipmapLevels() const { return _numMipmapLevels; }


        /** Copies pixels into a 2D texture image, as per glCopyTexImage2D.
          * Creates an OpenGL texture object from the current OpenGL background
          * framebuffer contents at position \a x, \a y with width \a width and
          * height \a height. \a width and \a height must be a power of two. */
        void copyTexImage2D(State& state, int x, int y, int width, int height );

        /** Copies a two-dimensional texture subimage, as per
          * glCopyTexSubImage2D. Updates a portion of an existing OpenGL
          * texture object from the current OpenGL background framebuffer
          * contents at position \a x, \a y with width \a width and height
          * \a height. Loads framebuffer data into the texture using offsets
          * \a xoffset and \a yoffset. \a width and \a height must be powers
          * of two. */
        void copyTexSubImage2D(State& state, int xoffset, int yoffset, int x, int y, int width, int height );
         
        virtual ~Texture2D();


    protected :

        virtual void computeInternalFormat() const;
        void allocateMipmap(State& state) const;

        /** Return true of the TextureObject assigned to the context associate with State object is valid.*/
        bool textureObjectValid(State& state) const;
 
        std::shared_ptr<Image> _image;

        /** Subloaded images can have different texture and image sizes. */
        mutable GLsizei _textureWidth, _textureHeight;

        /** Number of mipmap levels created. */
        mutable GLsizei _numMipmapLevels;
  

};



#endif
