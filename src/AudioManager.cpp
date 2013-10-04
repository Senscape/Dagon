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

#include <SDL2/SDL_timer.h>

#include "AudioManager.h"
#include "Config.h"
#include "Log.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

AudioManager::AudioManager()  :
config(Config::instance()),
log(Log::instance())
{
  _isInitialized = false;
  _isRunning = false;
  _mutex = SDL_CreateMutex();
  if (!_mutex)
    log.error(kModAudio, "%s", kString18001);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

AudioManager::~AudioManager() {
  SDL_DestroyMutex(_mutex);
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void AudioManager::clear() {
  if (_isInitialized) {
    if (!_arrayOfActiveAudios.empty()) {
      std::vector<Audio*>::iterator it;
      if (SDL_LockMutex(_mutex) == 0) {
        it = _arrayOfActiveAudios.begin();
        while (it != _arrayOfActiveAudios.end()) {
          (*it)->release();
          
          ++it;
        }
        SDL_UnlockMutex(_mutex);
      } else {
        log.error(kModAudio, "%s", kString18002);
      }
    }
  }
}

void AudioManager::flush() {
  if (_isInitialized) {
    if (!_arrayOfActiveAudios.empty()) {
      std::vector<Audio*>::iterator it;
      bool done = false;
      while (!done) {
        if (SDL_LockMutex(_mutex) == 0) {
          it = _arrayOfActiveAudios.begin();
          done = true;
          while ((it != _arrayOfActiveAudios.end())) {
            if ((*it)->retainCount() == 0) {
              if ((*it)->state() == kAudioStopped) { // Had to move this outside the switch
                // TODO: Automatically flush stopped and non-retained audios after
                // n update cycles
                (*it)->unload();
                _arrayOfActiveAudios.erase(it);
                done = false;
                break;
              }
              else {
                switch ((*it)->state()) {
                  case kAudioPlaying:
                    (*it)->fadeOut();
                    ++it;
                    break;
                  case kAudioPaused:
                  default:
                    ++it;
                    break;
                }
              }
            }
            else ++it;
          }
          SDL_UnlockMutex(_mutex);
        } else {
          log.error(kModAudio, "%s", kString18002);
        }
      }
    }
  }
}

void AudioManager::init() {
  log.trace(kModAudio, "%s", kString16001);
  
  char deviceName[256];
  char *defaultDevice;
  char *deviceList;
  char *devices[12];
  int numDevices, numDefaultDevice;
  
  strcpy(deviceName, "");
  if (config.debugMode) {
    if (alcIsExtensionPresent(NULL, (ALCchar*)"ALC_ENUMERATION_EXT") == AL_TRUE) { // Check if enumeration extension is present
      defaultDevice = (char *)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
      deviceList = (char *)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
      for (numDevices = 0; numDevices < 12; numDevices++) {devices[numDevices] = NULL;}
      for (numDevices = 0; numDevices < 12; numDevices++) {
        devices[numDevices] = deviceList;
        if (strcmp(devices[numDevices], defaultDevice) == 0) {
          numDefaultDevice = numDevices;
        }
        deviceList += strlen(deviceList);
        if (deviceList[0] == 0) {
          if (deviceList[1] == 0) {
            break;
          } else {
            deviceList += 1;
          }
        }
      }
      
      if (devices[numDevices] != NULL) {
        int i;
        
        numDevices++;
        if (config.debugMode) {
          log.trace(kModAudio, "%s", kString17003);
          log.trace(kModAudio, "0. NULL");
          for (i = 0; i < numDevices; i++) {
            log.trace(kModAudio, "%d. %s", i + 1, devices[i]);
          }
        }
        
        i = config.audioDevice;
        if ((i != 0) && (strlen(devices[i - 1]) < 256)) {
          strcpy(deviceName, devices[i - 1]);
        }
      }
    }
  }
  
  if (deviceName[0] == '\0') {
    log.trace(kModAudio, "%s", kString17004);
    _alDevice = alcOpenDevice(NULL); // Select the preferred device
  } else {
    log.trace(kModAudio, "%s: %s", kString17005, deviceName);
    _alDevice = alcOpenDevice((ALCchar*)deviceName); // Use the name from the enumeration process
  }
  
  if (!_alDevice) {
    log.error(kModAudio, "%s", kString16004);
    return;
  }
  
  _alContext = alcCreateContext(_alDevice, NULL);
  
  if (!_alContext) {
    log.error(kModAudio, "%s", kString16005);
    
    return;
  }
  
  alcMakeContextCurrent(_alContext);
  
  ALfloat listenerPos[] = {0.0f, 0.0f, 0.0f};
  ALfloat listenerVel[] = {0.0f, 0.0f, 0.0f};
  ALfloat listenerOri[] = {0.0f, 0.0f, -1.0f,
    0.0f, 1.0f, 0.0f}; // Listener facing into the screen
  
  alListenerfv(AL_POSITION, listenerPos);
  alListenerfv(AL_VELOCITY, listenerVel);
  alListenerfv(AL_ORIENTATION, listenerOri);
  
  ALint error = alGetError();
  
  if (error != AL_NO_ERROR) {
    log.error(kModAudio, "%s: init (%d)", kString16006, error);
    return;
  }
  
  log.info(kModAudio, "%s: %s", kString16002, alGetString(AL_VERSION));
  log.info(kModAudio, "%s: %s", kString16003, vorbis_version_string());
  
  _isInitialized = true;
  _isRunning = true;
  
  _thread = SDL_CreateThread(_runThread, "AudioManager", (void*)NULL);
  if (!_thread) {
    log.error(kModAudio, "%s:%s", kString18003, SDL_GetError());
  }
}

void AudioManager::registerAudio(Audio* target) {
  _arrayOfAudios.push_back(target);
}

void AudioManager::requestAudio(Audio* target) {
  if (!target->isLoaded()) {
    target->load();
  }
  
  target->retain();
  
  // If the audio is not active, then it's added to
  // that vector
  
  bool isActive = false;
  std::vector<Audio*>::iterator it;
  it = _arrayOfActiveAudios.begin();
  
  while (it != _arrayOfActiveAudios.end()) {
    if ((*it) == target) {
      isActive = true;
      break;
    }
    ++it;
  }
  
  if (!isActive) {
    if (SDL_LockMutex(_mutex) == 0) {
      _arrayOfActiveAudios.push_back(target);
      SDL_UnlockMutex(_mutex);
    } else {
      log.error(kModAudio, "%s", kString18002);
    }
  }
  
  // FIXME: Not very elegant. Must implement a state condition for
  // each audio object. Perhaps use AL_STATE even.
  if (target->state() == kAudioPaused) {
    target->play();
  }
}

void AudioManager::setOrientation(float* orientation) {
  if (_isInitialized) {
    alListenerfv(AL_ORIENTATION, orientation);
  }
}

void AudioManager::terminate() {
  // FIXME: Here it's important to determine if the
  // audio was created by Lua or by another class
  
  // Each audio object should unregister itself if
  // destroyed
  _isRunning = false;
  
  int threadReturnValue;
  SDL_WaitThread(_thread, &threadReturnValue);
  
  if (!_arrayOfAudios.empty()) {
    if (SDL_LockMutex(_mutex) == 0) {
      std::vector<Audio*>::iterator it = _arrayOfAudios.begin();
      while (it != _arrayOfAudios.end()) {
        // FIXME: Should delete here and let the audio object unload itself
        (*it)->unload();
        ++it;
      }
      SDL_UnlockMutex(_mutex);
    } else {
      log.error(kModAudio, "%s", kString18002);
    }
  }
  
  // Now we shut down OpenAL completely
  if (_isInitialized) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(_alContext);
    alcCloseDevice(_alDevice);
  }
}

// Asynchronous method
bool AudioManager::update() {
  if (_isRunning) {
    if (!_arrayOfActiveAudios.empty()) {
      if (SDL_LockMutex(_mutex) == 0) {
        std::vector<Audio*>::iterator it = _arrayOfActiveAudios.begin();
        while (it != _arrayOfActiveAudios.end()) {
          (*it)->update();
          ++it;
        }
        SDL_UnlockMutex(_mutex);
      } else {
        log.error(kModAudio, "%s", kString18002);
      }
    }
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

int AudioManager::_runThread(void *ptr) {
  while (AudioManager::instance().update()) {
    SDL_Delay(1);
  }
  return 0;
}
  
}
