////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2013 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <fstream>

#include "Config.h"
#include "Language.h"
#include "Log.h"
#include "Texture.h"
#include "stb_image.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////

char TEXIdent[] = "KS_TEX"; // We keep this one for backward compatibility

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Texture::Texture() :
config(Config::instance()),
log(Log::instance())
{
  _hasResource = false;
  _isBitmapLoaded = false;
  _isLoaded = false;
  _usageCount = 0;
  _compressionLevel = config.texCompression;
  this->setType(kObjectTexture);
  _mutex = SDL_CreateMutex();
  if (!_mutex)
    log.error(kModTexture, "%s", kString18001);
}

Texture::Texture(int withWidth, int andHeight, int andDepth) :
config(Config::instance()),
log(Log::instance())
{
  if (!withWidth)
    withWidth = kDefTexSize;
  if (!andHeight)
    andHeight = kDefTexSize;
  
  int comp;
  if (!andDepth) {
    comp = 3; // RGB
  } else {
    comp = andDepth / 8; // Whatever was requested
  }
  
  _width = withWidth;
  _height = andHeight;
  _depth = andDepth;
  _bitmap = new GLubyte[_width * _height * comp](); // Zero all bits
  glGenTextures(1, &_ident);
  glBindTexture(GL_TEXTURE_2D, _ident);
  glTexImage2D(GL_TEXTURE_2D, 0, comp, _width, _height,
               0, GL_RGB, GL_UNSIGNED_BYTE, _bitmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  delete[] _bitmap;
  
  // The texture doesn't require a resource, so we make it clear
  _hasResource = true;
  _isLoaded = true;
  // Since the texture will be loaded only once, we note this
  _usageCount = 1;
  _compressionLevel = config.texCompression;
  this->setType(kObjectTexture);
  _mutex = SDL_CreateMutex();
  if (!_mutex)
    log.error(kModTexture, "%s", kString18001);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Texture::~Texture() {
  this->unload();
  SDL_DestroyMutex(_mutex);
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Texture::hasResource() {
  return _hasResource;
}

bool Texture::isLoaded() {
  return _isLoaded;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

int Texture::depth() {
  return _depth;
}

int Texture::indexInBundle() {
  return _indexInBundle;
}

int Texture::height() {
  return _height;
}

std::string Texture::resource() {
  return _resource;
}

unsigned int Texture::usageCount() {
  return _usageCount;
}

int Texture::width() {
  return _width;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Texture::increaseUsageCount() {
  // We only keep track of the usage count if the texture is loaded
  if (_isLoaded)
    _usageCount++;
}

void Texture::setIndexInBundle(int index) {
  _indexInBundle = index;
}

void Texture::setResource(std::string fromFileName) {
  _resource = fromFileName;
  _hasResource = true;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void Texture::bind() {
  if (SDL_LockMutex(_mutex) == 0) {
    if (_isLoaded)
      glBindTexture(GL_TEXTURE_2D, _ident);
    SDL_UnlockMutex(_mutex);
  } else {
    log.error(kModTexture, "%s", kString18002);
  }
}

void Texture::clear() {
  if (SDL_LockMutex(_mutex) == 0) {
    _bitmap = new GLubyte[_width * _height * _depth];
    glBindTexture(GL_TEXTURE_2D, _ident);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height,
                    GL_RGB, GL_UNSIGNED_BYTE, _bitmap);
    delete[] _bitmap;
    SDL_UnlockMutex(_mutex);
  } else {
    log.error(kModTexture, "%s", kString18002);
  }
}

void Texture::load() {
  if (SDL_LockMutex(_mutex) == 0) {
    if (!_isLoaded) {
      if (!_hasResource) {
        log.error(kModTexture, "%s: %s", kString10005, this->name().c_str());
      }
      
      FILE* fh = fopen(_resource.c_str(), "rb");
      if (fh != NULL) {
        char magic[10]; // Used to identity file types
        if (fread(&magic, sizeof(magic), 1, fh) == 0) {
          // Couldn't read magic number
          log.error(kModTexture, "%s: %s", kString10003, _resource.c_str());
        }
        
        if (memcmp(TEXIdent, &magic, 7) == 0) { // Handle our own TEX format
          TEXMainHeader header;
          TEXSubHeader subheader;
          
          // Read the main header
          fseek(fh, 8, SEEK_SET); // Skip identifier
          fread(&header, 1, sizeof(header), fh);
          _width = static_cast<GLint>(header.width);
          _height = static_cast<GLint>(header.height);
          
          // Skip subheaders based on the index
          if (_indexInBundle) {
            for (int i = 0; i < _indexInBundle; i++) {
              fread(&subheader, 1, sizeof(subheader), fh);
              fseek(fh, sizeof(char) * subheader.size, SEEK_CUR);
            }
          }
          
          // Read the subheader
          fread(&subheader, 1, sizeof(subheader), fh);
          _depth = static_cast<GLuint>(subheader.depth);
          GLint size = static_cast<GLint>(subheader.size);
          GLint internalFormat = static_cast<GLint>(subheader.format);
          
          // Get the bitmap
          _bitmap = new GLubyte[size];
          fread(_bitmap, 1, sizeof(GLubyte) * size, fh);
          
          glGenTextures(1, &_ident);
          glBindTexture(GL_TEXTURE_2D, _ident);
          
          // Detect the compression level
          if (header.compressionLevel) {
            GLint compressed;
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
                                   _width, _height, 0, size, _bitmap);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED,
                                     &compressed);
            if (compressed == GL_TRUE) {
              _isLoaded = true;
            } else {
              log.error(kModTexture, "%s: %s", kString10003, fh);
            }
          } else {
            // Note that we only support RGB textures
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height,
                         0, GL_RGB, GL_UNSIGNED_BYTE, _bitmap);
            _isLoaded = true;
          }
          
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
          delete[] _bitmap;
        } else { // Let stb_image load the texture
          if (!_isBitmapLoaded) {
            fseek(fh, 0, SEEK_SET);
            int x, y, comp;
            _bitmap = static_cast<GLubyte*>(stbi_load_from_file(fh, &x, &y,
                                                                &comp,
                                                                STBI_default));
            _width = x;
            _height = y;
            _depth = comp;
          }
          if (_bitmap) {
            GLint format = 0, internalFormat = 0;
            switch (_depth) {
              case STBI_grey: {
                format = GL_LUMINANCE;
                if (_compressionLevel) {
                  internalFormat = GL_COMPRESSED_LUMINANCE;
                } else {
                  internalFormat = GL_LUMINANCE;
                }
                break;
              }
              case STBI_grey_alpha: {
                format = GL_LUMINANCE_ALPHA;
                if (_compressionLevel) {
                  internalFormat = GL_COMPRESSED_LUMINANCE_ALPHA;
                } else {
                  internalFormat = GL_LUMINANCE_ALPHA;
                }
                break;
              }
              case STBI_rgb: {
                format = GL_RGB;
                if (_compressionLevel) {
                  internalFormat = GL_COMPRESSED_RGB;
                } else {
                  internalFormat = GL_RGB;
                }
                break;
              }
              case STBI_rgb_alpha: {
                format = GL_RGBA;
                if (_compressionLevel) {
                  internalFormat = GL_COMPRESSED_RGBA;
                } else {
                  internalFormat = GL_RGBA;
                }
                break;
              }
              default: {
                log.warning(kModTexture, "%s: (%s) %d", kString10004,
                            _resource.c_str(), _depth);
                break;
              }
            }
            glGenTextures(1, &_ident);
            glBindTexture(GL_TEXTURE_2D, _ident);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height,
                         0, format, GL_UNSIGNED_BYTE, _bitmap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            free(_bitmap);
            _isBitmapLoaded = false;
            _isLoaded = true;
          } else {
            // Nothing loaded
            log.error(kModTexture, "%s: (%s) %s", kString10002,
                      _resource.c_str(), stbi_failure_reason());
          }
        }
        fclose(fh);
      } else {
        // File not found
        log.error(kModTexture, "%s: %s", kString10001, _resource.c_str());
      }
    }
    SDL_UnlockMutex(_mutex);
  } else {
    log.error(kModTexture, "%s", kString18002);
  }
}

void Texture::loadBitmap() {
  FILE* fh = fopen(_resource.c_str(), "rb");
  if (fh != NULL) {
    int x, y, comp;
    _bitmap = static_cast<GLubyte*>(stbi_load_from_file(fh, &x, &y,
                                                        &comp, STBI_default));
    _width = x;
    _height = y;
    _depth = comp;
    _isBitmapLoaded = true;
  }
}

void Texture::loadFromMemory(const unsigned char* dataToLoad, long size) {
  if (!_isLoaded) {
    int x, y, comp;
    _bitmap = static_cast<GLubyte*>(stbi_load_from_memory(dataToLoad, (int)size,
                                                          &x, &y, &comp,
                                                          STBI_default));
    if (_bitmap) {
      _width = x;
      _height = y;
      _depth = comp;
      
      GLint format = 0, internalFormat = 0;
      switch (comp) {
        case STBI_grey: {
          format = GL_LUMINANCE;
          if (_compressionLevel) {
            internalFormat = GL_COMPRESSED_LUMINANCE;
          } else {
            internalFormat = GL_LUMINANCE;
          }
          break;
        }
        case STBI_grey_alpha: {
          format = GL_LUMINANCE_ALPHA;
          if (_compressionLevel) {
            internalFormat = GL_COMPRESSED_LUMINANCE_ALPHA;
          } else {
            internalFormat = GL_LUMINANCE_ALPHA;
          }
          break;
        }
        case STBI_rgb: {
          format = GL_RGB;
          if (_compressionLevel) {
            internalFormat = GL_COMPRESSED_RGB;
          } else {
            internalFormat = GL_RGB;
          }
          break;
        }
        case STBI_rgb_alpha: {
          format = GL_RGBA;
          if (_compressionLevel) {
            internalFormat = GL_COMPRESSED_RGBA;
          } else {
            internalFormat = GL_RGBA;
          }
          break;
        }
        default: {
          log.warning(kModTexture, "%s: %d", kString10004, comp);
          break;
        }
      }
      glGenTextures(1, &_ident);
      glBindTexture(GL_TEXTURE_2D, _ident);
      glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height,
                   0, format, GL_UNSIGNED_BYTE, _bitmap);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      _isLoaded = true;
      free(_bitmap);
    } else {
      // Nothing loaded
      log.error(kModTexture, "%s: %s", kString10002, stbi_failure_reason());
    }
  }
}

void Texture::loadRawData(const unsigned char* dataToLoad,
                          int withWidth, int andHeight) {
  // Mostly useful to load frames from Video.
  // Note it defaults to inverted RGB.
  if (!_isLoaded) {
    glGenTextures(1, &_ident);
    glBindTexture(GL_TEXTURE_2D, _ident);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, withWidth, andHeight,
                 0, GL_BGR, GL_UNSIGNED_BYTE, dataToLoad);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    _width = withWidth;
    _height = andHeight;
    _depth = 24;
    _isLoaded = true;
  } else {
    glBindTexture(GL_TEXTURE_2D, _ident);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, withWidth, andHeight,
                    GL_BGR, GL_UNSIGNED_BYTE, dataToLoad);
  }
}

void Texture::saveToFile(std::string fileName){
  // NOTE: Always saves in TGA format
  if (_isLoaded) {
    fileName.append(".tga");
    
    FILE* fh = fopen(fileName.c_str(), "wb");
    if (fh == NULL)
      return;
    
    glBindTexture(GL_TEXTURE_2D, _ident);
    // We do this in case the texture wasn't loaded
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);
    _depth = 24; // Forced to 24-bits
    
    // The image header
    unsigned char header[18] = {0};
    header[2] = 2; // Uncompressed RGB
    header[12] = _width & 0xFF;
    header[13] = (_width >> 8) & 0xFF;
    header[14] = _height & 0xFF;
    header[15] = (_height >> 8) & 0xFF;
    header[16] = _depth; // bits per pixel
    fwrite(&header, 1, sizeof(header), fh);
    
    // The image
    int size = _width * _height * 3; // Three components (RGB)
    _bitmap = new GLubyte[size];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, _bitmap);
    fwrite(_bitmap, 1, sizeof(GLubyte) * size, fh);
    
    // Clean up
    fclose(fh);
    delete[] _bitmap;
  }
}

void Texture::unload() {
  if (_isLoaded) {
    glDeleteTextures(1, &_ident);
    _usageCount = 0;
    _isBitmapLoaded = false;
    _isLoaded = false;
  }
}
  
}
