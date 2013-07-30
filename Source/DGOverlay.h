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
    Point _position;
    
public:
    DGOverlay();
    ~DGOverlay();
    
    // Checks
    
    bool hasButtons();
    bool hasImages();
    Point position();
    
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
