////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2016 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_INTERNAL_AUDIO_H
#define DAGON_INTERNAL_AUDIO_H

#include "Audio.h"

namespace dagon {

class InternalAudio : public Audio {
public:
  InternalAudio(bool temp);

  bool isTemporary() const;
private:
  bool _isTemporary;
};

}

#endif // DAGON_INTERNAL_AUDIO_H
