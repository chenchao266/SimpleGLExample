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

#ifndef OSG_TEXTURE
#define OSG_TEXTURE 1

#include <OpenGLWindow/OpenGLInclude.h>
#include <Bullet3Common/geometry.h>
#include <Component/Image.h>


#include <list>
#include <map>



// forward declare
class TextureObjectSet;
class TextureObjectManager;


/** Texture pure virtual base class that encapsulates OpenGL texture
  * functionality common to the various types of OSG textures.
*/
class   Texture //: public StateAttribute
{

    public :

        Texture();

        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        Texture(const Texture& text);
  
        virtual GLenum getTextureTarget() const = 0;
 

        virtual int getTextureWidth() const { return 0; }
        virtual int getTextureHeight() const { return 0; }
        virtual int getTextureDepth() const { return 0; }

        enum WrapParameter {
            WRAP_S,
            WRAP_T,
            WRAP_R
        };

        enum WrapMode {
            CLAMP  = GL_CLAMP,
            CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
            CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
            REPEAT = GL_REPEAT,
            MIRROR = GL_MIRRORED_REPEAT
        };

        /** Sets the texture wrap mode. */
        void setWrap(WrapParameter which, WrapMode wrap);
        /** Gets the texture wrap mode. */
        WrapMode getWrap(WrapParameter which) const;


        /** Sets the border color. Only used when wrap mode is CLAMP_TO_BORDER.
         * The border color will be casted to the appropriate type to match the
         * internal pixel format of the texture. */
        void setBorderColor(const Vec4d& color) { _borderColor = color;   }

        /** Gets the border color. */
        const Vec4d& getBorderColor() const { return _borderColor; }

        /** Sets the border width. */
        void setBorderWidth(GLint width) { _borderWidth = width;   }

        GLint getBorderWidth() const { return _borderWidth; }

        enum FilterParameter {
            MIN_FILTER,
            MAG_FILTER
        };

        enum FilterMode {
            LINEAR                    = GL_LINEAR,
            LINEAR_MIPMAP_LINEAR      = GL_LINEAR_MIPMAP_LINEAR,
            LINEAR_MIPMAP_NEAREST     = GL_LINEAR_MIPMAP_NEAREST,
            NEAREST                   = GL_NEAREST,
            NEAREST_MIPMAP_LINEAR     = GL_NEAREST_MIPMAP_LINEAR,
            NEAREST_MIPMAP_NEAREST    = GL_NEAREST_MIPMAP_NEAREST
        };

        /** Sets the texture filter mode. */
        void setFilter(FilterParameter which, FilterMode filter);

        /** Gets the texture filter mode. */
        FilterMode getFilter(FilterParameter which) const;

        /** Sets the maximum anisotropy value, default value is 1.0 for no
          * anisotropic filtering. If hardware does not support anisotropic
          * filtering, use normal filtering (equivalent to a max anisotropy
          * value of 1.0. Valid range is 1.0f upwards.  The maximum value
          * depends on the graphics system. */
        void setMaxAnisotropy(float anis);

        /** Gets the maximum anisotropy value. */
        inline float getMaxAnisotropy() const { return _maxAnisotropy; }

        /** Sets the minimum level of detail value. */
        void setMinLOD(float minlod);

        /** Gets the minimum level of detail value. */
        inline float getMinLOD() const { return _minlod; }

        /** Sets the maximum level of detail value. */
        void setMaxLOD(float maxlod);

        /** Gets the maximum level of detail value. */
        inline float getMaxLOD() const { return _maxlod; }

        /** Gets the level of detail bias value. */
        void setLODBias(float lodbias);

        /** Sets the level of detail bias value. */
        inline float getLODBias() const { return _lodbias; }

        /** Configure the source of texture swizzling for all channels */
        inline void setSwizzle(const Vec4i& swizzle) { _swizzle = swizzle;  };

        /** Gets the source of texture swizzling for all channels */
        inline const Vec4i& getSwizzle() const { return _swizzle; }

        /** Sets the hardware mipmap generation hint. If enabled, it will
          * only be used if supported in the graphics system. */
        //inline void setUseHardwareMipMapGeneration(bool useHardwareMipMapGeneration) { _useHardwareMipMapGeneration = useHardwareMipMapGeneration; }

        /** Gets the hardware mipmap generation hint. */
        //inline bool getUseHardwareMipMapGeneration() const { return _useHardwareMipMapGeneration; }

        /** Sets whether or not the apply() function will unreference the image
          * data. If enabled, and the image data is only referenced by this
          * Texture, apply() will delete the image data. */
        inline void setUnRefImageDataAfterApply(bool flag) { _unrefImageDataAfterApply = flag; }

        /** Gets whether or not apply() unreferences image data. */
        inline bool getUnRefImageDataAfterApply() const { return _unrefImageDataAfterApply; }

        /** Sets whether to use client storage for the texture, if supported
          * by the graphics system. Note: If enabled, and the graphics system
          * supports it, the Image(s) associated with this texture cannot
          * be deleted, so the UnRefImageDataAfterApply flag would be ignored. */
        inline void setClientStorageHint(bool flag) { _clientStorageHint = flag; }

        /** Gets whether to use client storage for the texture. */
        inline bool getClientStorageHint() const { return _clientStorageHint; }

        /** Sets whether to force the texture to resize images that have dimensions
          * that are not a power of two. If enabled, NPOT images will be resized,
          * whether or not NPOT textures are supported by the hardware. If disabled,
          * NPOT images will not be resized if supported by hardware. */
        inline void setResizeNonPowerOfTwoHint(bool flag) { _resizeNonPowerOfTwoHint = flag; }

        /** Gets whether texture will force non power to two images to be resized. */
        inline bool getResizeNonPowerOfTwoHint() const { return _resizeNonPowerOfTwoHint; }

        enum InternalFormatMode {
            USE_IMAGE_DATA_FORMAT,
            USE_USER_DEFINED_FORMAT,
            USE_ARB_COMPRESSION,
            USE_S3TC_DXT1_COMPRESSION,
            USE_S3TC_DXT3_COMPRESSION,
            USE_S3TC_DXT5_COMPRESSION,
            USE_PVRTC_2BPP_COMPRESSION,
            USE_PVRTC_4BPP_COMPRESSION,
            USE_ETC_COMPRESSION,
            USE_ETC2_COMPRESSION,
            USE_RGTC1_COMPRESSION,
            USE_RGTC2_COMPRESSION,
            USE_S3TC_DXT1c_COMPRESSION,
            USE_S3TC_DXT1a_COMPRESSION
        };

        /** Sets the internal texture format mode. Note: If the texture format is
          * USE_IMAGE_DATA_FORMAT, USE_ARB_COMPRESSION, or USE_S3TC_COMPRESSION,
          * the internal format mode is set automatically and will overwrite the
          * previous _internalFormat. */
        inline void setInternalFormatMode(InternalFormatMode mode) { _internalFormatMode = mode; }

        /** Gets the internal texture format mode. */
        inline InternalFormatMode getInternalFormatMode() const { return _internalFormatMode; }

        /** Sets the internal texture format. Implicitly sets the
          * internalFormatMode to USE_USER_DEFINED_FORMAT.
          * The corresponding internal format type will be computed. */
        inline void setInternalFormat(GLint internalFormat)
        {
            _internalFormatMode = USE_USER_DEFINED_FORMAT;
            _internalFormat = internalFormat;
            computeInternalFormatType();
        }


        /** Gets the internal texture format. */
        inline GLint getInternalFormat() const { if (_internalFormat==0) computeInternalFormat(); return _internalFormat; }

        /** Return true if the internal format is one of the compressed formats.*/
        bool isCompressedInternalFormat() const;

        /** Sets the external source image format, used as a fallback when no Image is attached to provide the source image format. */
        inline void setSourceFormat(GLenum sourceFormat) { _sourceFormat = sourceFormat; }

        /** Gets the external source image format. */
        inline GLenum getSourceFormat() const { return _sourceFormat; }

        /** Sets the external source data type, used as a fallback when no Image is attached to provide the source image format.*/
        inline void setSourceType(GLenum sourceType) { _sourceType = sourceType; }

        /** Gets the external source data type.*/
        inline GLenum getSourceType() const { return _sourceType; }

        /** Texture type determined by the internal texture format */
        enum InternalFormatType{

            //! default OpenGL format (clamped values to [0,1) or [0,255])
            NORMALIZED = 0x0,

            //! float values, Shader Model 3.0 (see ARB_texture_float)
            FLOAT = 0x1,

            //! Signed integer values (see EXT_texture_integer)
            SIGNED_INTEGER = 0x2,

            //! Unsigned integer value (see EXT_texture_integer)
            UNSIGNED_INTEGER = 0x4
        };

        /** Get the internal texture format type. */
        inline InternalFormatType getInternalFormatType() const { return _internalFormatType; }

        /* select the size internal format to use based on Image when available or Texture format settings.*/
        GLenum selectSizedInternalFormat(const Image* image=0) const;

        class TextureObject;

        /** return true if the texture image data has been modified and the associated GL texture object needs to be updated.*/
        virtual bool isDirty(unsigned int /*contextID*/) const { return false; }


        /** Returns a pointer to the TextureObject for the current context. */
        inline TextureObject* getTextureObject(unsigned int contextID) const
        {
            return _textureObjectBuffer.get();
        }

        inline void setTextureObject(unsigned int contextID, TextureObject* to)
        {
            _textureObjectBuffer.reset(to);;
        }
          
        /** Force a manual allocation of the mipmap levels on the next apply() call.
          * User is responsible for filling the mipmap levels with valid data.
          * The OpenGL's glGenerateMipmapEXT function is used to generate the mipmap levels.
          * If glGenerateMipmapEXT is not supported or texture's internal format is not supported
          * by the glGenerateMipmapEXT, then empty mipmap levels will
          * be allocated manually. The mipmap levels are also allocated if a non-mipmapped
          * min filter is used. */
        void allocateMipmapLevels();

        /** Sets GL_TEXTURE_COMPARE_MODE_ARB to GL_COMPARE_R_TO_TEXTURE_ARB
          * See http://oss.sgi.com/projects/ogl-sample/registry/ARB/shadow.txt. */
        void setShadowComparison(bool flag) { _use_shadow_comparison = flag; }
        bool getShadowComparison() const { return _use_shadow_comparison; }

        enum ShadowCompareFunc {
            NEVER = GL_NEVER,
            LESS = GL_LESS,
            EQUAL = GL_EQUAL,
            LEQUAL = GL_LEQUAL,
            GREATER = GL_GREATER,
            NOTEQUAL = GL_NOTEQUAL,
            GEQUAL = GL_GEQUAL,
            ALWAYS = GL_ALWAYS
        };

        /** Sets shadow texture comparison function. */
        void setShadowCompareFunc(ShadowCompareFunc func) { _shadow_compare_func = func; }
        ShadowCompareFunc getShadowCompareFunc() const { return _shadow_compare_func; }

        enum ShadowTextureMode {
            LUMINANCE = GL_LUMINANCE,
            INTENSITY = GL_INTENSITY,
            ALPHA = GL_ALPHA,
            NONE = GL_NONE
        };

        /** Sets shadow texture mode after comparison. */
        void setShadowTextureMode(ShadowTextureMode mode) { _shadow_texture_mode = mode; }
        ShadowTextureMode getShadowTextureMode() const { return _shadow_texture_mode; }

        /** Sets the TEXTURE_COMPARE_FAIL_VALUE_ARB texture parameter. See
          * http://oss.sgi.com/projects/ogl-sample/registry/ARB/shadow_ambient.txt. */
        void setShadowAmbient(float shadow_ambient) { _shadow_ambient = shadow_ambient; }
        float getShadowAmbient() const { return _shadow_ambient; }


        /** Sets the texture image for the specified face. */
        virtual void setImage(unsigned int face, Image* image) = 0;

        template<class T> void setImage(unsigned int face, const std::shared_ptr<T>& image) { setImage(face, image.get()); }

        /** Gets the texture image for the specified face. */
        virtual Image* getImage(unsigned int face) = 0;

        /** Gets the const texture image for specified face. */
        virtual const Image* getImage(unsigned int face) const = 0;

        /** Gets the number of images that can be assigned to this Texture. */
        virtual unsigned int getNumImages() const = 0;
           
        /** Determine whether the given internalFormat is a compressed
          * image format. */
        static bool isCompressedInternalFormat(GLint internalFormat);

        /** Determine the size of a compressed image, given the internalFormat,
          * the width, the height, and the depth of the image. The block size
          * and the size are output parameters. */
        static void getCompressedSize(GLenum internalFormat, GLint width, GLint height, GLint depth, GLint& blockSize, GLint& size);


        /** Helper method. Creates the texture, but doesn't set or use a
          * texture binding. Note: Don't call this method directly unless
          * you're implementing a subload callback. */
        void applyTexImage2D_load(State& state, GLenum target, const Image* image, GLsizei width, GLsizei height,GLsizei numMipmapLevels) const;

        /** Helper method. Subloads images into the texture, but doesn't set
          * or use a texture binding. Note: Don't call this method directly
          * unless you're implementing a subload callback. */
        void applyTexImage2D_subload(State& state, GLenum target, const Image* image, GLsizei width, GLsizei height, GLint inInternalFormat, GLsizei numMipmapLevels) const;


        /** Returned by mipmapBeforeTexImage() to indicate what
          * mipmapAfterTexImage() should do */
        enum GenerateMipmapMode
        {
            GENERATE_MIPMAP_NONE,
            GENERATE_MIPMAP,
            GENERATE_MIPMAP_TEX_PARAMETER
        };
        virtual ~Texture();

    protected :

 
        virtual void computeInternalFormat() const = 0;

        /** Computes the internal format from Image parameters. */
        void computeInternalFormatWithImage(const Image& image) const;

        /** Computes the texture dimension for the given Image. */
        void computeRequiredTextureDimensions(State& state, const Image& image,GLsizei& width, GLsizei& height,GLsizei& numMipmapLevels) const;

        /** Computes the internal format type. */
        void computeInternalFormatType() const;

        /** Helper method. Sets texture parameters. */
        void applyTexParameters(GLenum target, State& state) const;
         
 
        /** Helper methods to be called before and after calling
          * gl[Compressed][Copy]Tex[Sub]Image2D to handle generating mipmaps. */
        GenerateMipmapMode mipmapBeforeTexImage(const State& state) const;
        void mipmapAfterTexImage(State& state, GenerateMipmapMode beforeResult) const;

        /** Helper method to generate mipmap levels by calling of glGenerateMipmapEXT.
          * If it is not supported, then call the virtual allocateMipmap() method */
        void generateMipmap(State& state) const;

        /** Allocate mipmap levels of the texture by subsequent calling of glTexImage* function. */
        virtual void allocateMipmap(State& state) const = 0;

        /** Returns -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
        int compareTexture(const Texture& rhs) const;

        /** Returns -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
        int compareTextureObjects(const Texture& rhs) const;
         
        WrapMode _wrap_s;
        WrapMode _wrap_t;
        WrapMode _wrap_r;

        FilterMode      _min_filter;
        FilterMode      _mag_filter;
        float           _maxAnisotropy;
        float           _minlod;
        float           _maxlod;
        float           _lodbias;
        Vec4i           _swizzle;
        //bool            _useHardwareMipMapGeneration;
        bool            _unrefImageDataAfterApply;
        bool            _clientStorageHint;
        bool            _resizeNonPowerOfTwoHint;

        Vec4d           _borderColor;
        GLint           _borderWidth;

        InternalFormatMode          _internalFormatMode;
        mutable InternalFormatType  _internalFormatType;
        mutable GLint       _internalFormat;
        mutable GLenum      _sourceFormat;
        mutable GLenum      _sourceType;

        bool                _use_shadow_comparison;
        ShadowCompareFunc   _shadow_compare_func;
        ShadowTextureMode   _shadow_texture_mode;
        float               _shadow_ambient;

    public:

        struct  TextureProfile
        {
            inline TextureProfile(GLenum target):
                _target(target),
                _numMipmapLevels(0),
                _internalFormat(0),
                _width(0),
                _height(0),
                _depth(0),
                _border(0),
                _size(0) {}

            inline TextureProfile(GLenum    target,
                                 GLint     numMipmapLevels,
                                 GLenum    internalFormat,
                                 GLsizei   width,
                                 GLsizei   height,
                                 GLsizei   depth,
                                 GLint     border):
                _target(target),
                _numMipmapLevels(numMipmapLevels),
                _internalFormat(internalFormat),
                _width(width),
                _height(height),
                _depth(depth),
                _border(border),
                _size(0) { computeSize(); }


            #define LESSTHAN(A,B) if (A<B) return true; if (B<A) return false;
            #define FINALLESSTHAN(A,B) return (A<B);

            bool operator < (const TextureProfile& rhs) const
            {
                LESSTHAN(_size,rhs._size);
                LESSTHAN(_target,rhs._target);
                LESSTHAN(_numMipmapLevels,rhs._numMipmapLevels);
                LESSTHAN(_internalFormat,rhs._internalFormat);
                LESSTHAN(_width,rhs._width);
                LESSTHAN(_height,rhs._height);
                LESSTHAN(_depth,rhs._depth);
                FINALLESSTHAN(_border, rhs._border);
            }

            bool operator == (const TextureProfile& rhs) const
            {
                return _target == rhs._target &&
                       _numMipmapLevels == rhs._numMipmapLevels &&
                       _internalFormat == rhs._internalFormat &&
                       _width == rhs._width &&
                       _height == rhs._height &&
                       _depth == rhs._depth &&
                       _border == rhs._border;
            }

            inline void set(GLint numMipmapLevels,
                            GLenum    internalFormat,
                            GLsizei   width,
                            GLsizei   height,
                            GLsizei   depth,
                            GLint     border)
            {
                _numMipmapLevels = numMipmapLevels;
                _internalFormat = internalFormat;
                _width = width;
                _height = height;
                _depth = depth;
                _border = border;
                computeSize();
            }

            inline bool match(GLenum    target,
                       GLint     numMipmapLevels,
                       GLenum    internalFormat,
                       GLsizei   width,
                       GLsizei   height,
                       GLsizei   depth,
                       GLint     border)
            {
                return (_target == target) &&
                       (_numMipmapLevels == numMipmapLevels) &&
                       (_internalFormat == internalFormat) &&
                       (_width == width) &&
                       (_height == height) &&
                       (_depth == depth) &&
                       (_border == border);
            }

            void computeSize();

            GLenum       _target;
            GLint        _numMipmapLevels;
            GLenum       _internalFormat;
            GLsizei      _width;
            GLsizei      _height;
            GLsizei      _depth;
            GLint        _border;
            unsigned int _size;
        };

        class  TextureObject //: public GraphicsObject
        {
        public:

            inline TextureObject(Texture* texture, GLuint id, GLenum target):
                _id(id),
                _profile(target),
 
                _texture(texture),
                _allocated(false),
                _frameLastUsed(0),
                _timeStamp(0) {}

            inline TextureObject(Texture* texture, GLuint id, const TextureProfile& profile):
                _id(id),
                _profile(profile),
          
                _texture(texture),
                _allocated(false),
                _frameLastUsed(0),
                _timeStamp(0) {}

            inline TextureObject(Texture* texture,
                          GLuint    id,
                          GLenum    target,
                          GLint     numMipmapLevels,
                          GLenum    internalFormat,
                          GLsizei   width,
                          GLsizei   height,
                          GLsizei   depth,
                          GLint     border):
                _id(id),
                _profile(target,numMipmapLevels,internalFormat,width,height,depth,border),
          
                _texture(texture),
                _allocated(false),
                _frameLastUsed(0),
                _timeStamp(0) {}

            inline bool match(GLenum    target,
                       GLint     numMipmapLevels,
                       GLenum    internalFormat,
                       GLsizei   width,
                       GLsizei   height,
                       GLsizei   depth,
                       GLint     border)
            {
                return isReusable() &&
                       _profile.match(target,numMipmapLevels,internalFormat,width,height,depth,border);
            }


            void bind();

            inline GLenum id() const { return _id; }
            inline GLenum target() const { return _profile._target; }

            inline unsigned int size() const { return _profile._size; }

            inline void setTexture(Texture* texture) { _texture = texture; }
            inline Texture* getTexture() const { return _texture; }

            inline void setTimeStamp(double timestamp) { _timeStamp = timestamp; }
            inline double getTimeStamp() const { return _timeStamp; }

            inline void setAllocated(bool allocated=true) { _allocated = allocated; }

            void setAllocated(GLint     numMipmapLevels,
                              GLenum    internalFormat,
                              GLsizei   width,
                              GLsizei   height,
                              GLsizei   depth,
                              GLint     border);

            inline bool isAllocated() const { return _allocated; }

            inline bool isReusable() const { return _allocated && _profile._width!=0; }

            /** release TextureObject to the orphan list to be reused or deleted.*/
            void release();

            GLuint              _id;
            TextureProfile      _profile;
            //TextureObjectSet*   _set;
            //TextureObject*      _previous;
            //TextureObject*      _next;
            Texture*            _texture;
            bool                _allocated;
            unsigned int        _frameLastUsed;
            double              _timeStamp;
 
            virtual ~TextureObject();

        };

        typedef std::list< std::shared_ptr<TextureObject> > TextureObjectList;


        static std::shared_ptr<TextureObject> generateTextureObject(const Texture* texture, unsigned int contextID,GLenum target);

        static std::shared_ptr<TextureObject> generateTextureObject(const Texture* texture,
                                                     unsigned int contextID,
                                                     GLenum    target,
                                                     GLint     numMipmapLevels,
                                                     GLenum    internalFormat,
                                                     GLsizei   width,
                                                     GLsizei   height,
                                                     GLsizei   depth,
                                                     GLint     border);

        TextureObject* generateAndAssignTextureObject(unsigned int contextID, GLenum target) const;

        TextureObject* generateAndAssignTextureObject(unsigned int contextID,
                                                     GLenum    target,
                                                     GLint     numMipmapLevels,
                                                     GLenum    internalFormat,
                                                     GLsizei   width,
                                                     GLsizei   height,
                                                     GLsizei   depth,
                                                     GLint     border) const;

    protected:
 
        mutable  std::shared_ptr<TextureObject>         _textureObjectBuffer;
 

};
 

#endif
