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
#include <Texture.h>
 
struct InternalPixelRelations
{
    GLenum sizedInternalFormat;
    GLint internalFormat;
    GLenum type;
};


InternalPixelRelations sizedInternalFormats[] = {

      { GL_R8UI                                , GL_RED_INTEGER  , GL_UNSIGNED_BYTE                             }
    , { GL_R8I                                 , GL_RED_INTEGER  , GL_BYTE                                      }
    , { GL_R16UI                               , GL_RED_INTEGER  , GL_UNSIGNED_SHORT                            }
    , { GL_R16I                                , GL_RED_INTEGER  , GL_SHORT                                     }
    , { GL_R32UI                               , GL_RED_INTEGER  , GL_UNSIGNED_INT                              }
    , { GL_R32I                                , GL_RED_INTEGER  , GL_INT                                       }

    , { GL_RG8UI                               , GL_RG_INTEGER       , GL_UNSIGNED_BYTE                             }
    , { GL_RG8I                                , GL_RG_INTEGER       , GL_BYTE                                      }
    , { GL_RG16UI                              , GL_RG_INTEGER       , GL_UNSIGNED_SHORT                            }
    , { GL_RG16I                               , GL_RG_INTEGER       , GL_SHORT                                     }
    , { GL_RG32UI                              , GL_RG_INTEGER       , GL_UNSIGNED_INT                              }
    , { GL_RG32I                               , GL_RG_INTEGER       , GL_INT                                       }

    , { GL_RGB8UI                          , GL_RGB_INTEGER  , GL_UNSIGNED_BYTE                             }
    , { GL_RGB8I                           , GL_RGB_INTEGER  , GL_BYTE                                      }
    , { GL_RGB16UI                         , GL_RGB_INTEGER  , GL_UNSIGNED_SHORT                            }
    , { GL_RGB16I                          , GL_RGB_INTEGER  , GL_SHORT                                     }
    , { GL_RGB32UI                         , GL_RGB_INTEGER  , GL_UNSIGNED_INT                              }
    , { GL_RGB32I                          , GL_RGB_INTEGER  , GL_INT                                       }

    , { GL_RGBA8UI                         , GL_RGBA_INTEGER , GL_UNSIGNED_BYTE                             }
    , { GL_RGBA8I                          , GL_RGBA_INTEGER , GL_BYTE                                      }
 // , { GL_RGB10_A2UI                      , GL_RGBA_INTEGER , GL_UNSIGNED_INT_2_10_10_10_REV               }
    , { GL_RGBA16UI                        , GL_RGBA_INTEGER , GL_UNSIGNED_SHORT                            }
    , { GL_RGBA16I                         , GL_RGBA_INTEGER , GL_SHORT                                     }
    , { GL_RGBA32I                         , GL_RGBA_INTEGER , GL_INT                                       }
    , { GL_RGBA32UI                        , GL_RGBA_INTEGER , GL_UNSIGNED_INT                              }

    , { GL_R8                                  , GL_RED              , GL_UNSIGNED_BYTE                             }
    , { GL_R16F                                , GL_RED              , GL_HALF_FLOAT                                }
    , { GL_R32F                                , GL_RED              , GL_FLOAT                                     }
    , { GL_R16F                                , GL_RED              , GL_FLOAT                                     }
    , { GL_RG8                                 , GL_RG               , GL_UNSIGNED_BYTE                             }
    , { GL_RG16F                               , GL_RG               , GL_HALF_FLOAT                                }
    , { GL_RG16F                               , GL_RG               , GL_FLOAT                                     }
    , { GL_RG32F                               , GL_RG               , GL_FLOAT                                     }
 // , ( GL_RGBA2                               , GL_RGB              , UNKNOWN                                      )
    , { GL_R3_G3_B2                            , GL_RGB              , GL_UNSIGNED_BYTE_3_3_2                       }
    , { GL_R3_G3_B2                            , GL_RGB              , GL_UNSIGNED_BYTE_2_3_3_REV                   }
    , { GL_RGB4                                , GL_RGB              , GL_UNSIGNED_SHORT_4_4_4_4                    }
    , { GL_RGB4                                , GL_RGB              , GL_UNSIGNED_SHORT_4_4_4_4_REV                }
    , { GL_RGB5                                , GL_RGB              , GL_UNSIGNED_SHORT_5_5_5_1                    }
    , { GL_RGB5                                , GL_RGB              , GL_UNSIGNED_SHORT_1_5_5_5_REV                }
    , { GL_RGB8                                , GL_RGB              , GL_UNSIGNED_BYTE                             }
    , { GL_RGB565                              , GL_RGB              , GL_UNSIGNED_BYTE                             }
    , { GL_RGB565                              , GL_RGB              , GL_UNSIGNED_SHORT_5_6_5                      }
    , { GL_RGB565                              , GL_RGB              , GL_UNSIGNED_SHORT_5_6_5_REV                  }
 // , { GL_RGB9_E5                             , GL_RGB              , GL_UNSIGNED_INT_9_9_9_5,                     }
 // , { GL_RGB9_E5                             , GL_RGB              , GL_UNSIGNED_INT_5_9_9_9_REV,                 }
 // , { GL_RGB9_E5                             , GL_RGB              , GL_HALF_FLOAT                                }
 // , { GL_RGB9_E5                             , GL_RGB              , GL_FLOAT                                     }
 // , { GL_R11F_G11F_B10F                      , GL_RGB              , GL_UNSIGNED_INT_10F_11F_11F_REV              }
 // , { GL_R11F_G11F_B10F                      , GL_RGB              , GL_HALF_FLOAT                                }
 // , { GL_R11F_G11F_B10F                      , GL_RGB              , GL_FLOAT                                     }
    , { GL_RGB10                               , GL_RGB              , GL_UNSIGNED_INT_2_10_10_10_REV               }
    , { GL_RGB10                               , GL_RGB              , GL_UNSIGNED_INT_10_10_10_2                   }
    , { GL_RGB12                               , GL_RGB              , GL_UNSIGNED_SHORT                            }
    , { GL_RGB16F                          , GL_RGB              , GL_HALF_FLOAT                                }
    , { GL_RGB32F                          , GL_RGB              , GL_FLOAT                                     }
    , { GL_RGB16F                          , GL_RGB              , GL_FLOAT                                     }


    , { GL_RGBA8                               , GL_RGBA             , GL_UNSIGNED_BYTE                             }
    , { GL_RGBA16                              , GL_RGBA             , GL_UNSIGNED_SHORT                            }
    , { GL_RGB10_A2                            , GL_RGBA             , GL_UNSIGNED_INT_10_10_10_2                   }
    , { GL_RGB10_A2                            , GL_RGBA             , GL_UNSIGNED_INT_2_10_10_10_REV               }
    , { GL_RGBA12                              , GL_RGBA             , GL_UNSIGNED_SHORT                            }
    , { GL_RGBA4                               , GL_RGBA             , GL_UNSIGNED_BYTE                             }
    , { GL_RGBA4                               , GL_RGBA             , GL_UNSIGNED_SHORT_4_4_4_4                    }
    , { GL_RGBA4                               , GL_RGBA             , GL_UNSIGNED_SHORT_4_4_4_4_REV                }
    , { GL_RGB5_A1                             , GL_RGBA             , GL_UNSIGNED_BYTE                             }
    , { GL_RGB5_A1                             , GL_RGBA             , GL_UNSIGNED_SHORT_5_5_5_1                    }
    , { GL_RGB5_A1                             , GL_RGBA             , GL_UNSIGNED_SHORT_1_5_5_5_REV                }
    , { GL_RGB5_A1                             , GL_RGBA             , GL_UNSIGNED_INT_10_10_10_2                   }
    , { GL_RGB5_A1                             , GL_RGBA             , GL_UNSIGNED_INT_2_10_10_10_REV               }
 // , { GL_RGBA16F                             , GL_RGBA             , GL_HALF_FLOAT                                }
    , { GL_RGBA16F                         , GL_RGBA             , GL_FLOAT                                     }
    , { GL_RGBA32F                         , GL_RGBA             , GL_FLOAT                                     }

    , { GL_SRGB8                               , GL_RGB              , GL_UNSIGNED_BYTE                             }
    , { GL_SRGB8_ALPHA8                        , GL_RGBA             , GL_UNSIGNED_BYTE                             }

    , { GL_R8_SNORM                            , GL_RED              , GL_BYTE                                      }
    , { GL_R16_SNORM                           , GL_RED              , GL_SHORT                                     }
    , { GL_RG8_SNORM                           , GL_RG               , GL_BYTE                                      }
    , { GL_RG16_SNORM                          , GL_RG               , GL_SHORT                                     }
    , { GL_RGB8_SNORM                          , GL_RGB              , GL_BYTE                                      }
    , { GL_RGB16_SNORM                         , GL_RGB              , GL_SHORT                                     }
    , { GL_RGBA8_SNORM                         , GL_RGBA             , GL_BYTE                                      }
};

InternalPixelRelations sizedDepthAndStencilInternalFormats[] = {
      { GL_DEPTH_COMPONENT16                   , GL_DEPTH_COMPONENT  , GL_UNSIGNED_SHORT                            }
    , { GL_DEPTH_COMPONENT16                   , GL_DEPTH_COMPONENT  , GL_UNSIGNED_INT                              }
    , { GL_DEPTH_COMPONENT24                   , GL_DEPTH_COMPONENT  , GL_UNSIGNED_INT                              }
    , { GL_DEPTH_COMPONENT32                   , GL_DEPTH_COMPONENT  , GL_UNSIGNED_INT                              }
    , { GL_DEPTH_COMPONENT32F                  , GL_DEPTH_COMPONENT  , GL_FLOAT                                     }
 // , { GL_DEPTH24_STENCIL8                    , GL_DEPTH_STENCIL    , GL_UNSIGNED_INT_24_8                         }
 // , { GL_DEPTH32F_STENCIL8                   , GL_DEPTH_STENCIL    , GL_FLOAT_32_UNSIGNED_INT_24_8_REV            }
};

InternalPixelRelations compressedInternalFormats[] = {
 // , { GL_COMPRESSED_RED                      , GL_RED              , GL_COMPRESSED_RED                            }
 // , { GL_COMPRESSED_RG                       , GL_RG               , GL_COMPRESSED_RG                             }
      { GL_COMPRESSED_RGB                      , GL_RGB              , GL_COMPRESSED_RGB                            }
    , { GL_COMPRESSED_RGBA                     , GL_RGBA             , GL_COMPRESSED_RGBA                           }
    , { GL_COMPRESSED_SRGB                     , GL_RGB              , GL_COMPRESSED_SRGB                           }
    , { GL_COMPRESSED_SRGB_ALPHA               , GL_RGBA             , GL_COMPRESSED_SRGB_ALPHA                     }
    , { GL_COMPRESSED_RED_RGTC1            , GL_RED              , GL_COMPRESSED_RED_RGTC1                  }
    , { GL_COMPRESSED_SIGNED_RED_RGTC1     , GL_RED              , GL_COMPRESSED_SIGNED_RED_RGTC1           }
 // , { GL_COMPRESSED_RG_RGTC2                 , GL_RG               , GL_COMPRESSED_RG_RGTC2                       }
 // , { GL_COMPRESSED_SIGNED_RG_RGTC2          , GL_RG               , GL_COMPRESSED_SIGNED_RG_RGTC2                }
 // , { GL_COMPRESSED_RGBA_BPTC_UNORM          , GL_RGBA             , GL_COMPRESSED_RGBA_BPTC_UNORM                }
 // , { GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM    , GL_RGBA             , GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM          }
 // , { GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT    , GL_RGB              , GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT          }
 // , { GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT  , GL_RGB              , GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT        }

    , { GL_COMPRESSED_RGB_S3TC_DXT1        , GL_RGB              , GL_COMPRESSED_RGB_S3TC_DXT1              }
    , { GL_COMPRESSED_RGBA_S3TC_DXT1       , GL_RGBA             , GL_COMPRESSED_RGBA_S3TC_DXT1             }
    , { GL_COMPRESSED_RGBA_S3TC_DXT3       , GL_RGBA             , GL_COMPRESSED_RGBA_S3TC_DXT3             }
    , { GL_COMPRESSED_RGBA_S3TC_DXT5       , GL_RGBA             , GL_COMPRESSED_RGBA_S3TC_DXT5             }

 // , { GL_COMPRESSED_SRGB_S3TC_DXT1       , GL_RGB              , GL_COMPRESSED_SRGB_S3TC_DXT1             }
 // , { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1 , GL_RGBA             , GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1       }
 // , { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3 , GL_RGBA             , GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3       }
 // , { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5 , GL_RGBA             , GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5       }
};

bool isSizedInternalFormat(GLint internalFormat)
{
    const size_t formatsCount = sizeof(sizedInternalFormats) / sizeof(sizedInternalFormats[0]);

    for (size_t i=0; i < formatsCount; ++i)
    {
        if((GLenum)internalFormat == sizedInternalFormats[i].sizedInternalFormat)
            return true;
    }

    return false;
}

GLenum assumeSizedInternalFormat(GLint internalFormat, GLenum type)
{
    const size_t formatsCount = sizeof(sizedInternalFormats) / sizeof(sizedInternalFormats[0]);

    for (size_t i=0; i < formatsCount; ++i)
    {
        if(internalFormat == sizedInternalFormats[i].internalFormat && type == sizedInternalFormats[i].type)
            return sizedInternalFormats[i].sizedInternalFormat;
    }

    return 0;
}

bool isCompressedInternalFormatSupportedByTexStorage(GLint internalFormat)
{
    const size_t formatsCount = sizeof(compressedInternalFormats) / sizeof(compressedInternalFormats[0]);

    for (size_t i=0; i < formatsCount; ++i)
    {
        if((GLenum)internalFormat == compressedInternalFormats[i].sizedInternalFormat)
            return true;
    }

    return false;
}

Texture::TextureObject::~TextureObject()
{
    // //OSG_NOTICE<<"Texture::TextureObject::~TextureObject() "<<this<<std::endl;
}

void Texture::TextureObject::bind()
{
    glBindTexture( _profile._target, _id);
    //if (_set) _set->moveToBack(this);
}

void Texture::TextureObject::release()
{
    //if (_set) _set->orphan(this);
}

void Texture::TextureObject::setAllocated(GLint     numMipmapLevels,
                                          GLenum    internalFormat,
                                          GLsizei   width,
                                          GLsizei   height,
                                          GLsizei   depth,
                                          GLint     border)
{
    _allocated=true;
    if (!match(_profile._target,numMipmapLevels,internalFormat,width,height,depth,border))
    {
        // keep previous size
        unsigned int previousSize = _profile._size;

        _profile.set(numMipmapLevels,internalFormat,width,height,depth,border);

        if (0)
        {
            //_set->moveToSet(this, _set->getParent()->getTextureObjectSet(_profile));
             
        }
    }
}

void Texture::TextureProfile::computeSize()
{
    unsigned int numBitsPerTexel = 32;

    switch(_internalFormat)
    {
        case(1): numBitsPerTexel = 8; break;
        case(GL_ALPHA): numBitsPerTexel = 8; break;
        case(GL_LUMINANCE): numBitsPerTexel = 8; break;
        case(GL_INTENSITY): numBitsPerTexel = 8; break;

        case(GL_LUMINANCE_ALPHA): numBitsPerTexel = 16; break;
        case(2): numBitsPerTexel = 16; break;

        case(GL_RGB): numBitsPerTexel = 24; break;
        case(GL_BGR): numBitsPerTexel = 24; break;
        case(3): numBitsPerTexel = 24; break;

        case(GL_RGBA): numBitsPerTexel = 32; break;
        case(4): numBitsPerTexel = 32; break;

        case(GL_COMPRESSED_ALPHA):           numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_INTENSITY):       numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_LUMINANCE_ALPHA): numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_RGB_S3TC_DXT1):   numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_RGBA_S3TC_DXT1):  numBitsPerTexel = 4; break;

        case(GL_COMPRESSED_RGB):             numBitsPerTexel = 8; break;
        case(GL_COMPRESSED_RGBA_S3TC_DXT3):  numBitsPerTexel = 8; break;
        case(GL_COMPRESSED_RGBA_S3TC_DXT5):  numBitsPerTexel = 8; break;

        case(GL_COMPRESSED_SIGNED_RED_RGTC1):       numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_RED_RGTC1):              numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2): numBitsPerTexel = 8; break;
        case(GL_COMPRESSED_RED_GREEN_RGTC2):        numBitsPerTexel = 8; break;

        case(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG):  numBitsPerTexel = 2; break;
        case(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG): numBitsPerTexel = 2; break;
        case(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG):  numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG): numBitsPerTexel = 4; break;

        case(GL_ETC1_RGB8_OES):                       numBitsPerTexel = 4; break;

        case(GL_COMPRESSED_RGB8_ETC2):                      numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_SRGB8_ETC2):                     numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2):  numBitsPerTexel = 8; break;
        case(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2): numBitsPerTexel = 8; break;
        case(GL_COMPRESSED_RGBA8_ETC2_EAC):                 numBitsPerTexel = 8; break;
        case(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC):          numBitsPerTexel = 8; break;
        case(GL_COMPRESSED_R11_EAC):                        numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_SIGNED_R11_EAC):                 numBitsPerTexel = 4; break;
        case(GL_COMPRESSED_RG11_EAC):                       numBitsPerTexel = 8; break;
        case(GL_COMPRESSED_SIGNED_RG11_EAC):                numBitsPerTexel = 8; break;
    }

    _size = (unsigned int)(ceil(double(_width * _height * _depth * numBitsPerTexel)/8.0));

    if (_numMipmapLevels>1)
    {
        unsigned int mipmapSize = _size / 4;
        for(GLint i=0; i<_numMipmapLevels && mipmapSize!=0; ++i)
        {
            _size += mipmapSize;
            mipmapSize /= 4;
        }
    }

    // //OSG_NOTICE<<"TO ("<<_width<<", "<<_height<<", "<<_depth<<") size="<<_size<<" numBitsPerTexel="<<numBitsPerTexel<<std::endl;
}
 
Texture::TextureObject* Texture::generateAndAssignTextureObject(unsigned int contextID, GLenum target) const
{
    _textureObjectBuffer = generateTextureObject(this, contextID, target);
    return _textureObjectBuffer.get();
}

Texture::TextureObject* Texture::generateAndAssignTextureObject(
                                             unsigned int contextID,
                                             GLenum    target,
                                             GLint     numMipmapLevels,
                                             GLenum    internalFormat,
                                             GLsizei   width,
                                             GLsizei   height,
                                             GLsizei   depth,
                                             GLint     border) const
{
    _textureObjectBuffer = generateTextureObject(this, contextID, target, numMipmapLevels, internalFormat, width, height, depth, border);
    return _textureObjectBuffer.get();
}
 
std::shared_ptr<Texture::TextureObject> Texture::generateTextureObject(const Texture* texture, unsigned int contextID, GLenum target)
{
    return nullptr;
}

std::shared_ptr<Texture::TextureObject> Texture::generateTextureObject(const Texture* texture, unsigned int contextID,
                                             GLenum    target,
                                             GLint     numMipmapLevels,
                                             GLenum    internalFormat,
                                             GLsizei   width,
                                             GLsizei   height,
                                             GLsizei   depth,
                                             GLint     border)
{
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Texture class implementation
//
Texture::Texture():
            _wrap_s(CLAMP),
            _wrap_t(CLAMP),
            _wrap_r(CLAMP),
            _min_filter(LINEAR_MIPMAP_LINEAR), // trilinear
            _mag_filter(LINEAR),
            _maxAnisotropy(1.0f),
            _minlod(0.0f),
            _maxlod(-1.0f),
            _lodbias(0.0f),
            _swizzle(GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA),
            //_useHardwareMipMapGeneration(true),
            _unrefImageDataAfterApply(false),
            _clientStorageHint(false),
            _resizeNonPowerOfTwoHint(true),
            _borderColor(0.0, 0.0, 0.0, 0.0),
            _borderWidth(0),
            _internalFormatMode(USE_IMAGE_DATA_FORMAT),
            _internalFormatType(NORMALIZED),
            _internalFormat(0),
            _sourceFormat(0),
            _sourceType(0),
            _use_shadow_comparison(false),
            _shadow_compare_func(LEQUAL),
            _shadow_texture_mode(LUMINANCE),
            _shadow_ambient(0)
{
}

Texture::Texture(const Texture& text):
            //StateAttribute(text),
            _wrap_s(text._wrap_s),
            _wrap_t(text._wrap_t),
            _wrap_r(text._wrap_r),
            _min_filter(text._min_filter),
            _mag_filter(text._mag_filter),
            _maxAnisotropy(text._maxAnisotropy),
            _minlod(text._minlod),
            _maxlod(text._maxlod),
            _lodbias(text._lodbias),
            _swizzle(text._swizzle),
            //_useHardwareMipMapGeneration(text._useHardwareMipMapGeneration),
            _unrefImageDataAfterApply(text._unrefImageDataAfterApply),
            _clientStorageHint(text._clientStorageHint),
            _resizeNonPowerOfTwoHint(text._resizeNonPowerOfTwoHint),
            _borderColor(text._borderColor),
            _borderWidth(text._borderWidth),
            _internalFormatMode(text._internalFormatMode),
            _internalFormatType(text._internalFormatType),
            _internalFormat(text._internalFormat),
            _sourceFormat(text._sourceFormat),
            _sourceType(text._sourceType),
            _use_shadow_comparison(text._use_shadow_comparison),
            _shadow_compare_func(text._shadow_compare_func),
            _shadow_texture_mode(text._shadow_texture_mode),
            _shadow_ambient(text._shadow_ambient)
{
}

Texture::~Texture()
{
 
}

 
void Texture::setWrap(WrapParameter which, WrapMode wrap)
{
    switch( which )
    {
        case WRAP_S : _wrap_s = wrap;  break;
        case WRAP_T : _wrap_t = wrap;  break;
        case WRAP_R : _wrap_r = wrap; break;
        default : //OSG_WARN<<"Error: invalid 'which' passed Texture::setWrap("<<(unsigned int)which<<","<<(unsigned int)wrap<<")"<<std::endl; 
            break;
    }
}


Texture::WrapMode Texture::getWrap(WrapParameter which) const
{
    switch( which )
    {
        case WRAP_S : return _wrap_s;
        case WRAP_T : return _wrap_t;
        case WRAP_R : return _wrap_r;
        default : //OSG_WARN<<"Error: invalid 'which' passed Texture::getWrap(which)"<<std::endl; 
            return _wrap_s;
    }
}


void Texture::setFilter(FilterParameter which, FilterMode filter)
{
    switch( which )
    {
        case MIN_FILTER : _min_filter = filter;  break;
        case MAG_FILTER : _mag_filter = filter;  break;
        default : //OSG_WARN<<"Error: invalid 'which' passed Texture::setFilter("<<(unsigned int)which<<","<<(unsigned int)filter<<")"<<std::endl; 
            break;
    }
}


Texture::FilterMode Texture::getFilter(FilterParameter which) const
{
    switch( which )
    {
        case MIN_FILTER : return _min_filter;
        case MAG_FILTER : return _mag_filter;
        default : //OSG_WARN<<"Error: invalid 'which' passed Texture::getFilter(which)"<< std::endl; 
            return _min_filter;
    }
}

void Texture::setMaxAnisotropy(float anis)
{
    if (_maxAnisotropy!=anis)
    {
        _maxAnisotropy = anis;
 
    }
}

void Texture::setMinLOD(float anis)
{
    if (_minlod!=anis)
    {
        _minlod = anis;
  
    }
}

void Texture::setMaxLOD(float anis)
{
    if (_maxlod!=anis)
    {
        _maxlod = anis;
 
    }
}

void Texture::setLODBias(float anis)
{
    if (_lodbias!=anis)
    {
        _lodbias = anis;
 
    }
}
  
void Texture::allocateMipmapLevels()
{
 
}

void Texture::computeInternalFormatWithImage(const Image& image) const
{
    GLint internalFormat = image.getInternalTextureFormat();

    if (_internalFormatMode==USE_IMAGE_DATA_FORMAT)
    {
        internalFormat = image.getInternalTextureFormat();
    }
    else if (_internalFormatMode==USE_USER_DEFINED_FORMAT)
    {
        internalFormat = _internalFormat;
    }
    else
    {

        const unsigned int contextID = 0; // state.getContextID();  // set to 0 right now, assume same parameters for each graphics context...
        bool isTextureCompressionS3TCSupported = true;//GL_EXT_texture_compression_s3tc
        bool isTextureCompressionPVRTCSupported = true;//GL_IMG_texture_compression_pvrtc
        bool isTextureCompressionETCSupported = true; // "GL_OES_compressed_ETC1_RGB8_texture");
        bool isTextureCompressionETC2Supported = true; // "GL_ARB_ES3_compatibility");
        bool isTextureCompressionRGTCSupported =  true; // "GL_EXT_texture_compression_rgtc");
   
        switch(_internalFormatMode)
        {
        case(USE_ARB_COMPRESSION):
            if (1)
            {
                switch(image.getPixelFormat())
                {
                    case(1): internalFormat = GL_COMPRESSED_ALPHA; break;
                    case(2): internalFormat = GL_COMPRESSED_LUMINANCE_ALPHA; break;
                    case(3): internalFormat = GL_COMPRESSED_RGB; break;
                    case(4): internalFormat = GL_COMPRESSED_RGBA; break;
                    case(GL_RGB): internalFormat = GL_COMPRESSED_RGB; break;
                    case(GL_RGBA): internalFormat = GL_COMPRESSED_RGBA; break;
                    case(GL_ALPHA): internalFormat = GL_COMPRESSED_ALPHA; break;
                    case(GL_LUMINANCE): internalFormat = GL_COMPRESSED_LUMINANCE; break;
                    case(GL_LUMINANCE_ALPHA): internalFormat = GL_COMPRESSED_LUMINANCE_ALPHA; break;
                    case(GL_INTENSITY): internalFormat = GL_COMPRESSED_INTENSITY; break;
                }
            }
            break;

        case(USE_S3TC_DXT1_COMPRESSION):
            if (isTextureCompressionS3TCSupported)//GL_EXT_texture_compression_s3tc
            {
                switch(image.getPixelFormat())
                {
                    case(3):        internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1; break;
                    case(4):        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1; break;
                    case(GL_RGB):   internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1; break;
                    case(GL_RGBA):  internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1; break;
                    default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_S3TC_DXT1c_COMPRESSION):
            if (isTextureCompressionS3TCSupported)
            {
                switch(image.getPixelFormat())
                {
                    case(3):
                    case(4):
                    case(GL_RGB):
                    case(GL_RGBA):  internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1; break;
                    default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_S3TC_DXT1a_COMPRESSION):
            if (isTextureCompressionS3TCSupported)
            {
                switch(image.getPixelFormat())
                {
                    case(3):
                    case(4):
                    case(GL_RGB):
                    case(GL_RGBA):  internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1; break;
                    default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_S3TC_DXT3_COMPRESSION):
            if (isTextureCompressionS3TCSupported)
            {
                switch(image.getPixelFormat())
                {
                    case(3):
                    case(GL_RGB):   internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1; break;
                    case(4):
                    case(GL_RGBA):  internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3; break;
                    default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_S3TC_DXT5_COMPRESSION):
            if (isTextureCompressionS3TCSupported)
            {
                switch(image.getPixelFormat())
                {
                    case(3):
                    case(GL_RGB):   internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1; break;
                    case(4):
                    case(GL_RGBA):  internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5; break;
                    default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_PVRTC_2BPP_COMPRESSION):
            if (isTextureCompressionPVRTCSupported)//GL_IMG_texture_compression_pvrtc
            {
                switch(image.getPixelFormat())
                {
                case(3):
                case(GL_RGB):   internalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG; break;
                case(4):
                case(GL_RGBA):  internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG; break;
                default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_PVRTC_4BPP_COMPRESSION):
            if (isTextureCompressionPVRTCSupported)
            {
                switch(image.getPixelFormat())
                {
                case(3):
                case(GL_RGB):   internalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG; break;
                case(4):
                case(GL_RGBA):  internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; break;
                default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_ETC_COMPRESSION):
            if (isTextureCompressionETCSupported)
            {
                switch(image.getPixelFormat())
                {
                case(3):
                case(GL_RGB):   internalFormat = GL_ETC1_RGB8_OES; break;
                default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_ETC2_COMPRESSION):
            if (isTextureCompressionETC2Supported)
            {
                switch(image.getPixelFormat())
                {
                    case(1):
                    case(GL_RED):   internalFormat = GL_COMPRESSED_R11_EAC; break;
                    case(2):
                    case(GL_RG):   internalFormat = GL_COMPRESSED_RG11_EAC; break;
                    case(3):
                    case(GL_RGB):   internalFormat = GL_COMPRESSED_RGB8_ETC2; break;
                    case(4):
                    case(GL_RGBA):  internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC; break;
                    default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_RGTC1_COMPRESSION):
            if (isTextureCompressionRGTCSupported)
            {
                switch(image.getPixelFormat())
                {
                    case(3):
                    case(GL_RGB):   internalFormat = GL_COMPRESSED_RED_RGTC1; break;
                    case(4):
                    case(GL_RGBA):  internalFormat = GL_COMPRESSED_RED_RGTC1; break;
                    default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        case(USE_RGTC2_COMPRESSION):
            if (isTextureCompressionRGTCSupported)
            {
                switch(image.getPixelFormat())
                {
                    case(3):
                    case(GL_RGB):   internalFormat = GL_COMPRESSED_RED_GREEN_RGTC2; break;
                    case(4):
                    case(GL_RGBA):  internalFormat = GL_COMPRESSED_RED_GREEN_RGTC2; break;
                    default:        internalFormat = image.getInternalTextureFormat(); break;
                }
            }
            break;

        default:
            break;
        }
    }

#if BT_USE_EGL
    // GLES doesn't cope with internal formats of 1,2,3 and 4 and glTexImage doesn't
    // handle the _OES pixel formats so map them to the appropriate equivilants.
    switch(internalFormat)
    {
        case(1) : internalFormat = GL_LUMINANCE; break;
        case(2) : internalFormat = GL_LUMINANCE_ALPHA; break;
        case(3) : internalFormat = GL_RGB; break;
        case(4) : internalFormat = GL_RGBA; break;
        case(GL_RGB8_OES) : internalFormat = GL_RGB; break;
        case(GL_RGBA8_OES) : internalFormat = GL_RGBA; break;
        default: break;
    }
#elif 1
    switch(internalFormat)
    {
        case(GL_INTENSITY) : internalFormat = GL_RED; break; // should it be swizzled to match RGBA(INTENSITY, INTENSITY, INTENSITY, INTENSITY)?
        case(GL_LUMINANCE) : internalFormat = GL_RED; break; // should it be swizzled to match RGBA(LUMINANCE, LUMINANCE, LUMINANCE, 1.0)?
        case(1) : internalFormat = GL_RED; break; // or should this be GL_ALPHA?
        case(2) : internalFormat = GL_RG; break; // should we assume GL_LUMINANCE_ALPHA?
        case(GL_LUMINANCE_ALPHA) : internalFormat = GL_RG; break; // should it be swizlled to match RGAB(LUMUNIANCE, LUMINANCE, LUMINANCE, ALPHA)?
        case(3) : internalFormat = GL_RGB; break;
        case(4) : internalFormat = GL_RGBA; break;
        default: break;
    }
#endif

    _internalFormat = internalFormat;


    computeInternalFormatType();

    ////OSG_NOTICE<<"Internal format="<<std::hex<<internalFormat<<std::dec<<std::endl;
}

void Texture::computeInternalFormatType() const
{
    // Here we could also precompute the _sourceFormat if it is not set,
    // since it is different for different internal formats
    // (i.e. rgba integer texture --> _sourceFormat = GL_RGBA_INTEGER)
    // Should we do this?  ( Art, 09. Sept. 2007)

    // compute internal format type based on the internal format
    switch(_internalFormat)
    {
        case GL_RGBA32UI:
        case GL_RGBA16UI:
        case GL_RGBA8UI:

        case GL_RGB32UI:
        case GL_RGB16UI:
        case GL_RGB8UI:

        case GL_RG32UI:
        case GL_RG16UI:
        case GL_RG8UI:

        case GL_R32UI:
        case GL_R16UI:
        case GL_R8UI:

        case GL_LUMINANCE32UI:
        case GL_LUMINANCE16UI:
        case GL_LUMINANCE8UI:

        case GL_INTENSITY32UI:
        case GL_INTENSITY16UI:
        case GL_INTENSITY8UI:

        case GL_LUMINANCE_ALPHA32UI:
        case GL_LUMINANCE_ALPHA16UI:
        case GL_LUMINANCE_ALPHA8UI :
            _internalFormatType = UNSIGNED_INTEGER;
            break;

        case GL_RGBA32I:
        case GL_RGBA16I:
        case GL_RGBA8I:

        case GL_RGB32I:
        case GL_RGB16I:
        case GL_RGB8I:

        case GL_RG32I:
        case GL_RG16I:
        case GL_RG8I:

        case GL_R32I:
        case GL_R16I:
        case GL_R8I:

        case GL_LUMINANCE32I:
        case GL_LUMINANCE16I:
        case GL_LUMINANCE8I:

        case GL_INTENSITY32I:
        case GL_INTENSITY16I:
        case GL_INTENSITY8I:

        case GL_LUMINANCE_ALPHA32I:
        case GL_LUMINANCE_ALPHA16I:
        case GL_LUMINANCE_ALPHA8I:
            _internalFormatType = SIGNED_INTEGER;
            break;

        case GL_RGBA32F:
        case GL_RGBA16F:

        case GL_RGB32F:
        case GL_RGB16F:

        case GL_RG32F:
        case GL_RG16F:

        case GL_R32F:
        case GL_R16F:

        case GL_LUMINANCE32F:
        case GL_LUMINANCE16F:

        case GL_INTENSITY32F:
        case GL_INTENSITY16F:

        case GL_LUMINANCE_ALPHA32F:
        case GL_LUMINANCE_ALPHA16F:
            _internalFormatType = FLOAT;
            break;

        default:
            _internalFormatType = NORMALIZED;
            break;
    };
}

bool Texture::isCompressedInternalFormat() const
{
    return isCompressedInternalFormat(getInternalFormat());
}

bool Texture::isCompressedInternalFormat(GLint internalFormat)
{
    switch(internalFormat)
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
        case(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG):
        case(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG):
        case(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG):
        case(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG):
            return true;
        default:
            return false;
    }
}

void Texture::getCompressedSize(GLenum internalFormat, GLint width, GLint height, GLint depth, GLint& blockSize, GLint& size)
{
    if (internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1 || internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1)
        blockSize = 8;
    else if (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3 || internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5)
        blockSize = 16;
    else if (internalFormat == GL_ETC1_RGB8_OES)
        blockSize = 8;
    else if (internalFormat == GL_COMPRESSED_RGB8_ETC2 || internalFormat == GL_COMPRESSED_SRGB8_ETC2)
        blockSize = 8;
    else if (internalFormat == GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 || internalFormat == GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2)
        blockSize = 8;
    else if (internalFormat == GL_COMPRESSED_RGBA8_ETC2_EAC || internalFormat == GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC)
        blockSize = 16;
    else if (internalFormat == GL_COMPRESSED_R11_EAC || internalFormat == GL_COMPRESSED_SIGNED_R11_EAC)
        blockSize = 8;
    else if (internalFormat == GL_COMPRESSED_RG11_EAC || internalFormat == GL_COMPRESSED_SIGNED_RG11_EAC)
        blockSize = 16;
    else if (internalFormat == GL_COMPRESSED_RED_RGTC1 || internalFormat == GL_COMPRESSED_SIGNED_RED_RGTC1)
        blockSize = 8;
    else if (internalFormat == GL_COMPRESSED_RED_GREEN_RGTC2 || internalFormat == GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2)
        blockSize = 16;
    else if (internalFormat == GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG || internalFormat == GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG)
    {
         blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
         GLint widthBlocks = width / 8;
         GLint heightBlocks = height / 4;
         GLint bpp = 2;

         // Clamp to minimum number of blocks
         if(widthBlocks < 2)
             widthBlocks = 2;
         if(heightBlocks < 2)
             heightBlocks = 2;

         size = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
         return;
     }
    else if (internalFormat == GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG || internalFormat == GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG)
    {
         blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
         GLint widthBlocks = width / 4;
         GLint heightBlocks = height / 4;
         GLint bpp = 4;

         // Clamp to minimum number of blocks
         if(widthBlocks < 2)
             widthBlocks = 2;
         if(heightBlocks < 2)
             heightBlocks = 2;

         size = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
         return;
    }
    else
    {
        //OSG_WARN<<"Texture::getCompressedSize(...) : cannot compute correct size of compressed format ("<<internalFormat<<") returning 0."<<std::endl;
        blockSize = 0;
    }

    size = ((width+3)/4)*((height+3)/4)*depth*blockSize;
}

void Texture::applyTexParameters(GLenum target, State& state) const
{
    // get the contextID (user defined ID of 0 upwards) for the
    // current OpenGL context.
    const unsigned int contextID = 0;
 
    TextureObject* to = getTextureObject(contextID);
    if (to)
    {
        //debugObjectLabel(GL_TEXTURE, to->id(), getName());
    }


    WrapMode ws = _wrap_s, wt = _wrap_t, wr = _wrap_r;
 
    #if 1
        if (ws == CLAMP) ws = CLAMP_TO_EDGE;
        if (wt == CLAMP) wt = CLAMP_TO_EDGE;
        if (wr == CLAMP) wr = CLAMP_TO_EDGE;
    #endif

    const Image * image = getImage(0);
    if( image &&
        image->isMipmap() &&
     
        int( image->getNumMipmapLevels() ) <
            Image::computeNumberOfMipmapLevels( image->s(), image->t(), image->r() ) )
            glTexParameteri( target, GL_TEXTURE_MAX_LEVEL, image->getNumMipmapLevels() - 1 );


    glTexParameteri( target, GL_TEXTURE_WRAP_S, ws );

    if (target!=GL_TEXTURE_1D) glTexParameteri( target, GL_TEXTURE_WRAP_T, wt );

    if (target==GL_TEXTURE_3D) glTexParameteri( target, GL_TEXTURE_WRAP_R, wr );


    glTexParameteri( target, GL_TEXTURE_MIN_FILTER, _min_filter);
    glTexParameteri( target, GL_TEXTURE_MAG_FILTER, _mag_filter);
    bool isTextureFilterAnisotropicSupported = false;// "GL_EXT_texture_filter_anisotropic");
    // Art: I think anisotropic filtering is not supported by the integer textures
    if (isTextureFilterAnisotropicSupported && //GL_texture_filter_anisotropic
        _internalFormatType != SIGNED_INTEGER && _internalFormatType != UNSIGNED_INTEGER)
    {
        // note, GL_TEXTURE_MAX_ANISOTROPY will either be defined
        // by gl.h (or via glext.h) or by include/Texture.
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY, _maxAnisotropy);
    }
    bool  isTextureSwizzleSupported = false;//"GL_ARB_texture_swizzle");

    if (isTextureSwizzleSupported)//GL_texture_swizzle
    {
        // note, GL_TEXTURE_SWIZZLE_RGBA will either be defined
        // by gl.h (or via glext.h) or by include/Texture.
        glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, _swizzle.u);
    }

    if (1)
    {
 

        if (_internalFormatType == SIGNED_INTEGER)
        {
            GLint color[4] = {(GLint)_borderColor.x, (GLint)_borderColor.y, (GLint)_borderColor.z, (GLint)_borderColor.w};
            glTexParameterIiv(target, GL_TEXTURE_BORDER_COLOR, color);
        }else if (_internalFormatType == UNSIGNED_INTEGER)
        {
            GLuint color[4] = {(GLuint)_borderColor.x, (GLuint)_borderColor.y, (GLuint)_borderColor.z, (GLuint)_borderColor.w};
            glTexParameterIuiv(target, GL_TEXTURE_BORDER_COLOR, color);
        }else{
            GLfloat color[4] = {(GLfloat)_borderColor.x, (GLfloat)_borderColor.y, (GLfloat)_borderColor.z, (GLfloat)_borderColor.w};
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color);
        }
    }
    bool isShadowAmbientSupported = false;// "GL_ARB_shadow_ambient"));
    // integer textures are not supported by the shadow
    // GL_TEXTURE_1D_ARRAY could be included in the check below but its not yet implemented in OSG
    if ( 
        (target == GL_TEXTURE_2D || target == GL_TEXTURE_1D || target == GL_TEXTURE_RECTANGLE || target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_2D_ARRAY ) &&
        _internalFormatType != SIGNED_INTEGER && _internalFormatType != UNSIGNED_INTEGER)
    {
        if (_use_shadow_comparison)
        {
            glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
            glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, _shadow_compare_func);
            #if defined(OSG_GL1_AVAILABLE) || defined(OSG_GL2_AVAILABLE)
                glTexParameteri(target, GL_DEPTH_TEXTURE_MODE, _shadow_texture_mode);
            #endif

            // if ambient value is 0 - it is default behaviour of GL_shadow
            // no need for GL_shadow_ambient in this case
            if (isShadowAmbientSupported && _shadow_ambient > 0)//GL_shadow_ambient
            {
                glTexParameterf(target, TEXTURE_COMPARE_FAIL_VALUE, _shadow_ambient);
            }
        }
        else
        {
            glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        }
    }
    // if range is valid
    if( _maxlod - _minlod >= 0)
    {
        glTexParameterf(target, GL_TEXTURE_MIN_LOD, _minlod);
        glTexParameterf(target, GL_TEXTURE_MAX_LOD, _maxlod);
    }

    glTexParameterf(target, GL_TEXTURE_LOD_BIAS, _lodbias);

 

}

void Texture::computeRequiredTextureDimensions(State& state, const Image& image,GLsizei& inwidth, GLsizei& inheight,GLsizei& numMipmapLevels) const
{
    int maxTextureSize = 65536;
    int width,height;

    if( !_resizeNonPowerOfTwoHint  )
    {
        width = image.s();
        height = image.t();
    }
    else
    {
        width = Image::computeNearestPowerOfTwo(image.s()-2*_borderWidth)+2*_borderWidth;
        height = Image::computeNearestPowerOfTwo(image.t()-2*_borderWidth)+2*_borderWidth;
    }

    // cap the size to what the graphics hardware can handle.
    if (width>maxTextureSize) width = maxTextureSize;
    if (height>maxTextureSize) height = maxTextureSize;

    inwidth = width;
    inheight = height;

    if( _min_filter == LINEAR || _min_filter == NEAREST)
    {
        numMipmapLevels = 1;
    }
    else if( image.isMipmap() )
    {
        numMipmapLevels = image.getNumMipmapLevels();
    }
    else
    {
        numMipmapLevels = 1;
        for(int s=1; s<width || s<height; s <<= 1, ++numMipmapLevels) {}
    }

    // //OSG_NOTICE<<"Texture::computeRequiredTextureDimensions() image.s() "<<image.s()<<" image.t()="<<image.t()<<" width="<<width<<" height="<<height<<" numMipmapLevels="<<numMipmapLevels<<std::endl;
    // //OSG_NOTICE<<"  _resizeNonPowerOfTwoHint="<<_resizeNonPowerOfTwoHint<< <<std::endl;
}

 
GLenum Texture::selectSizedInternalFormat(const Image* image) const
{
    if (image)
    {
        bool compressed_image = isCompressedInternalFormat((GLenum)image->getPixelFormat());

        //calculate sized internal format
        if(compressed_image)
        {
            if(isCompressedInternalFormatSupportedByTexStorage(_internalFormat))
            {
                return _internalFormat;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if(isSizedInternalFormat(_internalFormat))
            {
                return _internalFormat;
            }
            else
            {
                return assumeSizedInternalFormat((GLenum)image->getInternalTextureFormat(), (GLenum)image->getDataType());
            }
        }
    }
    else
    {
        if (isSizedInternalFormat(_internalFormat)) return _internalFormat;

        return assumeSizedInternalFormat(_internalFormat, (_sourceType!=0) ? _sourceType : GL_UNSIGNED_BYTE);
    }
}

void Texture::applyTexImage2D_load(State& state, GLenum target, const Image* image, GLsizei inwidth, GLsizei inheight,GLsizei numMipmapLevels) const
{
    // if we don't have a valid image we can't create a texture!
    if (!image || !image->data())
        return;
 
 
    // select the internalFormat required for the texture.
    bool compressed_image = isCompressedInternalFormat((GLenum)image->getPixelFormat());

    // If the texture's internal format is a compressed type, then the
    // user is requesting that the graphics card compress the image if it's
    // not already compressed. However, if the image is not a multiple of
    // four in each dimension the subsequent calls to glTexSubImage* will
    // fail. Revert to uncompressed format in this case.
    if (isCompressedInternalFormat(_internalFormat) &&
        (((inwidth >> 2) << 2) != inwidth ||
         ((inheight >> 2) << 2) != inheight))
    {
        //OSG_NOTICE<<"Received a request to compress an image, but image size is not a multiple of four ("<<inwidth<<"x"<<inheight<<"). Reverting to uncompressed.\n";
        switch(_internalFormat)
        {
            case GL_COMPRESSED_RGB_S3TC_DXT1:
            case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
            case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            case GL_ETC1_RGB8_OES:
            case(GL_COMPRESSED_RGB8_ETC2):
            case(GL_COMPRESSED_SRGB8_ETC2):
            case GL_COMPRESSED_RGB: _internalFormat = GL_RGB; break;
            case GL_COMPRESSED_RGBA_S3TC_DXT1:
            case GL_COMPRESSED_RGBA_S3TC_DXT3:
            case GL_COMPRESSED_RGBA_S3TC_DXT5:
            case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
            case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            case(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2):
            case(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2):
            case(GL_COMPRESSED_RGBA8_ETC2_EAC):
            case(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC):
            case GL_COMPRESSED_RGBA: _internalFormat = GL_RGBA; break;
            case GL_COMPRESSED_ALPHA: _internalFormat = GL_ALPHA; break;
            case GL_COMPRESSED_LUMINANCE: _internalFormat = GL_LUMINANCE; break;
            case GL_COMPRESSED_LUMINANCE_ALPHA: _internalFormat = GL_LUMINANCE_ALPHA; break;
            case GL_COMPRESSED_INTENSITY: _internalFormat = GL_INTENSITY; break;
            case(GL_COMPRESSED_R11_EAC):
            case(GL_COMPRESSED_SIGNED_R11_EAC):
            case GL_COMPRESSED_SIGNED_RED_RGTC1:
            case GL_COMPRESSED_RED_RGTC1: _internalFormat = GL_RED; break;
            case(GL_COMPRESSED_RG11_EAC):
            case(GL_COMPRESSED_SIGNED_RG11_EAC):
            case GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2:
            case GL_COMPRESSED_RED_GREEN_RGTC2: _internalFormat = GL_RG; break;
        }
    }
    bool   isClientStorageSupported = false;// "GL_APPLE_client_storage";
    glPixelStorei(GL_UNPACK_ALIGNMENT,image->getPacking());
    unsigned int rowLength = image->getRowLength();
    //GL_APPLE_client_storage
    bool useClientStorage = isClientStorageSupported && getClientStorageHint();
    if (useClientStorage)
    {
 
    }

    unsigned char* dataPtr = (unsigned char*)image->data();

    // //OSG_NOTICE<<"inwidth="<<inwidth<<" inheight="<<inheight<<" image->getFileName()"<<image->getFileName()<<std::endl;

    bool needImageRescale = inwidth!=image->s() || inheight!=image->t();
    if (needImageRescale)
    { 
    }

    bool mipmappingRequired = _min_filter != LINEAR && _min_filter != NEAREST;
    bool useHardwareMipMapGeneration = mipmappingRequired && (!image->isMipmap() );
 
    GLuint pbo = 1;// image->getOrCreateGLBufferObject(state.getContextID());
    glGenBuffers(1, &pbo);
    if (pbo && !needImageRescale)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
        ///dataPtr = reinterpret_cast<unsigned char*>(pbo->getOffset(image->getBufferIndex()));
        rowLength = 0;
 
    }
    else
    {
        pbo = 0;
    }
#ifndef BT_USE_EGL
    glPixelStorei(GL_UNPACK_ROW_LENGTH,rowLength);
#endif
    if( !mipmappingRequired || useHardwareMipMapGeneration)
    {

        GenerateMipmapMode mipmapResult = mipmapBeforeTexImage(state);

        if ( !compressed_image)
        {
            numMipmapLevels = 1;

            glTexImage2D( target, 0, _internalFormat,
                inwidth, inheight, _borderWidth,
                (GLenum)image->getPixelFormat(),
                (GLenum)image->getDataType(),
                dataPtr);

        }
        else if (true)
        {
            numMipmapLevels = 1;

            GLint blockSize, size;
            getCompressedSize(_internalFormat, inwidth, inheight, 1, blockSize,size);

            glCompressedTexImage2D(target, 0, _internalFormat,
                inwidth, inheight,0,
                size,
                dataPtr);
        }

        mipmapAfterTexImage(state, mipmapResult);
    }
    else
    {
        // we require mip mapping.
        if(image->isMipmap())
        {

            // image is mip mapped so we take the mip map levels from the image.

            numMipmapLevels = image->getNumMipmapLevels();

            int width  = inwidth;
            int height = inheight;
            //GL_texture_storage 4.2
            GLenum texStoragesizedInternalFormat = glTexStorage2D && (_borderWidth==0) ? selectSizedInternalFormat(image) : 0;

            if (texStoragesizedInternalFormat!=0)
            {
                if (getTextureTarget()==GL_TEXTURE_CUBE_MAP)
                {
                    if (target==GL_TEXTURE_CUBE_MAP_POSITIVE_X)
                    {
                        // only allocate on first face image
                        glTexStorage2D(GL_TEXTURE_CUBE_MAP, numMipmapLevels, texStoragesizedInternalFormat, width, height);
                    }
                }
                else
                {
                    glTexStorage2D(target, numMipmapLevels, texStoragesizedInternalFormat, width, height);
                }

                if( !compressed_image )
                {
                    for( GLsizei k = 0 ; k < numMipmapLevels  && (width || height) ;k++)
                    {

                        if (width == 0)
                            width = 1;
                        if (height == 0)
                            height = 1;

                        glTexSubImage2D( target, k,
                            0, 0,
                            width, height,
                            (GLenum)image->getPixelFormat(),
                            (GLenum)image->getDataType(),
                            dataPtr + image->getMipmapOffset(k));

                        width >>= 1;
                        height >>= 1;
                    }
                }
                else if (true)
                {
                    GLint blockSize,size;
                    for( GLsizei k = 0 ; k < numMipmapLevels  && (width || height) ;k++)
                    {
                        if (width == 0)
                            width = 1;
                        if (height == 0)
                            height = 1;

                        getCompressedSize(image->getInternalTextureFormat(), width, height, 1, blockSize,size);

                        //state.checkGLErrors("before glCompressedTexSubImage2D(");

                       glCompressedTexSubImage2D(target, k,
                            0, 0,
                            width, height,
                            (GLenum)image->getPixelFormat(),
                            size,
                            dataPtr + image->getMipmapOffset(k));

                        //state.checkGLErrors("after glCompressedTexSubImage2D(");

                        width >>= 1;
                        height >>= 1;
                    }
                }
            }
            else
            {
                if( !compressed_image )
                {
                    for( GLsizei k = 0 ; k < numMipmapLevels  && (width || height) ;k++)
                    {

                        if (width == 0)
                            width = 1;
                        if (height == 0)
                            height = 1;

                        glTexImage2D( target, k, _internalFormat,
                             width, height, _borderWidth,
                            (GLenum)image->getPixelFormat(),
                            (GLenum)image->getDataType(),
                            dataPtr + image->getMipmapOffset(k));

                        width >>= 1;
                        height >>= 1;
                    }
                }
                else if (true)
                {
                    GLint blockSize, size;

                    for( GLsizei k = 0 ; k < numMipmapLevels  && (width || height) ;k++)
                    {
                        if (width == 0)
                            width = 1;
                        if (height == 0)
                            height = 1;

                        getCompressedSize(_internalFormat, width, height, 1, blockSize,size);

                        glCompressedTexImage2D(target, k, _internalFormat,
                                                           width, height, _borderWidth,
                                                           size, dataPtr + image->getMipmapOffset(k));

                        width >>= 1;
                        height >>= 1;
                    }
                }
            }
        }
        else
        {
 
        }

    }

    if (pbo)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  
    }

 

    if (needImageRescale)
    { 
    }

    if (useClientStorage)
    {
 
    }
}
 

void Texture::applyTexImage2D_subload(State& state, GLenum target, const Image* image, GLsizei inwidth, GLsizei inheight, GLint inInternalFormat, GLint numMipmapLevels) const
{
    // if we don't have a valid image we can't create a texture!
    if (!image || !image->data())
        return;

    // image size has changed so we have to re-load the image from scratch.
    if (image->s()!=inwidth || image->t()!=inheight || image->getInternalTextureFormat()!=inInternalFormat )
    {
        applyTexImage2D_load(state, target, image, inwidth, inheight,numMipmapLevels);
        return;
    }
    // else image size the same as when loaded so we can go ahead and subload

    // If the texture's internal format is a compressed type, then the
    // user is requesting that the graphics card compress the image if it's
    // not already compressed. However, if the image is not a multiple of
    // four in each dimension the subsequent calls to glTexSubImage* will
    // fail. Revert to uncompressed format in this case.
    if (isCompressedInternalFormat(_internalFormat) &&
        (((inwidth >> 2) << 2) != inwidth ||
         ((inheight >> 2) << 2) != inheight))
    {
        applyTexImage2D_load(state, target, image, inwidth, inheight, numMipmapLevels);
        return;
    }

 


    // get the contextID (user defined ID of 0 upwards) for the
    // current OpenGL context.
    const unsigned int contextID = 0;
 
    // select the internalFormat required for the texture.
    bool compressed_image = isCompressedInternalFormat((GLenum)image->getPixelFormat());

    glPixelStorei(GL_UNPACK_ALIGNMENT,image->getPacking());
    unsigned int rowLength = image->getRowLength();

    unsigned char* dataPtr = (unsigned char*)image->data();

    bool needImageRescale = inwidth!=image->s() || inheight!=image->t();
    if (needImageRescale)
    { 
    }


    bool mipmappingRequired = _min_filter != LINEAR && _min_filter != NEAREST;
    bool useHardwareMipMapGeneration = mipmappingRequired && (!image->isMipmap() );
    bool useGluBuildMipMaps = mipmappingRequired && (!useHardwareMipMapGeneration && !image->isMipmap());

    GLuint pbo = 1; // image->getOrCreateGLBufferObject(contextID);
    if (pbo && !needImageRescale && !useGluBuildMipMaps)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
        ///dataPtr = reinterpret_cast<unsigned char*>(pbo->getOffset(image->getBufferIndex()));
        rowLength = 0;
 
    }
    else
    {
        pbo = 0;
    }
#ifndef BT_USE_EGL
    glPixelStorei(GL_UNPACK_ROW_LENGTH,rowLength);
#endif
    if( !mipmappingRequired || useHardwareMipMapGeneration)
    {

        GenerateMipmapMode mipmapResult = mipmapBeforeTexImage(state);

        if (!compressed_image)
        {
            glTexSubImage2D( target, 0,
                0, 0,
                inwidth, inheight,
                (GLenum)image->getPixelFormat(),
                (GLenum)image->getDataType(),
                dataPtr);
        }
        else if (true)
        {
            GLint blockSize,size;
            getCompressedSize(image->getInternalTextureFormat(), inwidth, inheight, 1, blockSize,size);

            glCompressedTexSubImage2D(target, 0,
                0,0,
                inwidth, inheight,
                (GLenum)image->getPixelFormat(),
                size,
                dataPtr);
        }

        mipmapAfterTexImage(state, mipmapResult);
    }
    else
    {
        if (image->isMipmap())
        {
            numMipmapLevels = image->getNumMipmapLevels();

            int width  = inwidth;
            int height = inheight;

            if( !compressed_image )
            {
                for( GLsizei k = 0 ; k < numMipmapLevels  && (width || height) ;k++)
                {

                    if (width == 0)
                        width = 1;
                    if (height == 0)
                        height = 1;

                    glTexSubImage2D( target, k,
                        0, 0,
                        width, height,
                        (GLenum)image->getPixelFormat(),
                        (GLenum)image->getDataType(),
                        dataPtr + image->getMipmapOffset(k));

                    width >>= 1;
                    height >>= 1;
                }
            }
            else if (true)
            {
                GLint blockSize,size;
                for( GLsizei k = 0 ; k < numMipmapLevels  && (width || height) ;k++)
                {
                    if (width == 0)
                        width = 1;
                    if (height == 0)
                        height = 1;

                    getCompressedSize(image->getInternalTextureFormat(), width, height, 1, blockSize,size);

                    //state.checkGLErrors("before glCompressedTexSubImage2D(");

                    glCompressedTexSubImage2D(target, k,
                                                       0, 0,
                                                       width, height,
                                                       (GLenum)image->getPixelFormat(),
                                                       size,
                                                       dataPtr + image->getMipmapOffset(k));

                    //state.checkGLErrors("after glCompressedTexSubImage2D(");

                    width >>= 1;
                    height >>= 1;
                }

            }
        }
        else
        {
            ////OSG_WARN<<"Warning:: cannot subload mip mapped texture from non mipmapped image."<<std::endl;
            applyTexImage2D_load(state, target, image, inwidth, inheight,numMipmapLevels);
        }
    }

    if (pbo)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
 

    if (needImageRescale)
    { 
    }
}
 
Texture::GenerateMipmapMode Texture::mipmapBeforeTexImage(const State& state) const
{
 
    {
#if 1
        return GENERATE_MIPMAP;
#else
         
        return GENERATE_MIPMAP_TEX_PARAMETER;
#endif
    }
    return GENERATE_MIPMAP_NONE;
}

void Texture::mipmapAfterTexImage(State& state, GenerateMipmapMode beforeResult) const
{
    switch (beforeResult)
    {
        case GENERATE_MIPMAP:
        {
            unsigned int contextID = 0;
            TextureObject* textureObject = getTextureObject(contextID);
            if (textureObject)
            {
           
                glGenerateMipmap(textureObject->target());
            }
            break;
        }
        case GENERATE_MIPMAP_TEX_PARAMETER:
            glTexParameteri(getTextureTarget(), GL_GENERATE_MIPMAP, GL_FALSE);
            break;
        case GENERATE_MIPMAP_NONE:
            break;
    }
}

void Texture::generateMipmap(State& state) const
{
    const unsigned int contextID = 0;

    // get the texture object for the current contextID.
    TextureObject* textureObject = getTextureObject(contextID);

    // if not initialized before, then do nothing
    if (textureObject == NULL) return;
     
    // if internal format does not provide automatic mipmap generation, then do manual allocation
    if (_internalFormatType == SIGNED_INTEGER || _internalFormatType == UNSIGNED_INTEGER)
    {
        allocateMipmap(state);
        return;
    }

    // get fbo extension which provides us with the glGenerateMipmapEXT function
 

    // FrameBufferObjects are required for glGenerateMipmap
    if (1)
    {
        textureObject->bind();
        glGenerateMipmap(textureObject->target());

        // inform state that this texture is the current one bound.
     

    // if the function is not supported, then do manual allocation
    }else
    {
        allocateMipmap(state);
    }

}
 
