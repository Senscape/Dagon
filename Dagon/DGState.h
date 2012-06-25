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

#ifndef DG_STATE_H
#define DG_STATE_H

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// Default state is node unless a menu is specified

enum DGGlobalStates {
	DGStateCutscene,
	DGStateInventory,
    DGStateLookAt,
	DGStateMenu,
	DGStateNode,
    DGStateSleep,
    DGStateSplash,
	DGStateVideoSync,
	DGStateZoom
};

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGState {
    int _current;
    int _previous;
    
public:
    DGState();
    ~DGState();
    
    int current();
    int previous();
    void set(int theState);    
    void setPrevious();
};

#endif // DG_STATE_H
