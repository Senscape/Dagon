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
 *  60 - Font
 *  70 - Audio
 *  80 - Video
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
#define DGMsg250002 "Table with coordinates expected to link node"
#define DGMsg250003 "Error loading script file"
#define DGMsg250004 "Table with coordinates expected to create spot"
#define DGMsg250005 "Cannot switch to object"
#define DGMsg250006 "Expected function as second parameter"
#define DGMsg250007 "Runtime error"
#define DGMsg250008 "Memory allocation error"
#define DGMsg250009 "Error while executing callback"
#define DGMsg250010 "Syntax error"
#define DGMsg250011 "Function expected as second parameter in register()"

// Font module
#define DGMsg060000 "Initializing font manager..."
#define DGMsg060001 "Freetype version"
#define DGMsg260002 "Failed to initialize font library"
#define DGMsg260003 "Requested font is corrupt"
#define DGMsg260004 "Default font is corrupt"
#define DGMsg260005 "Error loading glyph"
#define DGMsg260006 "Error getting glyph"

// Audio module
#define DGMsg070000 "Initializing audio manager..."
#define DGMsg270001 "Failed to initialize OpenAL device"
#define DGMsg270002 "Failed to create OpenAL context"
#define DGMsg270003 "OpenAL error"
#define DGMsg270004 "Streaming error"
#define DGMsg270005 "File not found"
#define DGMsg270006 "Unsupported number of channels in file"
#define DGMsg270007 "Unable to initialize Ogg callbacks"

// Video module
#define DGMsg080000 "Initializing video manager..."
#define DGMsg080001 "Loading"
#define DGMsg280002 "File not found"
#define DGMsg280003 "Error parsing stream headers"
#define DGMsg280004 "End of file while searching for codec headers"
#define DGMsg280005 "Resource not set in video object"

#endif

///////////////////////////////

#endif // DG_LANGUAGE_H
