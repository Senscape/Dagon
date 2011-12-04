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
	DG_BOOL			loaded;
	DG_BOOL			loop;
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

DG_ARRAY* channels;

static DG_BOOL active = DG_NO;
ALCdevice* pDevice;
ALCcontext* pContext;

////////////////////////////////////////////////////////////////////////////////
///// Private Prototypes												   /////
////////////////////////////////////////////////////////////////////////////////

DG_AUDIO* _toaudio(DG_OBJECT* audio);

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

void dg_audio_init() {
	pDevice = alcOpenDevice(NULL);
	pContext = alcCreateContext(pDevice, NULL);
	alcMakeContextCurrent(pContext);
	
	channels = dg_array_new(0);
	
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

DG_OBJECT* dg_audio_new(const char* name, DG_BOOL loop) {
	DG_OBJECT* audio;
	DG_AUDIO audio_data;
	
	strcpy(audio_data.file, name);
	audio_data.loaded = DG_NO;
	audio_data.loop = loop;
    audio_data.gain = 1.0f;
    audio_data.last_gain = 1.0f;
    audio_data.target_gain = 1.0f;
    audio_data.fade_style = DG_FADE_NONE;
	audio_data.oggFile = NULL;
	
	// Store data into object and return
	audio = dg_object_new(DG_OBJ_AUDIO, &audio_data, sizeof(audio_data));
	dg_array_add_object(channels, audio);
	
	return audio;
}

void dg_audio_release(DG_OBJECT* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
		if (audio_data->loaded)
			dg_audio_unload(audio);
		
		dg_object_release(audio);
		// Delete from array?
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Implementation													   /////
////////////////////////////////////////////////////////////////////////////////

DG_BOOL dg_audio_is_loaded(DG_OBJECT* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
        return audio_data->loaded;
	}
	
	return DG_NO;
}

DG_BOOL dg_audio_is_playing(DG_OBJECT* audio) {
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

void dg_audio_load(DG_OBJECT* audio) {
    int error;
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
		int result;
		
		if (!(audio_data->oggFile = fopen(dg_config_get_path(DG_RES, audio_data->file), "rb")))
			dg_log_error(DG_MOD_AUDIO, "Could not open Ogg file");
		
		if ((result = ov_open(audio_data->oggFile, &audio_data->oggStream, NULL, 0)) < 0) {
			fclose(audio_data->oggFile);
			
			dg_log_error(DG_MOD_AUDIO, "Could not open Ogg stream");
			
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
            dg_log_error(DG_MOD_AUDIO, "OpenAL error while loading file: %s (%d)", audio_data->file, error);
	}
}

///// Temporary

#if defined DG_PLATFORM_MAC

#define FADE_STEP 1.0f

#elif defined DG_PLATFORM_WIN

#define FADE_STEP 1.0f

#endif

/////

void dg_audio_fade_in(DG_OBJECT* audio, int msecs) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
		//audio_data->target_gain = audio_data->last_gain; // This is broken
		audio_data->target_gain = 1.0f;
		if (!msecs)
			audio_data->fade_step = 1.0f; // Instant fade
		else
			audio_data->fade_step = FADE_STEP / (float)msecs; // NOT ACCURATE, 1.0 Windows, 100.0 Mac
		
		audio_data->fade_style = DG_FADE_IN;
	}
}

void dg_audio_fade_out(DG_OBJECT* audio, int msecs) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
		audio_data->last_gain = audio_data->gain;
		audio_data->target_gain = 0.0f;
		if (!msecs)
			audio_data->fade_step = 1.0f; // Instant fade
		else
			audio_data->fade_step = FADE_STEP / (float)msecs; // NOT ACCURATE, 1.0 Windows, 100.0 Mac
		
		audio_data->fade_style = DG_FADE_OUT;
	}	
}

void dg_audio_set_volume(DG_OBJECT* audio, float volume) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {
        if (audio_data->loaded) {
            alSourcef(audio_data->source, AL_GAIN, volume);
        }
        
		audio_data->gain = volume;
		audio_data->target_gain = volume;
		
		if (volume == 0)
			audio_data->last_gain = 1.0f;
		else
			audio_data->last_gain = volume;
	}
}

DG_BOOL dg_audio_loop() {
	int error;
	DG_BOOL keep_processing = DG_YES;
    DG_AUDIO* audio_data;
	
	if (active) {
		int idx, num_channels = dg_array_count(channels);
		
		for (idx = 0; idx < num_channels; idx++) {
			DG_OBJECT* audio = dg_array_get_object(channels, idx);
			audio_data = _toaudio(audio);
			
			if (audio_data) {
				if (audio_data->loaded) {
					if (dg_audio_is_playing(audio)) {
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
											dg_log_error(DG_MOD_AUDIO, "Generic error");
										else
											break;
								}
								
								if (size == 0) {
									if (audio_data->loop)
										ov_raw_seek(&audio_data->oggStream, 0);
									else {
										dg_audio_stop(audio); // Shouldn't stop before processing the buffers
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
			dg_log_error(DG_MOD_AUDIO, "OpenAL error was raised: %d - at index: %d", error, idx);
	}
	
	return active;
}

void dg_audio_pause(DG_OBJECT* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {	
		if (dg_audio_is_playing(audio)) {
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

void dg_audio_play(DG_OBJECT* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {		
		int i;
		
		if (dg_audio_is_playing(audio) || !audio_data->loaded)
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
						dg_log_error(DG_MOD_AUDIO, "Generic error");
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
		
		if (!dg_audio_is_playing(audio))
			dg_log_error(DG_MOD_AUDIO, "Ogg refused to play");
	}
}

void dg_audio_queue(unsigned int channel, const char* file) {
    // TODO: Must implement
	// This is now a different object
}

void dg_audio_terminate() {
	int idx, num_channels = dg_array_count(channels);

	active = DG_NO;
	
	for (idx = 0; idx < num_channels; idx++) {
		DG_OBJECT* audio = dg_array_get_object(channels, idx);
		DG_AUDIO* audio_data = _toaudio(audio);
	
		if (audio_data) {
			if (audio_data->loaded)
				dg_audio_unload(audio);
		}
	}

	alcMakeContextCurrent(NULL);
	alcDestroyContext(pContext);
	alcCloseDevice(pDevice);
}

void dg_audio_stop(DG_OBJECT* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {	
		if (dg_audio_is_playing(audio)) {
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

void dg_audio_unload(DG_OBJECT* audio) {
	DG_AUDIO* audio_data = _toaudio(audio);
	
	if (audio_data) {	
		audio_data->loaded = DG_NO;
		
		dg_audio_stop(audio);
		
		alDeleteSources(1, &audio_data->source);
		alDeleteBuffers(1, audio_data->buffers);
		
		ov_clear(&audio_data->oggStream);
		fclose(audio_data->oggFile);
	}
}

////////////////////////////////////////////////////////////////////////////////
///// Private Functions													   /////
////////////////////////////////////////////////////////////////////////////////

DG_AUDIO* _toaudio(DG_OBJECT* audio) {
	if (dg_object_check(audio, DG_OBJ_AUDIO)) {
		DG_AUDIO* audio_data = (DG_AUDIO*)dg_object_data(audio);
		return audio_data;
	}
	else {
		// raise error
		return NULL;
	}
}
