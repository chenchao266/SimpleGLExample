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

#ifndef OSG_IMAGE
#define OSG_IMAGE 1


#include <string>
#include <vector>
#include "OpenGLWindow/OpenGLInclude.h"
#include "Bullet3Common/geometry.h"
#ifndef GL_DEPTH_COMPONENT32F_NV
#define GL_DEPTH_COMPONENT32F_NV          0x8DAB
#endif

#ifndef GL_KHR_texture_compression_astc_hdr
#define GL_KHR_texture_compression_astc_hdr 1
#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR   0x93B0
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR   0x93B1
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR   0x93B2
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR   0x93B3
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR   0x93B4
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR   0x93B5
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR   0x93B6
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR   0x93B7
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR  0x93B8
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR  0x93B9
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR  0x93BA
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
#endif /* GL_KHR_texture_compression_astc_hdr */

#ifndef GL_texture_compression_s3tc
#define GL_COMPRESSED_RGB_S3TC_DXT1         0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1        0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3        0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5        0x83F3
#endif

#ifndef GL_texture_compression_rgtc
#define GL_COMPRESSED_RED_RGTC1                0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1         0x8DBC
#define GL_COMPRESSED_RED_GREEN_RGTC2          0x8DBD
#define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2   0x8DBE
#endif

#ifndef GL_IMG_texture_compression_pvrtc
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG      0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG      0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG     0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG     0x8C03
#endif

#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_ETC1_RGB8_OES                            0x8D64
#endif
 
#define GL_ALPHA32F                          0x8816
#define GL_INTENSITY32F                      0x8817
#define GL_LUMINANCE32F                      0x8818
#define GL_LUMINANCE_ALPHA32F                0x8819
#define GL_ALPHA16F                          0x881C
#define GL_INTENSITY16F                      0x881D
#define GL_LUMINANCE16F                      0x881E
#define GL_LUMINANCE_ALPHA16F                0x881F
#ifndef GL_texture_integer
 
#define GL_ALPHA32UI                                   0x8D72
#define GL_INTENSITY32UI                               0x8D73
#define GL_LUMINANCE32UI                               0x8D74
#define GL_LUMINANCE_ALPHA32UI                         0x8D75

 
#define GL_ALPHA16UI                                   0x8D78
#define GL_INTENSITY16UI                               0x8D79
#define GL_LUMINANCE16UI                               0x8D7A
#define GL_LUMINANCE_ALPHA16UI                         0x8D7B

 
#define GL_ALPHA8UI                                    0x8D7E
#define GL_INTENSITY8UI                                0x8D7F
#define GL_LUMINANCE8UI                                0x8D80
#define GL_LUMINANCE_ALPHA8UI                          0x8D81

 
#define GL_ALPHA32I                                    0x8D84
#define GL_INTENSITY32I                                0x8D85
#define GL_LUMINANCE32I                                0x8D86
#define GL_LUMINANCE_ALPHA32I                          0x8D87

 
#define GL_ALPHA16I                                    0x8D8A
#define GL_INTENSITY16I                                0x8D8B
#define GL_LUMINANCE16I                                0x8D8C
#define GL_LUMINANCE_ALPHA16I                          0x8D8D

 
#define GL_ALPHA8I                                     0x8D90
#define GL_INTENSITY8I                                 0x8D91
#define GL_LUMINANCE8I                                 0x8D92
#define GL_LUMINANCE_ALPHA8I                           0x8D93

  
#define GL_LUMINANCE_INTEGER                           0x8D9C
#define GL_LUMINANCE_ALPHA_INTEGER                     0x8D9D

#define GL_RGBA_INTEGER_MODE                           0x8D9E
#endif
#ifndef GL_NV_texture_shader
#define GL_HILO_NV                              0x86F4
#define GL_DSDT_NV                              0x86F5
#define GL_DSDT_MAG_NV                          0x86F6
#define GL_DSDT_MAG_VIB_NV                      0x86F7
#define GL_HILO16_NV                            0x86F8
#define GL_SIGNED_HILO_NV                       0x86F9
#define GL_SIGNED_HILO16_NV                     0x86FA
#define GL_SIGNED_RGBA_NV                       0x86FB
#define GL_SIGNED_RGBA8_NV                      0x86FC
#define GL_SIGNED_RGB_NV                        0x86FE
#define GL_SIGNED_RGB8_NV                       0x86FF
#define GL_SIGNED_LUMINANCE_NV                  0x8701
#define GL_SIGNED_LUMINANCE8_NV                 0x8702
#define GL_SIGNED_LUMINANCE_ALPHA_NV            0x8703
#define GL_SIGNED_LUMINANCE8_ALPHA8_NV          0x8704
#define GL_SIGNED_ALPHA_NV                      0x8705
#define GL_SIGNED_ALPHA8_NV                     0x8706
#define GL_SIGNED_INTENSITY_NV                  0x8707
#define GL_SIGNED_INTENSITY8_NV                 0x8708
#define GL_DSDT8_NV                             0x8709
#define GL_DSDT8_MAG8_NV                        0x870A
#define GL_DSDT8_MAG8_INTENSITY8_NV             0x870B
#define GL_SIGNED_RGB_UNSIGNED_ALPHA_NV         0x870C
#define GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV       0x870D
#endif
 
#ifndef TEXTURE_COMPARE_FAIL_VALUE
#define TEXTURE_COMPARE_FAIL_VALUE   0x80BF
#endif
 
/* Pixel storage modes, used by gluScaleImage */
struct PixelStorageModes
{
    // sets defaults as per glGet docs in OpenGL red book
    PixelStorageModes();

    // use glGet's to retrieve all the current settings
    void retrieveStoreModes();

    // use glGet's to retrieve all the current 3D settings
    void retrieveStoreModes3D();

    GLint pack_alignment;
    GLint pack_row_length;
    GLint pack_skip_rows;
    GLint pack_skip_pixels;
    GLint pack_lsb_first;
    GLint pack_swap_bytes;
    GLint pack_skip_images;
    GLint pack_image_height;

    GLint unpack_alignment;
    GLint unpack_row_length;
    GLint unpack_skip_rows;
    GLint unpack_skip_pixels;
    GLint unpack_lsb_first;
    GLint unpack_swap_bytes;
    GLint unpack_skip_images;
    GLint unpack_image_height;
};
class State;
/** Image class for encapsulating the storage texture image data. */
class   Image //: public BufferData
{

    public :

        Image();

        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        Image(const Image& image );
         
        virtual const void*   getDataPointer() const { return data(); }
        virtual unsigned int    getTotalDataSize() const { return getTotalSizeInBytesIncludingMipmaps(); }

       
        void setFileName(const std::string& fileName);
        inline const std::string& getFileName() const { return _fileName; }

        enum WriteHint {
            NO_PREFERENCE,
            STORE_INLINE,
            EXTERNAL_FILE
        };

        void setWriteHint(WriteHint writeHint) { _writeHint = writeHint; }
        WriteHint getWriteHint() const { return _writeHint; }

        enum AllocationMode {
            NO_DELETE,
            USE_NEW_DELETE,
            USE_MALLOC_FREE
        };

        /** Set the method used for deleting data once it goes out of scope. */
        void setAllocationMode(AllocationMode mode) { _allocationMode = mode; }

        /** Get the method used for deleting data once it goes out of scope. */
        AllocationMode getAllocationMode() const { return _allocationMode; }


        /** Allocate a pixel block of specified size and type. */
        virtual void allocateImage(int s,int t,int r,
                           GLenum pixelFormat,GLenum type,
                           int packing=1);


        /** Set the image dimensions, format and data. */
        virtual void setImage(int s,int t,int r,
                      GLint internalTextureformat,
                      GLenum pixelFormat,GLenum type,
                      unsigned char* data,
                      AllocationMode mode,
                      int packing=1, int rowLength=0);

        /** Read pixels from current frame buffer at specified position and size, using glReadPixels.
          * Create memory for storage if required, reuse existing pixel coords if possible.
        */
        virtual void readPixels(int x,int y,int width,int height,
                        GLenum pixelFormat, GLenum type, int packing=1);


        /** Read the contents of the current bound texture, handling compressed pixelFormats if present.
          * Create memory for storage if required, reuse existing pixel coords if possible.
        */
        virtual void readImageFromCurrentTexture(unsigned int contextID, bool copyMipMapsIfAvailable, GLenum type = GL_UNSIGNED_BYTE, unsigned int face = 0);

        /** swap the data and settings between two image objects.*/
        void swap(Image& rhs);
          
        enum Origin
        {
            BOTTOM_LEFT,
            TOP_LEFT
        };

        /** Set the origin of the image.
          * The default value is BOTTOM_LEFT and is consistent with OpenGL.
          * TOP_LEFT is used for imagery that follows standard Imagery convention, such as movies,
          * and hasn't been flipped yet.  For such images one much flip the t axis of the tex coords.
          * to handle this origin position. */
        void setOrigin(Origin origin) { _origin = origin; }

        /** Get the origin of the image.*/
        Origin getOrigin() const { return _origin; }


        /** Width of image. */
        inline int s() const { return _s; }

        /** Height of image. */
        inline int t() const { return _t; }

        /** Depth of image. */
        inline int r() const { return _r; }

        void setRowLength(int length);
        inline int getRowLength() const { return _rowLength; }

        void setInternalTextureFormat(GLint internalFormat);
        inline GLint getInternalTextureFormat() const { return _internalTextureFormat; }

        void setPixelFormat(GLenum pixelFormat);
        inline GLenum getPixelFormat() const { return _pixelFormat; }

        void setDataType(GLenum dataType);
        inline GLenum getDataType() const { return _dataType; }

        void setPacking(unsigned int packing) { _packing = packing; }
        inline unsigned int getPacking() const { return _packing; }

        /** Return true of the pixel format is an OpenGL compressed pixel format.*/
        bool isCompressed() const;

        /** Set the pixel aspect ratio, defined as the pixel width divided by the pixel height.*/
        inline void setPixelAspectRatio(float pixelAspectRatio) { _pixelAspectRatio = pixelAspectRatio; }

        /** Get the pixel aspect ratio.*/
        inline float getPixelAspectRatio() const { return _pixelAspectRatio; }

        /** Return the number of bits required for each pixel. */
        inline unsigned int getPixelSizeInBits() const { return computePixelSizeInBits(_pixelFormat,_dataType); }

        /** Return the number of bytes each row of pixels occupies once it has been packed. */
        inline unsigned int getRowSizeInBytes() const { return computeRowWidthInBytes(_s,_pixelFormat,_dataType,_packing); }

        /** Return the number of bytes between each successive row.
          * Note, getRowSizeInBytes() will only equal getRowStepInBytes() when isDataContiguous() return true. */
        inline unsigned int getRowStepInBytes() const { return computeRowWidthInBytes(_rowLength==0?_s:_rowLength,_pixelFormat,_dataType,_packing); }

        /** Return the number of bytes each image (_s*_t) of pixels occupies. */
        inline unsigned int getImageSizeInBytes() const { return getRowSizeInBytes()*_t; }

        /** Return the number of bytes between each successive image.
          * Note, getImageSizeInBytes() will only equal getImageStepInBytes() when isDataContiguous() return true. */
        inline unsigned int getImageStepInBytes() const { return getRowStepInBytes()*_t; }

        /** Return the number of bytes the whole row/image/volume of pixels occupies. */
        inline unsigned int getTotalSizeInBytes() const { return getImageSizeInBytes()*_r; }

        /** Return the number of bytes the whole row/image/volume of pixels occupies, including all mip maps if included. */
        unsigned int getTotalSizeInBytesIncludingMipmaps() const;

        /** Return true if the Image represent a valid and usable imagery.*/
        bool valid() const { return _s!=0 && _t!=0 && _r!=0 && _data!=0 && _dataType!=0; }

        /** Raw image data.
          * Note, data in successive rows may not be contiguous, isDataContiguous() return false then you should
          * take care to access the data per row rather than treating the whole data as a single block. */
        inline unsigned char* data() { return _data; }

        /** Raw const image data.
          * Note, data in successive rows may not be contiguous, isDataContiguous() return false then you should
          * take care to access the data per row rather than treating the whole data as a single block. */
        inline const unsigned char* data() const { return _data; }

        inline unsigned char* data(unsigned int column, unsigned int row = 0, unsigned int image = 0)
        {
            if (!_data) return NULL;
            return _data+(column*getPixelSizeInBits())/8+row*getRowStepInBytes()+image*getImageSizeInBytes();
        }

        inline const unsigned char* data(unsigned int column, unsigned int row = 0, unsigned int image = 0) const
        {
            if (!_data) return NULL;
            return _data+(column*getPixelSizeInBits())/8+row*getRowStepInBytes()+image*getImageSizeInBytes();
        }

        /** return true if the data stored in the image is a contiguous block of data.*/
        bool isDataContiguous() const { return _rowLength==0 || _rowLength==_s; }

        /** Convenience class for assisting the copying of image data when the image data isn't contiguous.*/
        class   DataIterator
        {
        public:
            DataIterator(const Image* image);
            DataIterator(const DataIterator& ri);
            ~DataIterator() {}

            /** advance iterator to next block of data.*/
            void operator ++ ();

            /** is iterator valid.*/
            bool valid() const { return _currentPtr!=0; }

            /** data pointer of current block to copy.*/
            const unsigned char* data() const { return _currentPtr; }

            /** Size of current block to copy.*/
            unsigned int size() const { return _currentSize; }

        protected:


            void assign();

            const Image*                   _image;
            int                                 _rowNum;
            int                                 _imageNum;
            unsigned int                        _mipmapNum;
            const unsigned char*                _currentPtr;
            unsigned int                        _currentSize;
        };

        /** Get the color value for specified texcoord.*/
        Vec4f getColor(unsigned int s,unsigned t=0,unsigned r=0) const;

        /** Get the color value for specified texcoord.*/
        Vec4f getColor(const Vec2f& texcoord) const { return getColor(Vec3f(texcoord.x,texcoord.y,0.0f)); }

        /** Get the color value for specified texcoord.*/
        Vec4f getColor(const Vec3f& texcoord) const;

         /** Set the color value for specified texcoord.*/
        void setColor(const Vec4f& color, unsigned int s, unsigned int t=0, unsigned int r=0);

         /** Set the color value for specified texcoord. Note texcoord is clamped to edge.*/
        void setColor(const Vec4f& color, const Vec2f& texcoord ) { setColor(color, Vec3f(texcoord.x, texcoord.y, 0.0f)); }

         /** Set the color value for specified texcoord. Note texcoord is clamped to edge.*/
        void setColor(const Vec4f& color, const Vec3f& texcoord );

        /** Flip the image horizontally, around s dimension. */
        void flipHorizontal();

        /** Flip the image vertically, around t dimension. */
        void flipVertical();

        /** Flip the image around the r dimension. Only relevant for 3D textures. */
        void flipDepth();

        /** Ensure image dimensions are a power of two.
          * Mipmapped textures require the image dimensions to be
          * power of two and are within the maximum texture size for
          * the host machine.
        */
        void ensureValidSizeForTexturing(GLint maxTextureSize);

        static bool isPackedType(GLenum type);
        static GLenum computePixelFormat(GLenum pixelFormat);
        static GLenum computeFormatDataType(GLenum pixelFormat);

        /** return the dimensions of a block of compressed pixels */
        static Vec3i computeBlockFootprint(GLenum pixelFormat);

        /** return the size in bytes of a block of compressed pixels */
        static unsigned int computeBlockSize(GLenum pixelFormat, GLenum packing);
        static unsigned int computeNumComponents(GLenum pixelFormat);
        static unsigned int computePixelSizeInBits(GLenum pixelFormat,GLenum type);
        static unsigned int computeRowWidthInBytes(int width,GLenum pixelFormat,GLenum type,int packing);
        static unsigned int computeImageSizeInBytes(int width,int height, int depth, GLenum pixelFormat, GLenum type, int packing = 1, int slice_packing = 1, int image_packing = 1);
        static int roudUpToMultiple(int s, int pack);
        static int computeNearestPowerOfTwo(int s,float bias=0.5f);
        static int computeNumberOfMipmapLevels(int s,int t = 1, int r = 1);

        /** Precomputed mipmaps stuff. */
        typedef std::vector< unsigned int > MipmapDataType;

        inline bool isMipmap() const {return !_mipmapData.empty();};

        unsigned int getNumMipmapLevels() const
        {
            return static_cast<unsigned int>(_mipmapData.size())+1;
        };

        /** Send offsets into data. It is assumed that first mipmap offset (index 0) is 0.*/
        inline void setMipmapLevels(const MipmapDataType& mipmapDataVector) { _mipmapData = mipmapDataVector; }

        inline const MipmapDataType& getMipmapLevels() const { return _mipmapData; }

        inline unsigned int getMipmapOffset(unsigned int mipmapLevel) const
        {
            if(mipmapLevel == 0)
                return 0;
            else if (mipmapLevel < getNumMipmapLevels())
               return _mipmapData[mipmapLevel-1];
            return 0;
        };

        inline unsigned char* getMipmapData(unsigned int mipmapLevel)
        {
           return _data+getMipmapOffset(mipmapLevel);
        }

        inline const unsigned char* getMipmapData(unsigned int mipmapLevel) const
        {
           return _data+getMipmapOffset(mipmapLevel);
        }

        /** returns false for texture formats that do not support texture subloading */
        bool supportsTextureSubloading() const;

        /** Return true if this image is translucent - i.e. it has alpha values that are less 1.0 (when normalized). */
        virtual bool isImageTranslucent() const;
         
        virtual ~Image();
        void dirty() {};
    protected :

  
        Image& operator = (const Image&) { return *this; }
         
        std::string _fileName;
        WriteHint   _writeHint;

        Origin _origin;

        int _s, _t, _r;
        int _rowLength;
        GLint _internalTextureFormat;
        GLenum _pixelFormat;
        GLenum _dataType;
        unsigned int _packing;
        float _pixelAspectRatio;

        AllocationMode _allocationMode;
        unsigned char* _data;

        void deallocateData();

        void setData(unsigned char* data,AllocationMode allocationMode);

        MipmapDataType _mipmapData;
         
};
 
enum BufferComponent
{
    DEPTH_BUFFER,
    STENCIL_BUFFER,
    PACKED_DEPTH_STENCIL_BUFFER,
    COLOR_BUFFER,
    COLOR_BUFFER0,
    COLOR_BUFFER1 = COLOR_BUFFER0 + 1,
    COLOR_BUFFER2 = COLOR_BUFFER0 + 2,
    COLOR_BUFFER3 = COLOR_BUFFER0 + 3,
    COLOR_BUFFER4 = COLOR_BUFFER0 + 4,
    COLOR_BUFFER5 = COLOR_BUFFER0 + 5,
    COLOR_BUFFER6 = COLOR_BUFFER0 + 6,
    COLOR_BUFFER7 = COLOR_BUFFER0 + 7,
    COLOR_BUFFER8 = COLOR_BUFFER0 + 8,
    COLOR_BUFFER9 = COLOR_BUFFER0 + 9,
    COLOR_BUFFER10 = COLOR_BUFFER0 + 10,
    COLOR_BUFFER11 = COLOR_BUFFER0 + 11,
    COLOR_BUFFER12 = COLOR_BUFFER0 + 12,
    COLOR_BUFFER13 = COLOR_BUFFER0 + 13,
    COLOR_BUFFER14 = COLOR_BUFFER0 + 14,
    COLOR_BUFFER15 = COLOR_BUFFER0 + 15
};
#endif                                            // __SG_IMAGE_H
