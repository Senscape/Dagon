/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_EFFECTS_H
#define	DG_EFFECTS_H

#include "DGCommon.h"


enum DG_Effects {
	DG_FX_BLUR,
	DG_FX_COLORIZE,
	DG_FX_CONTRAST,
	DG_FX_CONVOLUTION,
	DG_FX_NOISE,
	DG_FX_SHARPEN
};

void	dg_effects_init(void);
void	dg_effects_pause(void);
void	dg_effects_play(void);
void	dg_effects_release(void);
void	dg_effects_set_blur(int offsetx, int offsety);
void    dg_effects_set_gamma(float level);

#endif // DG_EFFECTS_H
