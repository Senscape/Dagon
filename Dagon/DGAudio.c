/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGArray.h"
#include "DGAudio.h"
#include "DGConfig.h"
#include "DGLog.h"
	 
#ifdef DG_PLATFORM_MAC

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#else

#include <AL/al.h>
#include <AL/alc.h>

#endif

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#define	DG_FADE_NONE	0
#define	DG_FADE_IN		1
#define	DG_FADE_OUT		2

#define BUFFER_SIZE		(8192 * 16) // Should be TWO buffers


////////////////////////////////////////////////////////////////////////////////
///// Private Data														   /////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
	char			file[DG_MAX_OBJNAME];
	DGBool			loaded;
	DGBool			loop;
	FILE*           oggFile;       // file handle
	OggVorbis_File  oggStream;     // stream handle
	vorbis_info*    vorbisInfo;    // some formatting data
	vorbis_comment* vorbisComment; // user comments
	
	// Must handle all buffers
	ALuint	buffers[2]; // front and back buffers
	ALuint	source;     // audio source
	ALenum	format;     // internal format
	
	float	fade_step;
	int		fade_style;
	float	gain;
	float	last_gain;
	float	target_gain;
} DG_AUDIO;

DGArray* channels;

static DGBool active = DG_NO;
ALCdevice* pDevice;
ALCcontext* pContext;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

DG_AUDIO* _toaudio(DGObject* audio);

////////////////////////////////////////////////////////////////////////////////
///// Init																   /////
////////////////////////////////////////////////////////////////////////////////

/*typedef long int (*alcASASetListenerProcPtr) (const ALuint property, ALvoid *data, ALuint dataSize);
long int alcASASetListenerProc(const ALuint property, ALvoid *data, ALuint dataSize)
{
    long int err = 0;
    static alcASASetListenerProcPtr proc = NULL;
    
    if (proc == NULL) {
        proc = (alcASASetListenerProcPtr) alcGetProcAddress(NULL, "alcASASetListener");
    }
    
    if (proc)
        err = proc(property, data, dataSize);
    return (err);
}

typedef long int	(*alcASASetSourceProcPtr)	(const ALuint property, ALuint source, ALvoid *data, ALuint dataSize);
long int  alcASASetSourceProc(const ALuint property, ALuint source, ALvoid *data, ALuint dataSize)
{
    long int	err = 0;
	static	alcASASetSourceProcPtr	proc = NULL;
    
    if (proc == NULL) {
        proc = (alcASASetSourceProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alcASASetSource");
    }
    
    if (proc)
        err = proc(property, source, data, dataSize);
    return (err);
}*/

void DGAudioInitialize() {
	pDevice = alcOpenDevice(NULL);
	pContext = alcCreateContext(pDevice, NULL);
	alcMakeContextCurrent(pContext);
	
	channels = DGArrayNew(0);
	
	// MAC ONLY
	// Reverb and Effects
    /*if (alcIsExtensionPresent(NULL, "ALC_EXT_ASA")) {
		ALfloat level = 40.0f;
		alcASASetListenerProc(alcGetEnumValue(NULL, "ALC_ASA_REVERB_GLOBAL_LEVEL"), &level, sizeof(level));
		
		ALboolean setting = 1;
		alcASASetListenerProc(alcGetEnumValue(NULL, "ALC_ASA_REVERB_ON"), &setting, sizeof(setting));
		
		ALfloat gain = 1.0f;
		alcASASetListenerProc(alcGetEnumValue(NULL, "ALC_ASA_REVERB_EQ_GAIN"), &gain, sizeof(gain));
		
		ALint roomtype = 12;
		alcASASetListenerProc(alcGetEnumValue(NULL, "ALC_ASA_REVERB_ROOM_TYPE"), &roomtype, sizeof(roomtype));
	}*/
	
	active = DG_YES;
}

////////////////////////////////////////////////////////////////////////////////
///// Standard New / Release											   /////
////////////////////////////////////////////////////////////////////////////////

DGObject* DGAudioNew(const char* withName, DGBool isLoopable) {
	DGObject* audio;
	DG_AUDIO audio_data;
	
	strcpy(audio_data.file, withName);
	audio_data.loaded = DG_NO;
	audio_data.loop = isLoopable;
    audio_data.gain = 1.0f;
    audio_data.last_gain = 1.0f;
    audio_data.target_gain = 1.0f;
    audio_data.fade_style = DG_FADE_NONE;
	audio_data.oggFile = NULL;
	
	// Store data into object and return
	audio = DGObjectNew(DG_OBJECT_AUDIO, &audio_data, sizeof(audio_data));
	DGArrayAddObject(channels, audio);
	
	return audio;
}

void DGAudioRelease(DGObject* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
		if (audio_data->loaded)
			DGAudioUnload(audio);
		
		DGObjectRelease(audio);
		// Delete from array?
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation													   /////
////////////////////////////////////////////////////////////////////////////////

DGBool DGAudioIsLoaded(DGObject* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
        return audio_data->loaded;
	}
	
	return DG_NO;
}

DGBool DGAudioIsPlaying(DGObject* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
        if (audio_data->loaded) {
            ALenum state;
            
            alGetSourcei(audio_data->source, AL_SOURCE_STATE, &state);
            
            return (state == AL_PLAYING);
        }
	}
	
	return DG_NO;
}

void DGAudioLoad(DGObject* audio) {
    int error;
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
		int result;
		
		if (!(audio_data->oggFile = fopen(DGConfigGetPath(DG_PATH_RES, audio_data->file), "rb")))
			DGLogError(DG_MOD_AUDIO, "Could not open Ogg file");
		
		if ((result = ov_open(audio_data->oggFile, &audio_data->oggStream, NULL, 0)) < 0) {
			fclose(audio_data->oggFile);
			
			DGLogError(DG_MOD_AUDIO, "Could not open Ogg stream");
			
			/*const char* ogg_stream::errorString(int code) {
			 switch(code)
			 {
			 case OV_EREAD:
			 return "Read from media.";
			 case OV_ENOTVORBIS:
			 return "Not Vorbis data.";
			 case OV_EVERSION:
			 return "Vorbis version mismatch.";
			 case OV_EBADHEADER:
			 return "Invalid Vorbis header.";
			 case OV_EFAULT:
			 return "Internal logic fault (bug or heap/stack corruption.";
			 default:
			 return "Unknown Ogg error.";
			 }
			 }*/
		}
		
		audio_data->vorbisInfo = ov_info(&audio_data->oggStream, -1);
		audio_data->vorbisComment = ov_comment(&audio_data->oggStream, -1);
		
		if (audio_data->vorbisInfo->channels == 1)
			audio_data->format = AL_FORMAT_MONO16;
		else
			audio_data->format = AL_FORMAT_STEREO16;
		
		alGenBuffers(2, audio_data->buffers);
		alGenSources(1, &audio_data->source);
		
		alSourcef (audio_data->source, AL_GAIN,			   audio_data->gain);
		alSource3f(audio_data->source, AL_POSITION,        0.0, 0.0, 0.0);
		alSource3f(audio_data->source, AL_VELOCITY,        0.0, 0.0, 0.0);
		alSource3f(audio_data->source, AL_DIRECTION,       0.0, 0.0, 0.0);
		alSourcef (audio_data->source, AL_ROLLOFF_FACTOR,  0.0          );
		alSourcei (audio_data->source, AL_SOURCE_RELATIVE, AL_TRUE      );
		
		//ALfloat level = 1.0f;
		//alcASASetSourceProc(alcGetEnumValue(NULL, "ALC_ASA_REVERB_SEND_LEVEL"), audio_data->source, &level, sizeof(level));
		
		audio_data->loaded = DG_YES;
        
        error = alGetError();
        if (error != AL_NO_ERROR)
            DGLogError(DG_MOD_AUDIO, "OpenAL error while loading file: %s (%d)", audio_data->file, error);
	}
}

///// Temporary

#if defined DG_PLATFORM_MAC

#define FADE_STEP 10.0f

#elif defined DG_PLATFORM_WIN

#define FADE_STEP 1.0f

#endif

/////

void DGAudioFadeIn(DGObject* audio, int forMilliseconds) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
		//audio_data->target_gain = audio_data->last_gain; // This is broken
		audio_data->target_gain = 1.0f;
		if (!forMilliseconds)
			audio_data->fade_step = 1.0f; // Instant fade
		else
			audio_data->fade_step = FADE_STEP / (float)forMilliseconds; // NOT ACCURATE, 1.0 Windows, 10.0 Mac
		
		audio_data->fade_style = DG_FADE_IN;
	}
}

void DGAudioFadeOut(DGObject* audio, int forMilliseconds) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
		audio_data->last_gain = audio_data->gain;
		audio_data->target_gain = 0.0f;
		if (!forMilliseconds)
			audio_data->fade_step = 1.0f; // Instant fade
		else
			audio_data->fade_step = FADE_STEP / (float)forMilliseconds; // NOT ACCURATE, 1.0 Windows, 10.0 Mac
		
		audio_data->fade_style = DG_FADE_OUT;
	}	
}

void DGAudioSetVolume(DGObject* audio, float theVolume) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
        if (audio_data->loaded) {
            alSourcef(audio_data->source, AL_GAIN, theVolume);
        }
        
		audio_data->gain = theVolume;
		audio_data->target_gain = theVolume;
		
		if (theVolume == 0)
			audio_data->last_gain = 1.0f;
		else
			audio_data->last_gain = theVolume;
	}
}

DGBool DGAudioLoop() {
	int error;
	DGBool keep_processing = DG_YES;
    DG_AUDIO* audio_data;
	
	if (active) {
		int idx, num_channels = DGArrayCount(channels);
		
		for (idx = 0; idx < num_channels; idx++) {
			DGObject* audio = DGArrayGetObject(channels, idx);
			audio_data = _toaudio(audio);
			
			if (audio_data) {
				if (audio_data->loaded) {
					if (DGAudioIsPlaying(audio)) {
						int j;
						int processed;
						
						alGetSourcei(audio_data->source, AL_BUFFERS_PROCESSED, &processed);
						
						while (processed--) {
							ALuint buffer;
							
							alSourceUnqueueBuffers(audio_data->source, 1, &buffer);
							for (j = 0; j < 1; j++) { // Move this to another function
								char data[BUFFER_SIZE];
								int  size = 0;
								int  section;
								int  result;
								
								while (size < BUFFER_SIZE) {
									result = ov_read(&audio_data->oggStream, data + size, BUFFER_SIZE - size, 0, 2, 1, & section);
									
									if (result > 0)
										size += result;
									else
										if (result < 0)
											DGLogError(DG_MOD_AUDIO, "Generic error");
										else
											break;
								}
								
								if (size == 0) {
									if (audio_data->loop)
										ov_raw_seek(&audio_data->oggStream, 0);
									else {
										DGAudioStop(audio); // Shouldn't stop before processing the buffers
										keep_processing = DG_NO;
									}
									break;
								}
								
								if (keep_processing)
									alBufferData(buffer, audio_data->format, data, size, audio_data->vorbisInfo->rate);
							}
							if (keep_processing)
								alSourceQueueBuffers(audio_data->source, 1, &buffer);
						}
						
						if (audio_data->fade_style == DG_FADE_OUT) {
							if (audio_data->target_gain < audio_data->gain) { // Fade out
								float aux = audio_data->gain;
								
								aux -= audio_data->fade_step;
								if (aux < 0) aux = 0.0f;
								
								alSourcef(audio_data->source, AL_GAIN, aux);
								audio_data->gain = aux;
							}
							else
								audio_data->fade_style = DG_FADE_NONE;
						}
						else if (audio_data->fade_style == DG_FADE_IN) {
							if (audio_data->target_gain > audio_data->gain) { // Fade in
								float aux = audio_data->gain;
								
								aux += audio_data->fade_step;
								//if (aux > 1) aux = 1.0f;
								
								alSourcef(audio_data->source, AL_GAIN, aux);
								audio_data->gain = aux;
							}
							else
								audio_data->fade_style = DG_FADE_NONE;
						}
					}
				}
			}
		}
		
		error = alGetError();
		if (error != AL_NO_ERROR)
			DGLogError(DG_MOD_AUDIO, "OpenAL error was raised: %d - at index: %d", error, idx);
	}
	
	return active;
}

void DGAudioPause(DGObject* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {	
		if (DGAudioIsPlaying(audio)) {
			int queued;
			
			//dg_log_trace(DG_MOD_AUDIO, "Pausing audio at channel %d", channel);
			alSourceStop(audio_data->source);
			
			alGetSourcei(audio_data->source, AL_BUFFERS_QUEUED, &queued);
			while (queued--) {
				ALuint buffer;
				alSourceUnqueueBuffers(audio_data->source, 1, &buffer);
			}
		}
	}
}

void DGAudioPlay(DGObject* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {		
		int i;
		
		if (DGAudioIsPlaying(audio) || !audio_data->loaded)
			return;
		
		//dg_log_trace(DG_MOD_AUDIO, "Playing audio at channel %d", channel);
		for (i = 0; i < 2; i++) { // Stream two buffers, should be configurable
			char data[BUFFER_SIZE];
			int  size = 0;
			int  section;
			int  result;
			
			// Watch out for this loop, if there's an error it kills the whole app
			while (size < BUFFER_SIZE) {
				result = ov_read(&audio_data->oggStream, data + size, BUFFER_SIZE - size, 0, 2, 1, & section);
				
				if (result > 0)
					size += result;
				else
					if (result < 0)
						DGLogError(DG_MOD_AUDIO, "Generic error");
					else
						break;
			}
			
			if (size == 0) {
				ov_raw_seek(&audio_data->oggStream, 0);
				break;
			}
			
			alBufferData(audio_data->buffers[i], audio_data->format, 
						 data, size, audio_data->vorbisInfo->rate);
		}
		
		alSourceQueueBuffers(audio_data->source, 2, audio_data->buffers);
		alSourcePlay(audio_data->source);
		
		if (!DGAudioIsPlaying(audio))
			DGLogError(DG_MOD_AUDIO, "Ogg refused to play");
	}
}

void DGAudioQueue(unsigned int inChannel, DGObject* anAudio) {
    // TODO: Must implement
	// This is now a different object
}

void DGAudioTerminate() {
	int idx, num_channels = DGArrayCount(channels);

	active = DG_NO;
	
	for (idx = 0; idx < num_channels; idx++) {
		DGObject* audio = DGArrayGetObject(channels, idx);
		DG_AUDIO* audio_data = _toaudio(audio);
	
		if (audio_data) {
			if (audio_data->loaded)
				DGAudioUnload(audio);
		}
	}

	alcMakeContextCurrent(NULL);
	alcDestroyContext(pContext);
	alcCloseDevice(pDevice);
}

void DGAudioStop(DGObject* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {	
		if (DGAudioIsPlaying(audio)) {
			int queued;
			
			alSourceStop(audio_data->source);

			alGetSourcei(audio_data->source, AL_BUFFERS_QUEUED, &queued);
			while (queued--) {
				ALuint buffer;
				alSourceUnqueueBuffers(audio_data->source, 1, &buffer);
			}
			
			ov_raw_seek(&audio_data->oggStream, 0);
		}
	}
}

void DGAudioUnload(DGObject* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {	
		audio_data->loaded = DG_NO;
		
		DGAudioStop(audio);
		
		alDeleteSources(1, &audio_data->source);
		alDeleteBuffers(1, audio_data->buffers);
		
		ov_clear(&audio_data->oggStream);
		fclose(audio_data->oggFile);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_AUDIO* _toaudio(DGObject* audio) {
	if (DGObjectIsType(audio, DG_OBJECT_AUDIO)) {
		DG_AUDIO* audio_data = (DG_AUDIO*)DGObjectData(audio);
		return audio_data;
	}
	else {
		// raise error
		return NULL;
	}
}
