////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

#ifndef DG_DEFINES_H
#define	DG_DEFINES_H

// Defaults

#define	DGDefScriptFile     "main" // Also window name
#define	DGDefCatalogPath	"catalog/"
#define	DGDefConfigFile		"dagon.cfg"
#define	DGDefLogFile		"dagon.log"
#define DGDefTexExtension	"tex"
#define DGDefTexSize		2048 // TODO: We should be able to read this value from each texture

// Maximums

#define DGMaxFeedback		512
#define DGMaxFileLength		256
#define DGMaxLogLength		256
#define DGMaxLogHistory		80
#define	DGMaxObjectName		80
#define	DGMaxPathLength		512

// Others

#define DGFileSeqStart      1
#define	DGFileSeqDigits     4

#define mkstr(a) # a
#define in_between(a) mkstr(a)

// The first six elements are the same as the faces of the cube
// We map these to the positions in arrays, we must be extra-careful about their values

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
	DGNorthWest
};

#endif // DG_DEFINES_H
