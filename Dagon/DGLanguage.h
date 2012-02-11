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

/*
 *	Messages structure: TNNMMM
 *	where T = type, NN = category and MMM = number
 * 
 *	Types:
 *
 *	0 = trace
 *	1 = warning
 *	2 = error
 *
 *	Category:
 *
 *	00 - Generic
 *	10 - Texture
 *	20 - Render
 *  30 - Control
 *  40 - System
 *  50 - Script
 *
 */

#ifndef DG_LANGUAGE_H
#define	DG_LANGUAGE_H

#define English

/////////// English ///////////

#ifdef English

// Texture module
#define DGMsg210000	"File not found"
#define DGMsg210001	"Error while loading image"
#define DGMsg210002	"Error while loading compressed image"
#define DGMsg210003	"Unsupported number of channels in image"
#define DGMsg210004	"No resource found for texture"

// Render module
#define DGMsg020000 "Initializing renderer..."
#define DGMsg020001 "OpenGL version"
#define DGMsg020002 "Visual effects not supported on this system"
#define DGMsg220001	"OpenGL error"

// Control module
#define DGMsg030000 "Dagon version"
#define DGMsg030001 "Initializing main controller..."
#define DGMsg130000 "No nodes to load in room"
#define DGMsg130001 "No spots to draw"
#define DGMsg230000 "Node not added to current room"
#define DGMsg230001 "No current room specified"

// System module

#define DGMsg040000 "Initializing system..."
#define DGMsg040001 "System ready!"
#define DGMsg140002 "System already initialized"
#define DGMsg240003 "Could not set vertical sync"
#define DGMsg240004 "Could not create controller thread"
#define DGMsg240005 "Could not enter fullscreen"
#define DGMsg240006 "Could not exit fullscreen"

// Funny messages when shutting down

#define DGMsg040100 "Shutdown complete"
#define DGMsg040101 "GAME OVER"
#define DGMsg040102 "You'll be back..."
#define DGMsg040103 "I'm afraid, Dave"

// Script module

#define DGMsg250000 "Switch: Not a room or node"
#define DGMsg250001 "Switch: Not an object"
#define DGMsg250002 "Link: Table with coordinates expected to link node"
#define DGMsg250003 "Error loading script file"

#endif

///////////////////////////////

#endif // DG_LANGUAGE_H
