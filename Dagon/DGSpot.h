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

#ifndef DG_SPOT_H
#define DG_SPOT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAction.h"
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum DGSpotFlags {
    DGSpotAuto,
	DGSpotClass,
	DGSpotSync,    
	DGSpotUser
};

class DGAudio;
class DGTexture;
class DGVideo;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGSpot : public DGObject {
    int _color;
	int _flags;
    
	DGAction* _actionData;
    DGAudio* _attachedAudio;
    DGTexture* _attachedTexture;
	DGVideo* _attachedVideo;
	
	bool _isEnabled;
	bool _isPlaying;
	
	bool _hasAction;
	bool _hasAudio;
	bool _hasColor;
	bool _hasTexture;
	bool _hasVideo;
	
    std::vector<int> _arrayOfCoordinates;
	unsigned int _onFace;
	
	int _xOrigin;
	int _yOrigin;
	int _zOrder; // For future use
    
public:
    DGSpot(std::vector<int> withArrayOfCoordinates, unsigned int onFace, int withFlags);
    ~DGSpot();
    
    // Checks
    
    bool hasAction();
    bool hasAudio();
    bool hasColor();
    bool hasFlag(int theFlag);
    bool hasTexture();
    bool hasVideo();
    bool isEnabled();
    bool isPlaying();
    
    // Gets
    
    DGAction* action();
    DGAudio* audio();
    int color();
    std::vector<int> arrayOfCoordinates();
    unsigned int face();
    DGTexture* texture();
    DGVideo* video();
    
    // Sets
    
    void setAction(DGAction* anAction);
    void setAudio(DGAudio* anAudio);
    void setColor(int aColor);
    void setTexture(DGTexture* aTexture);
    void setVideo(DGVideo* aVideo);
    
    // State changes
    
    void disable();
    void enable();
    void play();
    void resize(std::vector<int> newArrayOfCoordinates);
    void stop();    
    void toggle();
};

#endif // DG_SPOT_H
