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


enum DG_CONFIG_DEFAULTS {
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

enum DG_PATHS {
	DG_PATH_APP,
    DG_PATH_RES,
    DG_PATH_USER
};

typedef struct {
	char script[DG_MAX_PATHLEN];
    char app_path[DG_MAX_PATHLEN];
	char res_path[DG_MAX_PATHLEN];
	char user_path[DG_MAX_PATHLEN];
	
	int display_width;
	int display_height;
	int display_depth;
	
    DGBool dust;
	DGBool effects;
	int framerate;
	DGBool fullscreen;
	float global_speed;
	DGBool tex_compression;
	char tex_extension[4];
    DGBool scare;
    DGBool throb;
	DGBool vertical_sync;
	
	DGBool antialiasing;
	DGBool debug_mode;
	DGBool log;
	DGBool multithread;
	DGBool show_splash;
	DGBool show_spots;
} DG_CONFIG;

extern DG_CONFIG DGConfig;

void			DGConfigInitialize(void);
void			DGConfigTerminate(void);
const char*		DGConfigGetPath(int withType, const char* forFile);

#endif // DG_CONFIG_H
