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
#include "AudioAsset.h"

namespace dagon {

class InternalAudio : public Audio {
public:
  InternalAudio(const std::string& audioName, bool temp);

  bool isTemporary() const;
  void setAsset(const std::shared_ptr<AudioAsset>& asset);
private:
  bool _isTemporary;
  std::shared_ptr<AudioAsset> _asset;
};

}

#endif // DAGON_INTERNAL_AUDIO_H
