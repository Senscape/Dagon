/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_DEFINES_H
#define	DG_DEFINES_H

// Defaults

#define	DG_DEF_SCRIPTFILE	"main"	// Also window name
#define	DG_DEF_LOGFILE		"dagon.log" // No extension here
#define DG_DEF_EXTENSION	"tex"
#define DG_DEF_TEXSIZE		2048 // TODO: We should be able to read this value from each texture

// Maximums

#define DG_MAX_FEEDBACK		512
#define DG_MAX_FILELEN		80
#define DG_MAX_LOGLEN		256
#define DG_MAX_LOGHIST		80
#define	DG_MAX_OBJNAME		80
#define	DG_MAX_PATHLEN		256

// Others

#define DG_FILESEQSTART		1
#define	DG_FILESEQZERO		4

#define mkstr(a) # a
#define in_between(a) mkstr(a)

// Since we map these to the positions in arrays, we must be extra-careful about their values
enum DG_Cube_Faces {
	DG_FRONT = 0,
	DG_RIGHT = 1,
	DG_BACK = 2,
	DG_LEFT = 3,
	DG_TOP = 4,
	DG_BOTTOM = 5
};

// The first four elements are the same as the faces of the cube

enum DG_Directions {
	DG_NORTH,
	DG_EAST,
	DG_SOUTH,
	DG_WEST,
	DG_NORTHEAST,
	DG_SOUTHEAST,
	DG_SOUTHWEST,
	DG_NORTHWEST
};

enum DG_Spot_Flags {
	DG_NONE, // This one could be global
	DG_HAS_ACTION,
	DG_HAS_AUDIO,
	DG_HAS_IMAGE,
	DG_HAS_VIDEO
};

// Default state is node unless a menu is specified

enum DG_Global_States {
	DG_ST_CUTSCENE,
	DG_ST_INVENTORY,
	DG_ST_MENU,
	DG_ST_NODE,
    DG_ST_SPLASH,
	DG_ST_TIMER,
	DG_ST_VIDEO,
	DG_ST_ZOOM
};

#endif // DG_DEFINES_H
