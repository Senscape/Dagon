/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_COMMON_GL_H
#define	DG_COMMON_GL_H

#include "DGPlatform.h"

#if defined DG_PLATFORM_MAC

#include <GL/glew.h>

#elif defined DG_PLATFORM_IOS

#define	__QNXNTO__
#define	GLAPI
#define	APIENTRY
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <GLES/glu.h>

#elif defined DG_PLATFORM_WIN

#include <GL/glew.h>

#elif defined DG_PLATFORM_LINUX

#include <GL/glew.h>
#include <GL/glx.h>

#endif

#endif // DG_COMMON_GL_H
