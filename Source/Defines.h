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

#ifndef DAGON_DEFINES_H_
#define	DAGON_DEFINES_H_

// Defaults

#define	kDefScriptFile "Main" // Also name of the window
#define	kDefAudioPath "Audio/"
#define	kDefCursorPath "Cursors/"
#define	kDefFontPath "Fonts/"
#define	kDefImagePath "Images/"
#define	kDefNodePath "Nodes/"
#define	kDefRoomPath "Rooms/"
#define	kDefVideoPath "Video/"
#define	kDefResourcePath "Resources/"
#define	kDefConfigFile "Dagon.cfg"
#define	kDefLogFile "Dagon.log"
#define kDefTexExtension "tex"

// TODO: We should be able to read this value from each texture bundle
#define kDefTexSize 2048
#define kFrameratePrecision 0.25

// Maximums

enum MaxValues {
  kMaxFeedLength = 512,
  kMaxFileLength = 256,
  kMaxLogLength = 256,
  kMaxLogHistory = 80,
  kMaxObjectName = 80,
  kMaxPathLength = 256,
  kMaxSubPathLength = 32
};

// We use this macro to load files sequentially

#define kFileSeqStart 1
#define	kFileSeqDigits 3

#define mkstr(a) # a
#define in_between(a) mkstr(a)

enum CursorTypes {
  kCursorNormal = 0,
  kCursorDrag,
  kCursorLeft,
  kCursorRight,
  kCursorUp,
  kCursorDown,
  kCursorUpLeft,
  kCursorUpRight,
  kCursorDownLeft,
  kCursorDownRight,
  kCursorForward,
  kCursorBackward,
  kCursorUse,
  kCursorLook,
  kCursorTalk,
  kCursorCustom
};

// The first six elements are the same as the faces of the cube.
// We map these to the positions in arrays, so we must be
// extra-careful about their values.

enum Directions {
	kNorth = 0,
	kEast,
	kSouth,
	kWest,
  kUp,
  kDown,
	kNorthEast,
	kSouthEast,
	kSouthWest,
	kNorthWest,
  kCurrent = 0xFF // Maintain current direction
};

#endif // DAGON_DEFINES_H_
