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

/*
 *	Messages structure: kStringMMNNN
 *	where MM = Module and NNN = Number
 *
 *	Module:
 *
 *	00 - Generic
 *	10 - Texture
 *	11 - Render
 *  12 - Control
 *  13 - System
 *  14 - Script
 *  15 - Font
 *  16 - Audio
 *  17 - Video
 *
 */

#ifndef DAGON_LANGUAGE_H_
#define	DAGON_LANGUAGE_H_

#define English

/////////// English ///////////

#ifdef English

// Texture module
#define kString10001 "File not found"
#define kString10002 "Error while loading image"
#define kString10003 "Error while loading compressed image"
#define kString10004 "Unsupported number of channels in image"
#define kString10005 "No resource found for texture"

// Render module
#define kString11001 "Initializing renderer..."
#define kString11002 "OpenGL version"
#define kString11003 "Visual effects not supported on this system"
#define kString11004 "Could not create framebuffer"
#define kString11005 "GLEW version"
#define kString11006 "OpenGL error"

// Control module
#define kString12001 "Dagon version"
#define kString12002 "Initializing main controller..."
#define kString12003 "Initializing console..."
#define kString12004 "Build"
#define kString12005 "No nodes to load in room"
#define kString12006 "No spots to draw"
#define kString12007 "Creating objects in runtime is not recommended"
#define kString12008 "Node not added to current room"
#define kString12009 "No current room specified"
#define kString12010 "Slide not added to current room"

// Funny messages when shutting down
#define kString12100 "Shutdown complete"
#define kString12101 "GAME OVER"
#define kString12102 "You'll be back..."
#define kString12103 "I'm afraid, Dave"

// System module
#define kString13001 "Initializing system..."
#define kString13002 "System ready!"
#define kString13003 "SDL version"
#define kString13004 "System already initialized"
#define kString13005 "System not initialized"
#define kString13006 "Could not set vertical sync"
#define kString13007 "Could not create controller thread"
#define kString13008 "Could not enter fullscreen"
#define kString13009 "Could not exit fullscreen"
#define kString13010 "Could not create window"

// Script module
#define kString14001 "Initializing script..."
#define kString14002 "Lua version"
#define kString14003 "Switch: Not a room or node"
#define kString14004 "Switch: Not an object"
#define kString14005 "Table with coordinates expected to link node"
#define kString14006 "Error loading script file"
#define kString14007 "Table with coordinates expected to create spot"
#define kString14008 "Cannot switch to object"
#define kString14009 "Expected function as second parameter"
#define kString14010 "Runtime error"
#define kString14011 "Memory allocation error"
#define kString14012 "Error while executing callback"
#define kString14013 "Syntax error"
#define kString14014 "Function expected as second parameter in register()"
#define kString14015 "Bad configuration file"

// Font module
#define kString15001 "Initializing font manager..."
#define kString15002 "Freetype version"
#define kString15003 "Failed to initialize font library"
#define kString15004 "Requested font is corrupt"
#define kString15005 "Default font is corrupt"
#define kString15006 "Error loading glyph"
#define kString15007 "Error getting glyph"

// Audio module
#define kString16001 "Initializing audio manager..."
#define kString16002 "OpenAL version"
#define kString16003 "Vorbis version"
#define kString16004 "Failed to initialize OpenAL device"
#define kString16005 "Failed to create OpenAL context"
#define kString16006 "OpenAL error"
#define kString16007 "Streaming error"
#define kString16008 "File not found"
#define kString16009 "Unsupported number of channels in file"
#define kString16010 "Unable to initialize Ogg callbacks"

// Video module
#define kString17001 "Initializing video manager..."
#define kString17002 "Loading"
#define kString17003 "Enumerating devices..."
#define kString17004 "Opening default device"
#define kString17005 "Opening device"
#define kString17006 "Theora version"
#define kString17007 "File not found"
#define kString17008 "Error parsing stream headers"
#define kString17009 "End of file while searching for codec headers"
#define kString17010 "Resource not set in video object"

// SDL errors
#define kString18001 "Could not create mutex"
#define kString18002 "Could not lock mutex"
#define kString18003 "Failed to create thread"

#endif // English

///////////////////////////////

#endif // DAGON_LANGUAGE_H_
