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

#ifndef DAGON_STATE_H_
#define DAGON_STATE_H_

namespace dagon {

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
  
}

#endif // DAGON_STATE_H_
