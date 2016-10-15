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

#ifndef DAGON_AUDIO_H_
#define DAGON_AUDIO_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <string>

#include "Config.h"
#include "Platform.h"

#ifdef DAGON_MAC
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <SDL2/SDL_mutex.h>

#include "Asset.h"
#include "Defines.h"
#include "Object.h"
#include "Geometry.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////

class Config;
class Log;

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

enum AudioBufferState {
  kAudioStreamEOF = -1,
  kAudioStreamError = -2,
  kAudioGenericError = -3,
  kAudioStreamOK = 1
};

enum AudioStates {
  kAudioInitial,
  kAudioPlaying,
  kAudioPaused,
  kAudioStopped
};

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Audio : public Object {
  friend class AudioManager;

public:
  Audio(const std::string& audioName);
  ~Audio();
  
  // Checks
  bool doesAutoplay();
  bool isLoopable();
  bool isPlaying();
  bool isVarying();
  
  // Gets
  double cursor(); // For match function
  int state();
  AssetID_t filename() const;
  std::string audioName() const;
  
  // Sets
  void setAutoplay(bool autoplay);
  void setLoopable(bool loopable);
  void setPosition(unsigned int face, Point origin);
  void setVarying(bool varying);
  
  // State changes
  void match(Audio* audioToMatch);
  void play();
  void pause();
  void stop();
  void update();
  
private:
  Config& config;
  Log& log;
  
  Audio* _matchedAudio;
  SDL_mutex* _mutex;

  std::string _audioName;
  AssetID_t _filename;
  size_t _dataRead;
  
  bool _doesAutoplay;
  bool _isLoaded;
  bool _isLoopable;
  bool _isMatched;
  bool _isVarying;
  int _state;
  
  ALuint _alBuffers[kMaxAudioBuffers];
  ALenum _alFormat;
  ALuint _alSource;
  int _channels;
  ALsizei _rate;
  
  ov_callbacks _oggCallbacks;
  OggVorbis_File _oggStream;
  
  // Private methods
  void _load();
  void _unload();
  int _fillBuffer(ALuint* buffer);
  void _emptyBuffers();
  std::string _randomizeFile(const std::string &fileName);
  ALboolean _verifyError(const std::string &operation);
  
  // Callbacks for Vorbisfile library
  static std::size_t _oggRead(void* ptr, std::size_t size,
                         std::size_t nmemb, void* datasource);
  static int _oggSeek(void* datasource, ogg_int64_t offset, int whence);
  static int _oggClose(void* datasource);
  static long _oggTell(void* datasource);
  
  Audio(const Audio&);
  void operator=(const Audio&);
};
  
}

#endif // DAGON_AUDIO_H_
