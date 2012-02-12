////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGConfig.h"
#include "DGLog.h"
#include "DGNode.h"
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

DGTextureManager::DGTextureManager() {
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGTextureManager::~DGTextureManager() {
    if (!_arrayOfTextures.empty()) {
        vector<DGTexture*>::iterator it;
        
        for (it = _arrayOfTextures.begin(); it < _arrayOfTextures.end(); it++) {
            delete *it;
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
    // and unloads the least used textures if necessary
    
    long texturesToUnload = _arrayOfActiveTextures.size() - DGMaxActiveTextures;

    while (texturesToUnload > 0) {
        _arrayOfActiveTextures.front()->unload();
        _arrayOfActiveTextures.erase(_arrayOfActiveTextures.begin());
        
        texturesToUnload--;
    }
}

void DGTextureManager::requestNewBundle(DGNode* forNode) {
    if (forNode->hasBundleName()) {
        for (int i = 0; i < 6; i++) {
            std::vector<int> arrayOfCoordinates;
            // We ensure the texture is properly stretched, so we take the default cube size
            // TODO: This setting should be obtained from the DGConfig class
            int coords[] = {0, 0, DGDefTexSize, 0, DGDefTexSize, DGDefTexSize, 0, DGDefTexSize};
            unsigned arraySize = sizeof(coords) / sizeof(int);
            
            arrayOfCoordinates.assign(coords, coords + arraySize);
            DGSpot* spot = new DGSpot(arrayOfCoordinates, i, DGSpotClass);
            DGTexture* texture = new DGTexture;
            
            spot->setTexture(texture);
            spot->texture()->setIndexInBundle(i);
            
            // In this case, the filename is generated from the name
            // of the texture
            spot->texture()->setName(forNode->bundleName());
            
            requestNewTexture(spot->texture());
            
            forNode->addSpot(spot);
        }
    }
    
    // Possibly raise an error if this fails
}

void DGTextureManager::requestNewTexture(DGTexture* target) {
    // FIXME: If the script specifies a file with extension, we should
    // prioritize that and avoid doing any operations here.
    
    if (!target->hasResource()) {
        // We attempt loading the resource with automatically generated filenames

        char fileToLoad[DGMaxFileLength];
        
        if (config->bundleEnabled) {
            // Attempt loading the bundle
            
            snprintf(fileToLoad, DGMaxFileLength, "%s.%s", target->name(), config->texExtension());
            
        }
        else {
            // Attempt loading individual file with generated index and default extension
            
            snprintf(fileToLoad, DGMaxFileLength, "%s%0" in_between(DGFileSeqDigits) "d.%s", target->name(),
                     target->indexInBundle() + DGFileSeqStart, config->texExtension());
        }
        
        target->setResource(config->path(DGPathRes, fileToLoad));
    }
    
    _arrayOfTextures.push_back(target);
    
    // Nothing to do for now
    // It's the responsibility of another module to generate the res path accordingly
}

void DGTextureManager::requireTextureToLoad(DGTexture* target) {
    if (!target->isLoaded()) {
        target->load();
        
        _arrayOfActiveTextures.push_back(target);
    }
    
    target->increaseUsageCount();
    
    sort(_arrayOfActiveTextures.begin(), _arrayOfActiveTextures.end(), DGTextureSort);
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

bool DGTextureSort(DGTexture* t1, DGTexture* t2) {
    return t1->usageCount() < t2->usageCount();
}
