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

#ifndef DAGON_TEXTURE_H_
#define DAGON_TEXTURE_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>

#include <GL/glew.h>
#include <SDL2/SDL_mutex.h>

#include "Object.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// TODO: We should convert all short vars to int for better speed here,
// also read new defines.

typedef struct {
  char name[80];
  short width;
  short height;
  short compressionLevel; // 0: None, 1: GL only, 2: GL & zlib
  short numTextures;
} TEXMainHeader;

typedef struct {
  short cubePosition;
  short depth;
  int size;
  int format;
} TEXSubHeader;

class Config;
class Log;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Texture : public Object {
 public:
  Texture();
  Texture(int withWidth, int andHeight, int andDepth);
  ~Texture();
  
  // Checks
  bool hasResource();
  bool isLoaded();
  
  // Gets
  int depth();
  int indexInBundle();
  int height();
  std::string resource();
  unsigned int usageCount();
  int width();
  
  // Sets
  void increaseUsageCount();
  void setIndexInBundle(int index);
  void setResource(std::string fromFileName);
  
  // State changes
  void bind();
  void clear();
  void load();
  void loadBitmap(); // Temporary to test preloader
  
  // Textures loaded from memory are not managed
  void loadFromMemory(const unsigned char* dataToLoad, long size);
  void loadRawData(const unsigned char* dataToLoad,
                   int withWidth, int andHeight);
  void saveToFile(std::string fileName);
  void unload();
  
 private:
  Config& config;
  Log& log;
  
  GLubyte* _bitmap;
  unsigned int _compressionLevel;
  GLint _depth;
  bool _hasResource;
  GLint _height;
  GLuint _ident;
  int _indexInBundle;
  bool _isBitmapLoaded;
  bool _isLoaded;
  unsigned int _usageCount; // Used to keep track of the most used textures
  GLint _width;
  
  SDL_mutex* _mutex;
  // Eventually all file management will be handled by a ResourceManager object
  std::string _resource;
  
  Texture(const Texture&);
  void operator=(const Texture&);
};
  
}

#endif // DAGON_TEXTURE_H_
