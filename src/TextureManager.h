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
#include <set>
#include <vector>
#include <SDL2/SDL_thread.h>

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

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

  std::set<Texture*> _managedTextures;
  SDL_mutex* _setLock;

  std::queue<Texture*> _texturesToPreload;
  SDL_mutex* _queueLock;
  SDL_cond* _newJobCond;
  SDL_Thread* _preloaderThread;

  TextureManager();
  TextureManager(TextureManager const&);
  TextureManager& operator=(TextureManager const&);
  ~TextureManager();

  static int runPreloader(void* ptr);
public:
  static TextureManager& instance() {
    static TextureManager textureManager;
    return textureManager;
  }
  
  void init();
  void terminate();
  void registerTexture(Texture* target);
  void requestBundle(Node* forNode);
  void setNodesToPreload(const std::vector<Node*>& reachableNodes);
  void flushPreloader();
  void flush(const std::vector<Node*>& reachableNodes);
};
  
}

#endif // DAGON_TEXTUREMANAGER_H_
