/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#import "DGPlatform.h"


@interface DGViewDelegate : NSView {
	NSOpenGLPixelFormat* pixelFormat;
	NSOpenGLContext* glContext;
}

- (void)update;

@end
