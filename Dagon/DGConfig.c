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


DG_CONFIG DGConfig;

void DGConfigInitialize() {	
	strcpy(DGConfig.script, DG_DEF_SCRIPTFILE);
    
    strcpy(DGConfig.app_path, "");
    strcpy(DGConfig.res_path, "catalog/");
    strcpy(DGConfig.user_path, "");
	
	DGConfig.antialiasing = DG_DEF_ANTIALIASING;
	DGConfig.display_width = DG_DEF_DISPLAYWIDTH;
	DGConfig.display_height = DG_DEF_DISPLAYHEIGHT;
	DGConfig.display_depth = DG_DEF_DISPLAYDEPTH;
	DGConfig.debug_mode = DG_DEF_DEBUGMODE;
	DGConfig.dust = DG_DEF_DUST;
	DGConfig.fullscreen = DG_DEF_FULLSCREEN;
	DGConfig.effects = DG_DEF_EFFECTS;
	DGConfig.log = DG_DEF_LOG;
	DGConfig.multithread = DG_DEF_MULTITHREAD;
	DGConfig.framerate = DG_DEF_FRAMERATE;
	DGConfig.show_splash = DG_DEF_SHOWSPLASH;
	DGConfig.show_spots = DG_DEF_SHOWSPOTS;
	DGConfig.tex_compression = DG_DEF_TEXCOMPRESSION;
	strcpy(DGConfig.tex_extension, DG_DEF_EXTENSION);
    DGConfig.scare = DG_DEF_SCARE;
    DGConfig.throb = DG_DEF_THROB;
	DGConfig.vertical_sync = DG_DEF_VERTICALSYNC;
	
	//TODO: If vertical sync is enabled this should not be necessary!
	// This is a readonly setting
	DGConfig.global_speed = 60.0f / (float)DGConfig.framerate;
}

void DGConfigTerminate() {
	fclose(stdout);
	fclose(stderr);
}

const char*	DGConfigGetPath(int withType, const char* forFile) {
	static char full_path[DG_MAX_PATHLEN];
	
    switch (withType) {
        case DG_PATH_APP:
            strcpy(full_path, DGConfig.app_path);
            break;
        case DG_PATH_RES:
            strcpy(full_path, DGConfig.res_path);
            break;
        case DG_PATH_USER:
            strcpy(full_path, DGConfig.user_path);
            break;
    }
	
	strcat(full_path, forFile);
	
	return full_path;
}
