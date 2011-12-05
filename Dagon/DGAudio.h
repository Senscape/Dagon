/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_AUDIO_H
#define	DG_AUDIO_H

#include "DGCommon.h"
#include "DGObject.h"

// We should have a separate audio manager for all the audio objects...
// It's currently all mixed up here

// Init / Terminate

void		DGAudioInitialize(void);
void		DGAudioTerminate(void);

// New / Release

DGObject*	DGAudioNew(const char* withName, DGBool isLoopable);
void		DGAudioRelease(DGObject* audio);

// Load / Unload

void		DGAudioLoad(DGObject* audio);
void		DGAudioUnload(DGObject* audio);

// Checks

DGBool		DGAudioIsLoaded(DGObject* audio);
DGBool		DGAudioIsPlaying(DGObject* audio);

// Sets

void		DGAudioFadeIn(DGObject* audio, int forMilliseconds);
void		DGAudioFadeOut(DGObject* audio, int forMilliseconds);
void		DGAudioSetVolume(DGObject* audio, float theVolume);

// State Changes

DGBool		DGAudioLoop(void);
void		DGAudioPause(DGObject* audio);
void		DGAudioPlay(DGObject* audio);
void        DGAudioQueue(unsigned int inChannel, DGObject* anAudio);
void		DGAudioStop(DGObject* audio);

#endif // DG_AUDIO_H
