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
    std::vector<DGImage*>::iterator _itImage;
    
    bool _isVisible;
    DGPoint _position;
    
public:
    DGOverlay();
    ~DGOverlay();
    
    // Checks
    
    bool hasButtons();
    bool hasImages();
    bool isVisible();
    DGPoint position();
    
    // Gets
    
    DGButton* currentButton();
    DGImage* currentImage();
    
    // Sets
    
    void setPosition(int x, int y);
    
    // State changes
    
    DGButton* addButton(DGButton* aButton);
    DGImage* addImage(DGImage* anImage);    
    void beginIteratingButtons();
    void beginIteratingImages();    
    bool iterateButtons();
    bool iterateImages();
    void hide();
    void move(int offsetX, int offsetY); // Applies move to every element
    void show();
};

#endif // DG_OVERLAY_H
