////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2013 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DG_DEFINES_H
#define	DG_DEFINES_H

// Defaults

#define	DGDefScriptFile     "Main" // Also name of the window
#define	DGDefAudioPath      "Audio/"
#define	DGDefCursorPath     "Cursors/"
#define	DGDefFontPath       "Fonts/"
#define	DGDefImagePath      "Images/"
#define	DGDefNodePath       "Nodes/"
#define	DGDefRoomPath       "Rooms/"
#define	DGDefVideoPath      "Video/"
#define	DGDefResourcePath	"Resources/"
#define	DGDefConfigFile		"Dagon.cfg"
#define	DGDefLogFile		"Dagon.log"
#define DGDefTexExtension	"tex"
#define DGDefTexSize		2048 // TODO: We should be able to read this value from each texture bundle

// Maximums

#define DGMaxFeedLength		512
#define DGMaxFileLength		256
#define DGMaxLogLength		256
#define DGMaxLogHistory		80
#define	DGMaxObjectName		80
#define	DGMaxPathLength		256
#define	DGMaxSubPathLength	32

// Others

#define DGFrameratePrecision 0.25

#define DGFileSeqStart      1
#define	DGFileSeqDigits     3

#define mkstr(a) # a
#define in_between(a) mkstr(a)

enum DGCursorTypes {
    DGCursorNormal = 0,
    DGCursorDrag,
    DGCursorLeft,
    DGCursorRight,
    DGCursorUp,
    DGCursorDown,
    DGCursorUpLeft,
    DGCursorUpRight,
    DGCursorDownLeft,
    DGCursorDownRight,
    DGCursorForward,
    DGCursorBackward,
    DGCursorUse,
    DGCursorLook,
    DGCursorTalk,
    DGCursorCustom
};

// The first six elements are the same as the faces of the cube
// We map these to the positions in arrays, so we must be extra-careful about their values

enum DGDirections {
	DGNorth = 0,
	DGEast = 1,
	DGSouth = 2,
	DGWest = 3,
    DGUp = 4,
    DGDown = 5,
	DGNorthEast,
	DGSouthEast,
	DGSouthWest,
	DGNorthWest,
    DGCurrent = 0xFF // Maintain current direction
};

#endif // DG_DEFINES_H
