/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

/*
 *	Messages structure: TNNMMM
 *	where T = type, NN = category and MMM = number
 * 
 *	Types:
 *
 *	0 = normal message
 *	1 = warning
 *	2 = error
 *
 *	Category:
 *
 *	00 - Generic
 *	10 - File
 *	20 - Render
 *
 */

#ifndef DG_LANGUAGE_H
#define	DG_LANGUAGE_H

#define ENGLISH

/////////// English ///////////

#ifdef ENGLISH

#define MSG210000	"File not found:"
#define MSG210001	"Error while loading image:"
#define MSG210002	"Error while loading compressed image:"
#define MSG210003	"Unsupported number of channels in image:"

#define MSG220001	"OpenGL error:"

#endif

///////////////////////////////

#endif // DG_LANGUAGE_H
