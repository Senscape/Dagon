/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGConfig.h"
#include "DGLog.h"


DG_CONFIG config;

void dg_config_init() {	
	strcpy(config.script, DG_DEF_SCRIPTFILE);
    
    strcpy(config.app_path, "");
    strcpy(config.res_path, "catalog/");
    strcpy(config.user_path, "");
	
	config.antialiasing = DG_DEF_ANTIALIASING;
	config.display_width = DG_DEF_DISPLAYWIDTH;
	config.display_height = DG_DEF_DISPLAYHEIGHT;
	config.display_depth = DG_DEF_DISPLAYDEPTH;
	config.debug_mode = DG_DEF_DEBUGMODE;
	config.dust = DG_DEF_DUST;
	config.fullscreen = DG_DEF_FULLSCREEN;
	config.effects = DG_DEF_EFFECTS;
	config.log = DG_DEF_LOG;
	config.multithread = DG_DEF_MULTITHREAD;
	config.framerate = DG_DEF_FRAMERATE;
	config.show_splash = DG_DEF_SHOWSPLASH;
	config.show_spots = DG_DEF_SHOWSPOTS;
	config.tex_compression = DG_DEF_TEXCOMPRESSION;
	strcpy(config.tex_extension, DG_DEF_EXTENSION);
    config.scare = DG_DEF_SCARE;
    config.throb = DG_DEF_THROB;
	config.vertical_sync = DG_DEF_VERTICALSYNC;
	
	//TODO: If vertical sync is enabled this should not be necessary!
	// This is a readonly setting
	config.global_speed = 60.0f / (float)config.framerate;
}

void dg_config_finalize() {
	fclose(stdout);
	fclose(stderr);
}

const char*	dg_config_get_path(int type, const char* file) {
	static char full_path[DG_MAX_PATHLEN];
	
    switch (type) {
        case DG_APP:
            strcpy(full_path, config.app_path);
            break;
        case DG_RES:
            strcpy(full_path, config.res_path);
            break;
        case DG_USER:
            strcpy(full_path, config.user_path);
            break;
    }
	
	strcat(full_path, file);
	
	return full_path;
}
