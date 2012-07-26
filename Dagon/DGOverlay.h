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

#ifndef DG_OVERLAY_H
#define DG_OVERLAY_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class DGButton;
class DGImage;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGOverlay : public DGObject {
    std::vector<DGButton*> _arrayOfButtons;
    std::vector<DGImage*> _arrayOfImages;
    
    std::vector<DGButton*>::iterator _itButton;
    std::vector<DGButton*>::reverse_iterator _ritButton;
    std::vector<DGImage*>::iterator _itImage;
    
    bool _isIteratingBackwards;
    DGPoint _position;
    
public:
    DGOverlay();
    ~DGOverlay();
    
    // Checks
    
    bool hasButtons();
    bool hasImages();
    DGPoint position();
    
    // Gets
    
    DGButton* currentButton();
    DGImage* currentImage();
    
    // Sets
    
    void setPosition(int x, int y);
    
    // State changes
    
    DGButton* addButton(DGButton* aButton);
    DGImage* addImage(DGImage* anImage);    
    void beginIteratingButtons(bool iterateBackwards);
    void beginIteratingImages();
    void fadeIn();
    void fadeOut();
    bool iterateButtons();
    bool iterateImages();
    void move(int offsetX, int offsetY); // Applies move to every element
};

#endif // DG_OVERLAY_H
