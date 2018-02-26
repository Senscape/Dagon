////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_TEXTUREMANAGER_H_
#define DAGON_TEXTUREMANAGER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Platform.h"
#include "Texture.h"

#include <queue>
#include <SDL2/SDL_thread.h>

namespace dagon {

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

// This temporary macro is used to generate filenames
#define mkstr(a) # a
#define in_between(a) mkstr(a)

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class TextureManager {
  Config& config;
  Log& log;

  std::vector<Texture*> _arrayOfTextures;

  std::vector<Node*> _preloadedNodes;
  std::queue<std::pair<Texture*, Node*>> _texturesToPreload;
  SDL_mutex* _queueLock;
  SDL_cond* _newJobCond;
  SDL_Thread* _preloaderThread;

  TextureManager();
  TextureManager(TextureManager const&);
  TextureManager& operator=(TextureManager const&);
  ~TextureManager();

  static int runPreloader(void* ptr);
  void registerPreloadedNode(Node* theNode);

public:
  static TextureManager& instance() {
    static TextureManager textureManager;
    return textureManager;
  }
  
  void init();
  void terminate();
  void registerTexture(Texture* target);
  void requestBundle(Node* forNode);
  void setNodesToPreload(const std::vector<Node*>& theNodes);
  void flushPreloader();
};
  
}

#endif // DAGON_TEXTUREMANAGER_H_
