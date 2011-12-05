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


enum DG_EFFECTS {
	DG_EFFECT_BLUR,
	DG_EFFECT_COLORIZE,
	DG_EFFECT_CONTRAST,
	DG_EFFECT_CONVOLUTION,
	DG_EFFECT_NOISE,
	DG_EFFECT_SHARPEN
};

void	DGEffectsInit(void);
void	DGEffectsPause(void);
void	DGEffectsPlay(void);
void	DGEffectsSetBlur(int horizontalOffset, int verticalOffset);
void    DGEffectsSetGamma(float intensity);
void	DGEffectsTerminate(void);

#endif // DG_EFFECTS_H
