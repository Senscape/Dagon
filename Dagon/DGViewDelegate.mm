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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#import "DGControl.h"
#import "DGPlatform.h"
#import "DGViewDelegate.h"

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

@implementation DGViewDelegate

- (id)initWithFrame:(NSRect)frame {
    config = &DGConfig::getInstance();
    control = &DGControl::getInstance();
    
	// Pixel Format Attributes for the View-based (Non-FullScreen) NSOpenGLContext
    NSOpenGLPixelFormatAttribute attrs[] = {
		
        // Specifying "NoRecovery" gives us a context that cannot fall back to the software renderer.
        // This makes the View-based context a compatible with the fullscreen context, enabling us to use
        // the "shareContext" feature to share textures, display lists, and other OpenGL objects between the two.
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
		
		if (config->verticalSync) {
            GLint swapInterval = 1;
            [glContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
        }
        
        NSNotificationCenter* notificationCenter = [NSNotificationCenter defaultCenter];
        
        [notificationCenter addObserver: self
                               selector: @selector(_windowWillClose:)
                                   name: NSWindowWillCloseNotification
                                 object: _window];
    }
	
    return self;
}

- (void)dealloc {
	[glContext release];
	[pixelFormat release];
	[super dealloc];
}

- (void)keyDown:(NSEvent *)theEvent {
    int keyCode;
    
	if ([theEvent modifierFlags] & NSFunctionKeyMask) {
		keyCode = [theEvent keyCode];
		control->processKey(keyCode, false);
	}
	else {
        keyCode = *[[theEvent charactersIgnoringModifiers] UTF8String];
        if ([theEvent modifierFlags] & NSCommandKeyMask)
            control->processKey(keyCode, true);
        else
            control->processKey(keyCode, false);
	}
}

- (void)mouseDown:(NSEvent *)theEvent {
	NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	bool isInside = [self mouse:mouseLoc inRect:[self bounds]];
	
	if (isInside)
		control->processMouse(mouseLoc.x, mouseLoc.y, true);
}

- (void)mouseMoved:(NSEvent *)theEvent {
	NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	bool isInside = [self mouse:mouseLoc inRect:[self bounds]];
	
	if (isInside) {
		control->processMouse(mouseLoc.x, mouseLoc.y, false);
		
		[NSCursor hide];
	}
	else {
        control->processMouse(config->displayWidth/2, config->displayHeight/2, false);
        
		[NSCursor unhide];
    }
}

- (void)update {
    control->update();
	[glContext flushBuffer];
}

- (void)viewWillDraw {
	NSSize size = [[self superview] bounds].size;
	BOOL setCtx = [NSOpenGLContext currentContext] != glContext;
	
	[glContext update];
	[glContext setView:self];
    
	if (setCtx)
		[glContext makeCurrentContext];
	
    control->reshape(size.width, size.height);
    
	if (setCtx)
		[NSOpenGLContext clearCurrentContext];
    
    // TODO: Ensure this is thread-safe
    // (It probably is but everything is being drawn twice!)
    [self update];
    [super viewWillDraw];
} 

// FIXME: This notification also comes when closing the About dialog
- (void) _windowWillClose:(NSNotification *)notification {
    // Simulate the ESC key
    control->processKey(DGKeyEsc, false);
}

@end
