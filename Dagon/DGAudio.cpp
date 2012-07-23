////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
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

DGAudio::DGAudio() {
    config = &DGConfig::getInstance();
    log = &DGLog::getInstance();
    
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
    // Force an unload
    this->unload();
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

void DGAudio::setPosition(unsigned int onFace) {    
    switch (onFace) {
        case DGNorth:
            alSource3f(_alSource, AL_POSITION, 0.0, 0.0, -1.0);
            break;
        case DGEast:
            alSource3f(_alSource, AL_POSITION, 1.0, 0.0, 0.0);
            break;
        case DGSouth:
            alSource3f(_alSource, AL_POSITION, 0.0, 0.0, 1.0);
            break;
        case DGWest:
            alSource3f(_alSource, AL_POSITION, -1.0, 0.0, 0.0);
            break;
        case DGUp:
            alSource3f(_alSource, AL_POSITION, 0.0, 1.0, 0.0);
            break;
        case DGDown:
            alSource3f(_alSource, AL_POSITION, 0.0, -1.0, 0.0);
            break;            
    }
    
    _verifyError("position");
}

void DGAudio::setResource(const char* fromFileName) {
    strncpy(_resource.name, fromFileName, DGMaxFileLength);
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

void DGAudio::load() {
    if (!_isLoaded) { 
        FILE* fh;
        
        // FIXME: This object shouldn't reference DGConfig, let the manager do it
        const char* fileToLoad = _randomizeFile(_resource.name);
        fh = fopen(config->path(DGPathRes, fileToLoad, DGObjectAudio), "rb");	
        
        if (fh != NULL) {
            fseek(fh, 0, SEEK_END);
            _resource.dataSize = ftell(fh);
            fseek(fh, 0, SEEK_SET);
            _resource.data = (char*)malloc(_resource.dataSize);
            fread(_resource.data, _resource.dataSize, 1, fh);
            _resource.dataRead = 0;
            
            if (ov_open_callbacks(this, &_oggStream, NULL, 0, _oggCallbacks) < 0) {
                log->error(DGModAudio, "%s", DGMsg270007);
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
                log->error(DGModAudio, "%s: %s", DGMsg270006, fileToLoad);
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
    
            if (config->mute || this->fadeLevel() < 0.0f) {
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
        else log->error(DGModAudio, "%s: %s", DGMsg270005, fileToLoad);
    }
}

void DGAudio::match(DGAudio* matchedAudio) {
    _matchedAudio = matchedAudio;
    _isMatched = true;
}

void DGAudio::play() {
    if (_isLoaded && (_state != DGAudioPlaying)) {
        if (_isMatched)
            ov_time_seek(&_oggStream, _matchedAudio->cursor());
        

        alSourcePlay(_alSource);
        _state = DGAudioPlaying;
        
        _verifyError("play");
    }
}

void DGAudio::pause() {
    if (_state == DGAudioPlaying) {
        alSourceStop(_alSource);

        _state = DGAudioPaused;
        
        _verifyError("pause");
    } 
}

void DGAudio::stop() {
    if ((_state == DGAudioPlaying) || (_state == DGAudioPaused)) {
        alSourceStop(_alSource);

        ov_raw_seek(&_oggStream, 0);
        _state = DGAudioStopped;
        
        _verifyError("stop");
    }
}

void DGAudio::unload() {
    if (_isLoaded) {
        if (_state == DGAudioPlaying)
            this->stop();

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
        // TODO: Better change the name of the DGObject function
        this->updateFade();
        
        // FIXME: Not very elegant as we're doing this every time
        if (config->mute) {
            alSourcef(_alSource, AL_GAIN, 0.0f);
        }
        else {
            // Finally check the current volume. If it's zero, let the manager know
            // that we're done with this audio
            if (this->fadeLevel() > 0.0f)
                alSourcef(_alSource, AL_GAIN, this->fadeLevel());
            else
                this->pause();
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
        return fileName; // Return as-is
    }
    else { // Randomize
        static char fileToLoad[DGMaxFileLength];
        int index = (rand() % 6); // Allow to configure
        
        snprintf(fileToLoad, DGMaxFileLength, "%s%0" in_between(DGFileSeqDigits) "d.%s", fileName,
                 index + DGFileSeqStart, "ogg");
        
        return fileToLoad;
    }
}

bool DGAudio::_stream(ALuint* buffer) {
    // This is a failsafe; if this is true, we won't attempt
    // to stream anymore
    static bool _hasStreamingError = false;         
    
    if (!_hasStreamingError) {
        char data[DGAudioBufferSize];
        int size = 0;
        int section;
        long result;
        
        while (size < DGAudioBufferSize) {
            result = ov_read(&_oggStream, data + size, DGAudioBufferSize - size, 0, 2, 1, &section);
            
            if (result > 0)
                size += result;
            else if (result == 0) {
                // EOF
                if (_isLoopable)
                    ov_raw_seek(&_oggStream, 0);
                else
                    this->stop();
                
                return false;
            }
            else if (result == OV_HOLE) {
                // May return OV_HOLE after we rewind the stream,
                // so we just re-loop
                continue;
            }
            else if (result < 0) {
                // Error
                log->error(DGModAudio, "%s: %s", DGMsg270004, _resource.name);
                _hasStreamingError = true;
                
                return false;
            }
        }
        
        alBufferData(*buffer, _alFormat, data, size, _rate);
        
        return true;
    }
    else return false;
}

ALboolean DGAudio::_verifyError(const char* operation) {
   	ALint error = alGetError();
    
	if (error != AL_NO_ERROR) {
		log->error(DGModAudio, "%s: %s: %s (%d)", DGMsg270003, _resource.name, operation, error);
        
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
