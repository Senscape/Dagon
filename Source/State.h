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

#ifndef DG_STATE_H
#define DG_STATE_H

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// Default state is node unless a menu is specified

enum DGGlobalStates {
  StateCutscene,
  StateInventory,
  StateLookAt,
  StateMenu,
  StateNode,
  StateSleep,
  StateSplash,
  StateVideoSync,
  StateZoom
};

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class State {
  int _current;
  int _previous;
  
public:
  State();
  ~State();
  
  int current();
  int previous();
  void set(int theState);
  void setPrevious();
};

#endif // DG_STATE_H
