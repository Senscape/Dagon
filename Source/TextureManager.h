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

#ifndef DG_TEXTUREMANAGER_H
#define DG_TEXTUREMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Platform.h"
#include "Texture.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// TODO: This class should be a singleton

// This is temporary until we actually test how much memory is available.
// NOT accurate! It's only a reference value since textures are flushed
// before the next switch.
#define kMaxActiveTextures 18

class Config;
class Log;
class Node;
class Room;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class TextureManager {
  Config& config;
  Log& log;
  
  std::vector<Texture*> _arrayOfActiveTextures;
  std::vector<Texture*> _arrayOfTextures;
  
  Room* _roomToPreload;
  
  TextureManager();
  TextureManager(TextureManager const&);
  TextureManager& operator=(TextureManager const&);
  ~TextureManager();
  
public:
  static TextureManager& instance() {
    static TextureManager textureManager;
    return textureManager;
  }
  
  void appendTextureToBundle(const char* nameOfBundle, Texture* textureToAppend);
  void createBundle(const char* nameOfBundle);
  int itemsInBundle(const char* nameOfBundle);
  void flush();
  void init();
  void registerTexture(Texture* target);
  void requestBundle(Node* forNode);
  void requestTexture(Texture* target);
  void setRoomToPreload(Room* theRoom);
  bool updatePreloader();
};

#endif // DG_TEXTUREMANAGER_H
