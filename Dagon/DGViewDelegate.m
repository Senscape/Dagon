/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#import "DGConfig.h"
#import "DGControl.h"
#import "DGViewDelegate.h"


@implementation DGViewDelegate

- (id)initWithFrame:(NSRect)frame {
	// Pixel Format Attributes for the View-based (Non-FullScreen) NSOpenGLContext
    NSOpenGLPixelFormatAttribute attrs[] = {
		
        // Specifying "NoRecovery" gives us a context that cannot fall back to the software renderer.  This makes the View-based context a compatible with the fullscreen context, enabling us to use the "shareContext" feature to share textures, display lists, and other OpenGL objects between the two.
        NSOpenGLPFANoRecovery,
		
        // Attributes Common to FullScreen and Non-FullScreen
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFADepthSize, 16,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };
	
    self = [super initWithFrame:frame];
    if (self) {
		// Create our Non-FullScreen pixel format.
		pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
		glContext   = [[NSOpenGLContext alloc] initWithFormat:
					   pixelFormat shareContext: nil];
		
		[glContext makeCurrentContext];
		
		// TODO: This should be configurable
		GLint swapInterval = 1;
		[glContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
    }
	
    return self;
}

- (void)dealloc {
	[glContext release];
	[pixelFormat release];
	[super dealloc];
}

- (void)update {
	[glContext setView: self];
	[glContext flushBuffer];
}

- (void)keyDown:(NSEvent *)theEvent {
	if ([theEvent modifierFlags] & NSFunctionKeyMask) {
		int keyCode = [theEvent keyCode];
		DGControlProcessKey(keyCode);
	}
	else {
        const char* keyCode = [[theEvent charactersIgnoringModifiers] UTF8String];
        if ([theEvent modifierFlags] & NSCommandKeyMask)
            DGControlProcessCharKey(keyCode, DG_YES);
        else
            DGControlProcessCharKey(keyCode, DG_NO);
	}
}

- (void)mouseDown:(NSEvent *)theEvent {
	NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	DGBool isInside = [self mouse:mouseLoc inRect:[self bounds]];
	
	if (isInside)
		DGControlProcessMouse(mouseLoc.x, mouseLoc.y, DG_YES);
}

- (void)mouseMoved:(NSEvent *)theEvent {
	NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	DGBool isInside = [self mouse:mouseLoc inRect:[self bounds]];
	
	if (isInside) {
		DGControlProcessMouse(mouseLoc.x, mouseLoc.y, DG_NO);
		
		[NSCursor hide];
	}
	else
		[NSCursor unhide];
}

- (void)reshape {
	NSSize	size = [[self superview] bounds].size;
	DGBool	setCtx = [NSOpenGLContext currentContext] != glContext;
	
	[glContext update];
	
	if (setCtx)
		[glContext makeCurrentContext];
	
	DGConfig.display_width = size.width;
	DGConfig.display_height = size.height;
	
	if (setCtx)
		[NSOpenGLContext clearCurrentContext];
}

@end
