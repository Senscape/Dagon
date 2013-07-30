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

#include "DGAudio.h"

using namespace std;

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGAudio::DGAudio() :
    config(Config::instance()),
    log(DGLog::instance())
{
    _oggCallbacks.read_func = _oggRead;
	_oggCallbacks.seek_func = _oggSeek;
	_oggCallbacks.close_func = _oggClose;
	_oggCallbacks.tell_func = _oggTell;
    
    _isLoaded = false;
    
    // For convenience, this always defaults to false
    _isLoopable = false;
    _isMatched = false;
    _state = DGAudioInitial;
    
    this->setType(DGObjectAudio);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGAudio::~DGAudio() {
    // Let the manager perform the unload
}

////////////////////////////////////////////////////////////
// Implementation - Checks
////////////////////////////////////////////////////////////

bool DGAudio::isLoaded() {
    return _isLoaded;
}

bool DGAudio::isLoopable() {
    return _isLoopable;
}

bool DGAudio::isPlaying() {
    lock_guard<mutex> guard(_mutex);
    
    return (_state == DGAudioPlaying);
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

double DGAudio::cursor() {
    return ov_time_tell(&_oggStream);
}

int DGAudio::state() {
    return _state;
}

////////////////////////////////////////////////////////////
// Implementation - Sets
////////////////////////////////////////////////////////////

void DGAudio::setLoopable(bool loopable) {
    _isLoopable = loopable;
}

void DGAudio::setPosition(unsigned int onFace, DGPoint origin) {
    float x = (float)origin.x / (float)kDefTexSize;
    float y = (float)origin.y / (float)kDefTexSize;
    
    switch (onFace) {
        case kNorth:
            alSource3f(_alSource, AL_POSITION, x, y, -1.0);
            break;
        case kEast:
            alSource3f(_alSource, AL_POSITION, 1.0, y, x);
            break;
        case kSouth:
            alSource3f(_alSource, AL_POSITION, -x, y, 1.0);
            break;
        case kWest:
            alSource3f(_alSource, AL_POSITION, -1.0, y, -x);
            break;
        case kUp:
            alSource3f(_alSource, AL_POSITION, 0.0, 1.0, 0.0);
            break;
        case kDown:
            alSource3f(_alSource, AL_POSITION, 0.0, -1.0, 0.0);
            break;            
    }
    
    _verifyError("position");
}

void DGAudio::setResource(const char* fromFileName) {
    strncpy(_resource.name, fromFileName, kMaxFileLength);
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGAudio::load() {
    lock_guard<mutex> guard(_mutex);
    
    if (!_isLoaded) { 
        FILE* fh;
        
        // FIXME: This object shouldn't reference Config, let the Audio Manager do this
        const char* fileToLoad = _randomizeFile(_resource.name);
        fh = fopen(config.path(kPathResources, fileToLoad, DGObjectAudio).c_str(), "rb");
        
        if (fh != NULL) {
            fseek(fh, 0, SEEK_END);
            _resource.dataSize = ftell(fh);
            fseek(fh, 0, SEEK_SET);
            _resource.data = (char*)malloc(_resource.dataSize);
            fread(_resource.data, _resource.dataSize, 1, fh);
            _resource.dataRead = 0;
            
            if (ov_open_callbacks(this, &_oggStream, NULL, 0, _oggCallbacks) < 0) {
                log.error(DGModAudio, "%s", DGMsg270007);
                return;
            }
            
            // Get file info
            vorbis_info* info = ov_info(&_oggStream, -1);
            
            _channels = info->channels;
            _rate = (ALsizei)info->rate;
            
            if (_channels == 1) _alFormat = AL_FORMAT_MONO16;
            else if (_channels == 2 ) _alFormat = AL_FORMAT_STEREO16;
            else {
                // Invalid number of channels
                log.error(DGModAudio, "%s: %s", DGMsg270006, fileToLoad);
                return;
            }
            
            // We no longer require the file handle
            fclose(fh);
            
            alGenBuffers(DGAudioNumberOfBuffers, _alBuffers);
            alGenSources(1, &_alSource);

			for (int i = 0; i < DGAudioNumberOfBuffers; i++) {
				if (!_stream(&_alBuffers[i])) {
					_verifyError("prebuffer");
                
					return;
				}
			}
        
			alSourceQueueBuffers(_alSource, DGAudioNumberOfBuffers, _alBuffers);
    
            if (config.mute || this->fadeLevel() < 0.0f) {
                alSourcef(_alSource, AL_GAIN, 0.0f);
            }
            else {
                alSourcef(_alSource, AL_GAIN, this->fadeLevel());
            }
            
            alSource3f(_alSource, AL_POSITION,        0.0, 0.0, 0.0);
            alSource3f(_alSource, AL_VELOCITY,        0.0, 0.0, 0.0);
            alSource3f(_alSource, AL_DIRECTION,       0.0, 0.0, 0.0);
            
            _verifyError("load");
            
            _isLoaded = true;
        }
        else log.error(DGModAudio, "%s: %s", DGMsg270005, fileToLoad);
    }
}

void DGAudio::match(DGAudio* matchedAudio) {
    _matchedAudio = matchedAudio;
    _isMatched = true;
}

void DGAudio::play() {
    lock_guard<mutex> guard(_mutex);
    
    if (_isLoaded && (_state != DGAudioPlaying)) {
        if (_isMatched)
            ov_time_seek(&_oggStream, _matchedAudio->cursor());
        
        alSourcePlay(_alSource);
        _state = DGAudioPlaying;
        
        _verifyError("play");
    }
}

void DGAudio::pause() {
    lock_guard<mutex> guard(_mutex);
    
    if (_state == DGAudioPlaying) {
        alSourceStop(_alSource);

        _state = DGAudioPaused;
        
        _verifyError("pause");
    }
}

void DGAudio::stop() {
    lock_guard<mutex> guard(_mutex);
    
    if ((_state == DGAudioPlaying) || (_state == DGAudioPaused)) {
        alSourceStop(_alSource);

        ov_raw_seek(&_oggStream, 0);
        _state = DGAudioStopped;
        
        _verifyError("stop");
    }
}

void DGAudio::unload() {
    lock_guard<mutex> guard(_mutex);
    
    if (_isLoaded) {
        if (_state == DGAudioPlaying) {
            alSourceStop(_alSource);
            ov_raw_seek(&_oggStream, 0);
            _state = DGAudioStopped;
        }

		_emptyBuffers();
		
		alDeleteSources(1, &_alSource);
		alDeleteBuffers(DGAudioNumberOfBuffers, _alBuffers);
        ov_clear(&_oggStream);
        free(_resource.data);
        
        _isLoaded = false;
        
        _verifyError("unload");
    }
}

void DGAudio::update() {
    lock_guard<mutex> guard(_mutex);
    
    if (_state == DGAudioPlaying) {
        int processed;
        
        alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);
        
        while (processed--) {
            ALuint buffer;
            
            alSourceUnqueueBuffers(_alSource, 1, &buffer);
            _stream(&buffer);
            alSourceQueueBuffers(_alSource, 1, &buffer);
        }
        
        // Run fade operations
        // TODO: Better change the name of the DGObject "updateFade" function, it's somewhat confusing
        this->updateFade();
        
        // FIXME: Not very elegant as we're doing this check every time
        if (config.mute) {
            alSourcef(_alSource, AL_GAIN, 0.0f);
        }
        else {
            // Finally check the current volume. If it's zero, let the manager know
            // that we're done with this audio
            if (this->fadeLevel() > 0.0f)
                alSourcef(_alSource, AL_GAIN, this->fadeLevel());
            else {
                alSourceStop(_alSource);
                _state = DGAudioPaused;
            }
        }
    }
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGAudio::_emptyBuffers() {
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

const char* DGAudio::_randomizeFile(const char* fileName) {
    if (strstr(fileName, ".ogg")) { // Was extension specified?
        return fileName; // Then return as-is
    }
    else { // Randomize
        static char fileToLoad[kMaxFileLength];
        int index = (rand() % 6); // TODO: Allow to configure this value
        
        snprintf(fileToLoad, kMaxFileLength, "%s%0" in_between(kFileSeqDigits) "d.%s", fileName,
                 index + kFileSeqStart, "ogg");
        
        return fileToLoad;
    }
}

bool DGAudio::_stream(ALuint* buffer) {
    // This is a failsafe; if this is true, we won't attempt to stream anymore
    static bool _hasStreamingError = false;         
    
    if (!_hasStreamingError) {
        char* data;
        int size = 0;
        int section;
        long result;
        
		data = (char*)malloc(config.audioBuffer);
        while (size < config.audioBuffer) {
            result = ov_read(&_oggStream, data + size, config.audioBuffer - size, 0, 2, 1, &section);
            
            if (result > 0)
                size += result;
            else if (result == 0) {
                // EOF
                if (_isLoopable)
                    ov_raw_seek(&_oggStream, 0);
                else {
                    alSourceStop(_alSource);
                    ov_raw_seek(&_oggStream, 0);
                    _state = DGAudioStopped;
                }
                
                return false;
            }
            else if (result == OV_HOLE) {
                // May return OV_HOLE after we rewind the stream, so we just re-loop
                continue;
            }
            else if (result < 0) {
                // Error
                log.error(DGModAudio, "%s: %s", DGMsg270004, _resource.name);
                _hasStreamingError = true;
                
                return false;
            }
        }
        
        alBufferData(*buffer, _alFormat, data, size, _rate);
        delete(data);
        return true;
    }
    else return false;
}

ALboolean DGAudio::_verifyError(const char* operation) {
   	ALint error = alGetError();
    
	if (error != AL_NO_ERROR) {
		log.error(DGModAudio, "%s: %s: %s (%d)", DGMsg270003, _resource.name, operation, error);
        
		return AL_FALSE;
	}
    
	return AL_TRUE; 
}

// And now... The Vorbisfile callbacks

size_t DGAudio::_oggRead(void* ptr, size_t size, size_t nmemb, void* datasource) {
    DGAudio* audio = (DGAudio*)datasource;
    size_t nSize = size * nmemb;
    
    if ((audio->_resource.dataRead + nSize) > audio->_resource.dataSize)
        nSize = audio->_resource.dataSize - audio->_resource.dataRead;
    
    memcpy(ptr, audio->_resource.data + audio->_resource.dataRead, nSize);
    audio->_resource.dataRead += nSize;
    
	return nSize;
}

int DGAudio::_oggSeek(void* datasource, ogg_int64_t offset, int whence) {
    DGAudio* audio = (DGAudio*)datasource;
    
	switch (whence) {
        case SEEK_SET: 
            audio->_resource.dataRead = (size_t)offset; 
            break;
        case SEEK_CUR: 
            audio->_resource.dataRead += (size_t)offset;
            break;
        case SEEK_END: 
            audio->_resource.dataRead = (size_t)(audio->_resource.dataSize - offset); 
            break;
	}
    
	if (audio->_resource.dataRead > audio->_resource.dataSize) {
		audio->_resource.dataRead = 0;
		return -1;
	}
    
	return 0;
}

int DGAudio::_oggClose(void* datasource) {
    DGAudio* audio = (DGAudio*)datasource;
	audio->_resource.dataRead = 0;
    
	return 0;
}

long DGAudio::_oggTell(void* datasource) {
    DGAudio* audio = (DGAudio*)datasource;
	return (long)audio->_resource.dataRead;
}
