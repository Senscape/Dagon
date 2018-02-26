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
#include "Room.h"
#include "Spot.h"
#include "TextureManager.h"
#include "Control.h"

#include <algorithm>

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

TextureManager::TextureManager() :
config(Config::instance()),
log(Log::instance())
{
  _queueLock = SDL_CreateMutex();
  if (!_queueLock)
    log.error(kModTexture, "%s", kString18001);
  _newJobCond = SDL_CreateCond();
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

TextureManager::~TextureManager() {
  if (!_arrayOfTextures.empty()) {
    std::vector<Texture*>::iterator it;
    
    it = _arrayOfTextures.begin();
    
    while (it != _arrayOfTextures.end()) {
      delete *it;
      ++it;
    }
  }

  SDL_DestroyCond(_newJobCond);
  SDL_DestroyMutex(_queueLock);
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
    std::queue<std::pair<Texture*, Node*>> emptyQueue;
    _texturesToPreload.swap(emptyQueue);
  }

  _texturesToPreload.push({NULL, NULL});
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
  
  _arrayOfTextures.push_back(target);
  
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

void TextureManager::setNodesToPreload(const std::vector<Node*>& theNodes) {
  if (SDL_LockMutex(_queueLock) != 0) {
    log.error(kModTexture, "%s", kString18002);
    return;
  }

  assert(_texturesToPreload.empty());

  int numScheduledNodes = 0;
  for (Node* node : theNodes) {
    if (numScheduledNodes >= config.maxPreloadedNodes)
      break;

    auto it = std::find(_preloadedNodes.begin(), _preloadedNodes.end(), node);
    if (it != _preloadedNodes.end())
      continue;

    if (!node->hasSpots())
      continue;

    bool shouldPreloadNode = false;

    node->beginIteratingSpots();
    do {
      Spot* spot = node->currentSpot();

      if (spot->hasTexture()) {
	Texture *tex = spot->texture();
	if (!tex->isLoaded() && !tex->isBitmapLoaded()) {
	  log.info(kModTexture, "Scheduled preloading of %s", tex->resource().c_str());
	  _texturesToPreload.push({tex, node});
	  shouldPreloadNode = true;
	}
      }
    } while (node->iterateSpots());

    if (shouldPreloadNode)
      ++numScheduledNodes;
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
  std::queue<std::pair<Texture*, Node*>> emptyQueue;
  _texturesToPreload.swap(emptyQueue);

  SDL_UnlockMutex(_queueLock);
}

int TextureManager::runPreloader(void* ptr) {
  TextureManager* _this = (TextureManager*)ptr;

  Node* lastPreloadedNode = NULL;
  while (true) {
    if (SDL_LockMutex(_this->_queueLock) != 0) {
      _this->log.error(kModTexture, "%s", kString18002);
      continue;
    }

    while (_this->_texturesToPreload.empty()) {
      if (lastPreloadedNode) {
	_this->registerPreloadedNode(lastPreloadedNode);
	lastPreloadedNode = NULL;
      }

      SDL_CondWait(_this->_newJobCond, _this->_queueLock);
    }

    std::pair<Texture*, Node*> job = std::move(_this->_texturesToPreload.front());
    _this->_texturesToPreload.pop();
    SDL_UnlockMutex(_this->_queueLock);

    if (!job.first)
      break;

    if (lastPreloadedNode != job.second) {
      if (lastPreloadedNode)
	_this->registerPreloadedNode(lastPreloadedNode);

      lastPreloadedNode = job.second;
    }

    _this->log.info(kModTexture, "Preloading %s", job.first->resource().c_str());
    job.first->loadBitmap();
  }

  return 0;
}

void TextureManager::registerPreloadedNode(Node* theNode) {
  log.info(kModTexture, "Done preloading %s", theNode->name().c_str());

  auto it = std::find(_preloadedNodes.begin(), _preloadedNodes.end(), theNode);
  if (it != _preloadedNodes.end()) {
    return;
  }

  _preloadedNodes.push_back(theNode);
  int nodesToUnload = _preloadedNodes.size() - config.maxPreloadedNodes;

  while (nodesToUnload > 0) {
    auto nodeToUnload = std::find_if_not(_preloadedNodes.begin(), _preloadedNodes.end(),
					 [](Node* node) {
					   return node == Control::instance().currentNode();
					 });

    if (nodeToUnload != _preloadedNodes.end()) {
      Node* node = *nodeToUnload;

      if (node->hasSpots()) {
	node->beginIteratingSpots();

	do {
	  Spot* spot = node->currentSpot();

	  if (spot->hasTexture()) {
	    Texture* tex = spot->texture();

	    if (tex->isLoaded())
	      tex->unload();
	    else if (tex->isBitmapLoaded())
	      tex->unloadBitmap();
	  }
	} while (node->iterateSpots());
      }

      _preloadedNodes.erase(nodeToUnload);
      nodesToUnload--;
      log.info(kModTexture, "Unloaded node %s", node->name().c_str());
    }
  }

  assert((int)_preloadedNodes.size() <= config.maxPreloadedNodes);
}
  
}
