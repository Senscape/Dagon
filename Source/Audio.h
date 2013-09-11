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

#ifndef DAGON_AUDIO_H_
#define DAGON_AUDIO_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <mutex>
#include <string>

#include <AL/al.h>
#include <AL/alc.h>

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "Defines.h"
#include "Object.h"
#include "Geometry.h"

////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////

class Config;
class Log;

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// TODO: Allow to configure the number of buffers
#define kAudioBuffers 2

enum AudioStates {
  kAudioInitial,
  kAudioPlaying,
  kAudioPaused,
  kAudioStopped
};

struct Resource {
  int index;
  std::string name;
  char* data;
  size_t dataRead;
  size_t dataSize;
};

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class Audio : public Object {
 public:
  Audio();
  ~Audio();
  
  // Checks
  bool isLoaded();
  bool isLoopable();
  bool isPlaying();
  
  // Gets
  double cursor(); // For match function
  int state();
  
  // Sets
  void setLoopable(bool loopable);
  void setPosition(unsigned int face, Point origin);
  void setResource(std::string fileName);
    
  // State changes
  void load();
  void match(Audio* audioToMatch);
  void play();
  void pause();
  void stop();
  void unload();
  void update();
  
 private:
  Config& config;
  Log& log;
  
  Audio* _matchedAudio;
  std::mutex _mutex;
 
  // Eventually all file management will be handled by a separate class
  Resource _resource;
  
  bool _isLoaded = false;
  bool _isLoopable = false;
  bool _isMatched = false;
  int _state = kAudioInitial;
  
  ALuint _alBuffers[kAudioBuffers];
	ALenum _alFormat;
  ALuint _alSource;
  int _channels;
  ALsizei _rate;
  
  ov_callbacks _oggCallbacks;
  OggVorbis_File _oggStream;
  
  // Private methods
  bool _fillBuffer(ALuint* buffer);
  void _emptyBuffers();
  std::string _randomizeFile(const std::string &fileName);
  ALboolean _verifyError(const std::string &operation);
  
  // Callbacks for Vorbisfile library
  static size_t _oggRead(void* ptr, size_t size,
                         size_t nmemb, void* datasource);
  static int _oggSeek(void* datasource, ogg_int64_t offset, int whence);
  static int _oggClose(void* datasource);
  static long _oggTell(void* datasource);
  
  Audio(const Audio&);
  void operator=(const Audio&);
};

#endif // DAGON_AUDIO_H_
