////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_SPOT_H_
#define DAGON_SPOT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <stdint.h>
#include <vector>

#include "Action.h"
#include "Geometry.h"
#include "Colors.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum SpotFlags {
  kSpotAuto = 0x1,
  kSpotClass = 0x2,
  kSpotLoop = 0x4,
  kSpotSync = 0x8,
  kSpotUser = 0x10
};

class Audio;
class Texture;
class Video;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Spot : public Object {
 public:
  Spot(std::vector<int> withArrayOfCoordinates,
       unsigned int onFace, int withFlags);
  ~Spot();
  
  // Checks
  bool hasAction();
  bool hasAudio();
  bool hasColor();
  bool hasFlag(int theFlag);
  bool hasTexture();
  bool hasVideo();
  bool isPlaying();
  
  // Gets
  Action* action();
  Audio* audio();
  uint32_t color();
  std::vector<int> arrayOfCoordinates();
  unsigned int face();
  Point origin();
  Texture* texture();
  int vertexCount();
  Video* video();
  float volume();
  
  // Sets
  void setAction(Action* anAction);
  void setAudio(Audio* anAudio);
  void setColor(uint32_t theColor);
  void setOrigin(int x, int y);
  void setTexture(Texture* aTexture);
  void setVideo(Video* aVideo);
  void setVolume(float theVolume);
  
  // State changes
  void play();
  void resize(int width, int height);
  void stop();
  
 private:
  Action* _actionData;
  Audio* _attachedAudio;
  Texture* _attachedTexture;
  Video* _attachedVideo;
  
  std::vector<int> _arrayOfCoordinates;
  unsigned int _onFace;
  
  uint32_t _color;
  int _flags;
  bool _hasAction;
  bool _hasAudio;
  bool _hasColor;
  bool _hasTexture;
  bool _hasVideo; 
  bool _isPlaying;
  float _volume;
  int _xOrigin;
  int _yOrigin;
  int _zOrder; // For future use
  
  Spot(const Spot&);
  void operator=(const Spot&);
};
  
}

#endif // DAGON_SPOT_H_
