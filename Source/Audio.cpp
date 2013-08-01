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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <fstream>
#include <cassert>
#include <cstring>

#include "Audio.h"
#include "Config.h"
#include "Log.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Audio::Audio() :
  config(Config::instance()),
  log(Log::instance())
{
  _oggCallbacks.read_func = _oggRead;
	_oggCallbacks.seek_func = _oggSeek;
	_oggCallbacks.close_func = _oggClose;
	_oggCallbacks.tell_func = _oggTell;
    
  this->setType(DGObjectAudio);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Audio::~Audio() {
  // TODO: Unload if required
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool Audio::isLoaded() {
  std::lock_guard<std::mutex> guard(_mutex);
  return _isLoaded;
}

bool Audio::isLoopable() {
  return _isLoopable;
}

bool Audio::isPlaying() {
  std::lock_guard<std::mutex> guard(_mutex);
  return (_state == kAudioPlaying);
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

double Audio::cursor() {
  return ov_time_tell(&_oggStream);
}

int Audio::state() {
  return _state;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void Audio::setLoopable(bool loopable) {
  _isLoopable = loopable;
}

void Audio::setPosition(unsigned int face, Point origin) {
  float x = origin.x / kDefTexSize;
  float y = origin.y / kDefTexSize;
    
  switch (face) {
    case kNorth: {
      alSource3f(_alSource, AL_POSITION, x, y, -1.0);
      break;
    }
    case kEast: {
      alSource3f(_alSource, AL_POSITION, 1.0, y, x);
      break;
    }
    case kSouth: {
      alSource3f(_alSource, AL_POSITION, -x, y, 1.0);
      break;
    }
    case kWest: {
      alSource3f(_alSource, AL_POSITION, -1.0, y, -x);
      break;
    }
    case kUp: {
      alSource3f(_alSource, AL_POSITION, 0.0, 1.0, 0.0);
      break;
    }
    case kDown: {
      alSource3f(_alSource, AL_POSITION, 0.0, -1.0, 0.0);
      break;
    }
    default: {
      assert(false);
    }
  }
  
  _verifyError("position");
}

void Audio::setResource(std::string fileName) {
  _resource.name = fileName;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void Audio::load() {
  std::lock_guard<std::mutex> guard(_mutex);
    
  if (!_isLoaded) { 
    std::string fileToLoad = _randomizeFile(_resource.name);
    std::ifstream file(config.path(kPathResources, fileToLoad, DGObjectAudio),
                       std::ifstream::binary | std::ifstream::ate);
    
    if (file.good()) {
      _resource.dataSize = file.tellg();
      _resource.data = new char[_resource.dataSize];
      file.seekg(file.beg);
      file.read(_resource.data, _resource.dataSize);
      _resource.dataRead = 0;
      
      if (ov_open_callbacks(this, &_oggStream, NULL, 0, _oggCallbacks) < 0) {
        log.error(kModAudio, "%s", kString16010);
        return;
      }
      
      // We no longer require the file handle
      file.close();
      
      // Get file info
      vorbis_info* info = ov_info(&_oggStream, -1);
      _channels = info->channels;
      _rate = (ALsizei)info->rate;
            
      if (_channels == 1) {
        _alFormat = AL_FORMAT_MONO16;

      } else if (_channels == 2 ) {
        _alFormat = AL_FORMAT_STEREO16;
      } else {
        // Invalid number of channels
        log.error(kModAudio, "%s: %s", kString16009, fileToLoad.c_str());
        return;
      }
      
      alGenBuffers(kAudioBuffers, _alBuffers);
      alGenSources(1, &_alSource);
      alSource3f(_alSource, AL_POSITION, 0.0, 0.0, 0.0);
      alSource3f(_alSource, AL_VELOCITY, 0.0, 0.0, 0.0);
      alSource3f(_alSource, AL_DIRECTION, 0.0, 0.0, 0.0);

			for (int i = 0; i < kAudioBuffers; i++) {
        if (!_fillBuffer(&_alBuffers[i])) {
					_verifyError("prebuffer");
					return;
				}
			}
        
			alSourceQueueBuffers(_alSource, kAudioBuffers, _alBuffers);
  
      if (config.mute || this->fadeLevel() < 0.0) {
        alSourcef(_alSource, AL_GAIN, 0.0);
      } else {
        alSourcef(_alSource, AL_GAIN, this->fadeLevel());
      }
      
      _isLoaded = true;      
      _verifyError("load");
    } else {
      log.error(kModAudio, "%s: %s", kString16008, fileToLoad.c_str());
    }
  }
}

void Audio::match(Audio* audioToMatch) {
  _matchedAudio = audioToMatch;
  _isMatched = true;
}

void Audio::play() {
  std::lock_guard<std::mutex> guard(_mutex);
    
  if (_isLoaded && (_state != kAudioPlaying)) {
    if (_isMatched)
      ov_time_seek(&_oggStream, _matchedAudio->cursor());
        
    alSourcePlay(_alSource);
    _state = kAudioPlaying;
    _verifyError("play");
  }
}

void Audio::pause() {
  std::lock_guard<std::mutex> guard(_mutex);
    
  if (_state == kAudioPlaying) {
    alSourceStop(_alSource);
    _state = kAudioPaused;
    _verifyError("pause");
  }
}

void Audio::stop() {
  std::lock_guard<std::mutex> guard(_mutex);
    
  if ((_state == kAudioPlaying) || (_state == kAudioPaused)) {
    alSourceStop(_alSource);
    ov_raw_seek(&_oggStream, 0);
    _state = kAudioStopped;
    _verifyError("stop");
  }
}

void Audio::unload() {
  std::lock_guard<std::mutex> guard(_mutex);
    
  if (_isLoaded) {
    if (_state == kAudioPlaying) {
      alSourceStop(_alSource);
      ov_raw_seek(&_oggStream, 0);
      _state = kAudioStopped;
    }

		_emptyBuffers();
		alDeleteSources(1, &_alSource);
		alDeleteBuffers(kAudioBuffers, _alBuffers);
    ov_clear(&_oggStream);
    delete[] _resource.data;
    
    _isLoaded = false;
    _verifyError("unload");
  }
}

void Audio::update() {
  std::lock_guard<std::mutex> guard(_mutex);
    
  if (_state == kAudioPlaying) {
    int processed;
        
    alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);
        
    while (processed--) {
      ALuint buffer;
            
      alSourceUnqueueBuffers(_alSource, 1, &buffer);
      _fillBuffer(&buffer);
      alSourceQueueBuffers(_alSource, 1, &buffer);
    }
        
    // Run fade operations
    // TODO: Better change the name of the DGObject "updateFade" function,
    // it's somewhat confusing.
    this->updateFade();
        
    // FIXME: Not very elegant as we're doing this check every time
    if (config.mute) {
      alSourcef(_alSource, AL_GAIN, 0.0f);
    } else {
      // Finally check the current volume. If it's zero, let the manager know
      // that we're done with this audio.
      if (this->fadeLevel() > 0.0f) {
        alSourcef(_alSource, AL_GAIN, this->fadeLevel());
      } else {
        alSourceStop(_alSource);
        _state = kAudioPaused;
      }
    }
  }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

bool Audio::_fillBuffer(ALuint* buffer) {
  // This is a failsafe; if this is true, we won't attempt to stream anymore
  static bool _hasStreamingError = false;
  
  if (!_hasStreamingError) {
    char data[config.audioBuffer];
    int size = 0;
    
    while (size < config.audioBuffer) {
      int section;
      long result = ov_read(&_oggStream, data + size, config.audioBuffer - size,
                            0, 2, 1, &section);
      
      if (result > 0) {
        size += result;
      } else if (result == 0) {
        // EOF
        if (_isLoopable) {
          ov_raw_seek(&_oggStream, 0);
        } else {
          alSourceStop(_alSource);
          ov_raw_seek(&_oggStream, 0);
          _state = kAudioStopped;
        }
        return false;
      } else if (result == OV_HOLE) {
        // May return OV_HOLE after we rewind the stream, so we just re-loop.
        continue;
      } else if (result < 0) {
        // Error
        log.error(kModAudio, "%s: %s", kString16007, _resource.name.c_str());
        _hasStreamingError = true;
        return false;
      }
    }
    alBufferData(*buffer, _alFormat, data, size, _rate);
    return true;
  } else {
    return false; 
  }
}

void Audio::_emptyBuffers() {
  ALint state;
  alGetSourcei(_alSource, AL_SOURCE_STATE, &state);

	if (state != AL_PLAYING) {
    int queued;
    alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &queued);

    while (queued--) {
      ALuint buffer;
      alSourceUnqueueBuffers(_alSource, 1, &buffer);
			_verifyError("unqueue");
    }
  }
}

std::string Audio::_randomizeFile(const std::string &fileName) {
  // Was extension specified?
  if (fileName.find(".ogg") != std::string::npos ) {
    // Then return as-is
    return fileName;
  } else {
    // If not, randomize
    int index = (rand() % 6) + 1; // TODO: Allow to configure this value

    std::string fileToLoad(fileName);
    fileToLoad.append(2, '0'); // TODO: Also configure this
    fileToLoad += std::to_string(index) + ".ogg";
    return fileToLoad;
  }
}

ALboolean Audio::_verifyError(const std::string &operation) {
  ALint error = alGetError();
  
  if (error != AL_NO_ERROR) {
    log.error(kModAudio, "%s: %s: %s (%d)", kString16006,
              _resource.name.c_str(), operation.c_str(), error);
    return AL_FALSE;
	}
	return AL_TRUE; 
}

// And now... The Vorbisfile callbacks

size_t Audio::_oggRead(void* ptr, size_t size, size_t nmemb, void* datasource) {
  Audio* audio = static_cast<Audio*>(datasource);
  size_t nSize = size * nmemb;
  
  if ((audio->_resource.dataRead + nSize) > audio->_resource.dataSize)
    nSize = audio->_resource.dataSize - audio->_resource.dataRead;
    
  std::memcpy(ptr, audio->_resource.data + audio->_resource.dataRead, nSize);
  audio->_resource.dataRead += nSize;    
	return nSize;
}

int Audio::_oggSeek(void* datasource, ogg_int64_t offset, int whence) {
  Audio* audio = static_cast<Audio*>(datasource);
    
	switch (whence) {
    case SEEK_SET: {
      audio->_resource.dataRead = offset;
      break;
    }
    case SEEK_CUR: {
      audio->_resource.dataRead += offset;
      break;
    }
    case SEEK_END: {
      audio->_resource.dataRead = audio->_resource.dataSize - offset;
      break;
    }
    default: {
      assert(false);
    }
	}
    
	if (audio->_resource.dataRead > audio->_resource.dataSize) {
		audio->_resource.dataRead = 0;
		return -1;
	}
  
	return 0;
}

int Audio::_oggClose(void* datasource) {
  Audio* audio = static_cast<Audio*>(datasource);
  audio->_resource.dataRead = 0;
  return 0;
}

long Audio::_oggTell(void* datasource) {
  Audio* audio = static_cast<Audio*>(datasource);
  return audio->_resource.dataRead;
}
