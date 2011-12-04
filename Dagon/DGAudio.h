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


// Init / Terminate

void		dg_audio_init(void);
void		dg_audio_terminate(void);

// New / Release

DG_OBJECT*	dg_audio_new(const char* name, DG_BOOL loop);
void		dg_audio_release(DG_OBJECT* audio);

// Load / Unload

void		dg_audio_load(DG_OBJECT* audio);
void		dg_audio_unload(DG_OBJECT* audio);

// Checks

DG_BOOL		dg_audio_is_loaded(DG_OBJECT* audio);
DG_BOOL		dg_audio_is_playing(DG_OBJECT* audio);

// Sets

void		dg_audio_fade_in(DG_OBJECT* audio, int msecs);
void		dg_audio_fade_out(DG_OBJECT* audio, int msecs);
void		dg_audio_set_volume(DG_OBJECT* audio, float volume);

// State Changes

DG_BOOL		dg_audio_loop(void);
void		dg_audio_pause(DG_OBJECT* audio);
void		dg_audio_play(DG_OBJECT* audio);
void        dg_audio_queue(unsigned int channel, const char* file);
void		dg_audio_stop(DG_OBJECT* audio);

#endif // DG_AUDIO_H
