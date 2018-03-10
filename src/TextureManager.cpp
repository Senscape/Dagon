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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Config.h"
#include "Log.h"
#include "Node.h"
#include "Spot.h"
#include "TextureManager.h"

#include <algorithm>
#include <cassert>

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

TextureManager::TextureManager() :
config(Config::instance()),
log(Log::instance())
{
  _queueLock = SDL_CreateMutex();
  if (!_queueLock) log.error(kModTexture, "%s", kString18001);
  _setLock = SDL_CreateMutex();
  if (!_setLock) log.error(kModTexture, "%s", kString18001);
  _newJobCond = SDL_CreateCond();
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

TextureManager::~TextureManager() {
  if (!_managedTextures.empty()) {
    auto it = _managedTextures.begin();
    
    while (it != _managedTextures.end()) {
      delete *it;
      ++it;
    }
  }

  SDL_DestroyCond(_newJobCond);
  SDL_DestroyMutex(_queueLock);
  SDL_DestroyMutex(_setLock);
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void TextureManager::init() {
  _preloaderThread = SDL_CreateThread(runPreloader, "TexturePreloader", this);
}

void TextureManager::terminate() {
  if (SDL_LockMutex(_queueLock) != 0) {
    log.error(kModTexture, "%s", kString18002);
    return;
  }

  { // Empty out the queue.
    std::queue<Texture*> emptyQueue;
    _texturesToPreload.swap(emptyQueue);
  }

  _texturesToPreload.push(NULL);
  SDL_CondSignal(_newJobCond);
  SDL_UnlockMutex(_queueLock);

  int returnVal;
  SDL_WaitThread(_preloaderThread, &returnVal);
}

void TextureManager::registerTexture(Texture* target) {
  // FIXME: If the script specifies a file with extension, we should
  // prioritize that and avoid doing any operations here.
  
  if (!target->hasResource()) {
    // We attempt loading the resource with automatically generated filenames
    char fileToLoad[kMaxFileLength];
    
    if (config.bundleEnabled) {
      // Attempt loading the bundle
      snprintf(fileToLoad, kMaxFileLength, "%s.%s", target->name().c_str(), config.texExtension().c_str());
      
    }
    else {
      // Attempt loading individual file with generated index and default extension
      snprintf(fileToLoad, kMaxFileLength, "%s%0" in_between(kFileSeqDigits) "d.%s", target->name().c_str(),
               target->indexInBundle() + kFileSeqStart, config.texExtension().c_str());
    }
    
    target->setResource(config.path(kPathResources, fileToLoad, kObjectNode).c_str());
  }

  if (SDL_LockMutex(_setLock) != 0) {
    log.error(kModTexture, "%s", kString18002);
    return;
  }

  _managedTextures.insert(target);
  SDL_UnlockMutex(_setLock);
  
  // Nothing to do for now
  // It's the responsibility of another module to generate the res path accordingly
}

void TextureManager::requestBundle(Node* forNode) {
  if (forNode->hasBundleName()) {
    for (int i = 0; i < 6; i++) {
      std::vector<int> arrayOfCoordinates;
      // We ensure the texture is properly stretched, so we take the default cube size
      // TODO: This setting should be obtained from the Config class
      int coords[] = {0, 0, kDefTexSize, 0, kDefTexSize, kDefTexSize, 0, kDefTexSize};
      unsigned arraySize = sizeof(coords) / sizeof(int);
      
      arrayOfCoordinates.assign(coords, coords + arraySize);
      Spot* spot = new Spot(arrayOfCoordinates, i, kSpotClass);
      Texture* texture = new Texture;
      
      spot->setTexture(texture);
      spot->texture()->setIndexInBundle(i);
      
      // In this case, the filename is generated from the name
      // of the texture
      spot->texture()->setName(forNode->bundleName().c_str());
      
      registerTexture(spot->texture());
      
      forNode->addSpot(spot);
    }
  }
  
  // Possibly raise an error if this fails
}

template<class OutputIt>
static void computeNodeTextures(const std::vector<Node*>& theNodes, OutputIt out) {
  for (Node* node : theNodes) {
    if (!node->hasSpots()) continue;

    node->beginIteratingSpots();
    do {
      Spot* spot = node->currentSpot();

      if (spot->hasTexture()) {
        *out = spot->texture();
        ++out;
      }
    } while (node->iterateSpots());
  }
}

void TextureManager::setNodesToPreload(const std::vector<Node*>& reachableNodes) {
  if (SDL_LockMutex(_queueLock) != 0) {
    log.error(kModTexture, "%s", kString18002);
    return;
  }

  assert(_texturesToPreload.empty());

  std::vector<Texture*> reachableTextures;
  computeNodeTextures(reachableNodes, std::back_inserter(reachableTextures));

  for (Texture* texture : reachableTextures) {
    if (!texture->isLoaded()) {
      log.info(kModTexture, "Scheduled preloading of %s", texture->resource().c_str());
      _texturesToPreload.push(texture);
    }
  }

  SDL_CondSignal(_newJobCond);
  SDL_UnlockMutex(_queueLock);
}

void TextureManager::flushPreloader() {
  log.info(kModTexture, "Flushing preloader...");

  if (SDL_LockMutex(_queueLock) != 0) {
    log.error(kModTexture, "%s", kString18002);
    return;
  }

  // Empty the queue by swapping with an empty queue.
  std::queue<Texture*> emptyQueue;
  _texturesToPreload.swap(emptyQueue);

  SDL_UnlockMutex(_queueLock);
}

void TextureManager::flush(const std::vector<Node*>& reachableNodes) {
  std::set<Texture*> reachableTextures;
  computeNodeTextures(reachableNodes,
                      std::inserter(reachableTextures, reachableTextures.begin()));

  if (SDL_LockMutex(_setLock) != 0) {
    log.error(kModTexture, "%s", kString18002);
    return;
  }

  std::set<Texture*> texturesToUnload;
  std::set_difference(_managedTextures.begin(), _managedTextures.end(),
                      reachableTextures.begin(), reachableTextures.end(),
                      std::inserter(texturesToUnload, texturesToUnload.begin()));

  SDL_UnlockMutex(_setLock);

  for (Texture* texture : texturesToUnload) {
    bool unloaded = true;

    if (texture->isLoaded())
      texture->unload();
    else if (texture->isBitmapLoaded())
      texture->unloadBitmap();
    else
      unloaded = false;

    if (unloaded) {
      log.info(kModTexture, "Unloading %s", texture->resource().c_str());
    }
  }
}

int TextureManager::runPreloader(void* ptr) {
  TextureManager* _this = (TextureManager*)ptr;

  while (true) {
    if (SDL_LockMutex(_this->_queueLock) != 0) {
      _this->log.error(kModTexture, "%s", kString18002);
      continue;
    }

    while (_this->_texturesToPreload.empty()) {
      SDL_CondWait(_this->_newJobCond, _this->_queueLock);
    }

    Texture* job = _this->_texturesToPreload.front();
    _this->_texturesToPreload.pop();
    SDL_UnlockMutex(_this->_queueLock);

    if (!job) break;

    if (!job->isBitmapLoaded()) {
      _this->log.info(kModTexture, "Preloading %s", job->resource().c_str());
      job->loadBitmap();
      _this->registerTexture(job);
      _this->log.info(kModTexture, "Done preloading %s", job->resource().c_str());
    }
  }

  return 0;
}
  
}
