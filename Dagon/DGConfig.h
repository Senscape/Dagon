/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_CONFIG_H
#define	DG_CONFIG_H

#include "DGCommon.h"


enum DG_Config_Defaults {
	DG_DEF_ANTIALIASING = DG_YES,
	DG_DEF_DISPLAYWIDTH = 1024,
	DG_DEF_DISPLAYHEIGHT = 768,
	DG_DEF_DISPLAYDEPTH = 32,
	DG_DEF_DEBUGMODE = DG_YES, // This shouldn't default to yes, careful while finding paths
    DG_DEF_DUST = DG_NO,
	DG_DEF_FRAMERATE = 60,
	DG_DEF_FULLSCREEN = DG_NO,
	DG_DEF_EFFECTS = DG_YES,
	DG_DEF_LOG = DG_YES,
	DG_DEF_MULTITHREAD = DG_NO, // NOT SUPPORTED YET!!
    DG_DEF_SCARE = DG_NO,
	DG_DEF_SHOWSPLASH = DG_YES,
	DG_DEF_SHOWSPOTS = DG_NO,
	DG_DEF_TEXCOMPRESSION = DG_YES,
	DG_DEF_THROB = DG_NO,    
	DG_DEF_VERTICALSYNC = DG_YES
};

enum DG_Paths {
	DG_APP,
    DG_RES,
    DG_USER
};

typedef struct {
	char script[DG_MAX_PATHLEN];
    char app_path[DG_MAX_PATHLEN];
	char res_path[DG_MAX_PATHLEN];
	char user_path[DG_MAX_PATHLEN];
	
	int display_width;
	int display_height;
	int display_depth;
	
    DG_BOOL dust;
	DG_BOOL effects;
	int framerate;
	DG_BOOL fullscreen;
	float global_speed;
	DG_BOOL tex_compression;
	char tex_extension[4];
    DG_BOOL scare;
    DG_BOOL throb;
	DG_BOOL vertical_sync;
	
	DG_BOOL antialiasing;
	DG_BOOL debug_mode;
	DG_BOOL log;
	DG_BOOL multithread;
	DG_BOOL show_splash;
	DG_BOOL show_spots;
} DG_CONFIG;

extern DG_CONFIG config;

void			dg_config_init(void);
void			dg_config_finalize(void);
const char*		dg_config_get_path(int type, const char* file);

#endif // DG_CONFIG_H
