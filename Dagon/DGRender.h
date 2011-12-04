/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_RENDER_H
#define	DG_RENDER_H

#include "DGCommon.h"
#include "DGObject.h"


// Init / Release

void		dg_render_init(void);
void		dg_render_release(void);

// State Changes

void		dg_render_begin(DG_BOOL with_textures);
void		dg_render_blend(void);
void		dg_render_clear(void);
void		dg_render_copy_scene(void);
void		dg_render_cutscene(DG_OBJECT* video);
void		dg_render_draw_cursor(short x, short y);
void		dg_render_draw_poly(short* coords, short elements, short face, DG_BOOL invert);
void		dg_render_draw_quad(short* coords, DG_BOOL invert);
void		dg_render_dust(void);
void		dg_render_end(void);
void		dg_render_save_scene(void);
void		dg_render_set_splash(void);
void		dg_render_splash(void);
int			dg_render_test_color(int x, int y);
void		dg_render_update_scene(void);
void		dg_render_with_color(int color);

extern const unsigned char _binary_def_particle_start[];
extern const unsigned char _binary_def_splash_start[];

#endif // DG_RENDER_H
