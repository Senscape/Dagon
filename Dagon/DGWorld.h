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

#ifndef DG_WORLD_H
#define DG_WORLD_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <vector>
#include "DGRoom.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGWorld {
    std::vector<DGRoom*> _arrayOfRooms;
    DGRoom* _currentRoom;
    
public:
    DGWorld();
    ~DGWorld();
    
    // Checks
    
    bool hasRooms();
    
    // Gets
    
    DGRoom* currentRoom();    

    // State changes
    
    DGRoom* addRoom(DGRoom* aRoom);
    bool switchTo(DGRoom* theRoom);
};

#endif // DG_WORLD_H
