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

#include "Config.h"
#include "Log.h"
#include "DGNode.h"
#include "DGRoom.h"
#include "DGSpot.h"
#include "DGTextureManager.h"

using namespace std;

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

bool DGTextureSort(DGTexture* t1, DGTexture* t2);

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGTextureManager::DGTextureManager() :
    config(Config::instance()),
    log(Log::instance())
{
    _roomToPreload = NULL;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGTextureManager::~DGTextureManager() {
    if (!_arrayOfTextures.empty()) {
        vector<DGTexture*>::iterator it;
        
        it = _arrayOfTextures.begin();
        
        while (it != _arrayOfTextures.end()) {
            delete *it;
            it++;
        }   
    }
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGTextureManager::appendTextureToBundle(const char* nameOfBundle, DGTexture* textureToAppend) {
    // This function will store individual textures to a bundle
}

void DGTextureManager::createBundle(const char* nameOfBundle) {
    // This function will create bundles to store textures
}

int DGTextureManager::itemsInBundle(const char* nameOfBundle) {
    // This one should return the number of textures in a bundle
    return 0;
}

void DGTextureManager::flush() {
    // This function is called every time a switch is performed
    // and unloads the least used textures when necessary
    
    long texturesToUnload = _arrayOfActiveTextures.size() - kMaxActiveTextures;

    while (texturesToUnload > 0) {
        _arrayOfActiveTextures.front()->unload();
        _arrayOfActiveTextures.erase(_arrayOfActiveTextures.begin());
        
        texturesToUnload--;
    }
}

void DGTextureManager::init() {
    _preloaderThread = thread([&](){
        chrono::milliseconds dura(1);
        while (DGTextureManager::instance().updatePreloader()) {
            this_thread::sleep_for(dura);
        }
    });
}

void DGTextureManager::registerTexture(DGTexture* target) {
    // FIXME: If the script specifies a file with extension, we should
    // prioritize that and avoid doing any operations here.
    
    if (!target->hasResource()) {
        // We attempt loading the resource with automatically generated filenames
        char fileToLoad[kMaxFileLength];
        
        if (config.bundleEnabled) {
            // Attempt loading the bundle
            snprintf(fileToLoad, kMaxFileLength, "%s.%s", target->name(), config.texExtension().c_str());
            
        }
        else {
            // Attempt loading individual file with generated index and default extension
            snprintf(fileToLoad, kMaxFileLength, "%s%0" in_between(kFileSeqDigits) "d.%s", target->name(),
                     target->indexInBundle() + kFileSeqStart, config.texExtension().c_str());
        }
        
        target->setResource(config.path(kPathResources, fileToLoad, DGObjectNode).c_str());
    }
    
    _arrayOfTextures.push_back(target);
    
    // Nothing to do for now
    // It's the responsibility of another module to generate the res path accordingly
}

void DGTextureManager::requestBundle(DGNode* forNode) {
    if (forNode->hasBundleName()) {
        for (int i = 0; i < 6; i++) {
            std::vector<int> arrayOfCoordinates;
            // We ensure the texture is properly stretched, so we take the default cube size
            // TODO: This setting should be obtained from the Config class
            int coords[] = {0, 0, kDefTexSize, 0, kDefTexSize, kDefTexSize, 0, kDefTexSize};
            unsigned arraySize = sizeof(coords) / sizeof(int);
            
            arrayOfCoordinates.assign(coords, coords + arraySize);
            DGSpot* spot = new DGSpot(arrayOfCoordinates, i, DGSpotClass);
            DGTexture* texture = new DGTexture;
            
            spot->setTexture(texture);
            spot->texture()->setIndexInBundle(i);
            
            // In this case, the filename is generated from the name
            // of the texture
            spot->texture()->setName(forNode->bundleName());
            
            registerTexture(spot->texture());
            
            forNode->addSpot(spot);
        }
    }
    
    // Possibly raise an error if this fails
}

void DGTextureManager::requestTexture(DGTexture* target) {
    if (!target->isLoaded()) {
        target->load();
        
        _arrayOfActiveTextures.push_back(target);
    }
    
    target->increaseUsageCount();
    
    sort(_arrayOfActiveTextures.begin(), _arrayOfActiveTextures.end(), DGTextureSort);
}

void DGTextureManager::setRoomToPreload(DGRoom* theRoom) {
    _roomToPreload = theRoom;
}

bool DGTextureManager::updatePreloader() {
    if (_roomToPreload) {
        if (_roomToPreload->hasNodes()) {
            _roomToPreload->beginIteratingNodes();
            do {
                DGNode* node = _roomToPreload->iterator();
                if (node->hasSpots()) {
                    node->beginIteratingSpots();
                    do {
                        DGSpot* spot = node->currentSpot();
                        
                        if (spot->hasTexture()) {
                            if (!spot->texture()->isLoaded()) {
                                spot->texture()->loadBitmap();
                                
                                //_arrayOfActiveTextures.push_back(spot->texture());
                            }
                        }
                    } while (node->iterateSpots());
                }
            } while (_roomToPreload->iterateNodes());
        }
    }
    
    return true;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

bool DGTextureSort(DGTexture* t1, DGTexture* t2) {
    return t1->usageCount() < t2->usageCount();
}
