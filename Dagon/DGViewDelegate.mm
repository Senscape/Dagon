////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
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

#import "DGConfig.h"
#import "DGControl.h"
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
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFADepthSize, 32,
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
        
        [[NSColor blackColor] setFill];
        NSRectFill(frame);
    }
	
    return self;
}

- (void)dealloc {
	[glContext release];
	[pixelFormat release];
	[super dealloc];
}

- (BOOL)isFlipped {
    return YES;
}

- (void)keyDown:(NSEvent *)theEvent {
    int keyCode;
    
	if ([theEvent modifierFlags] & NSFunctionKeyMask) {
		keyCode = [theEvent keyCode];
		control->processFunctionKey(keyCode);
	}
	else {
        keyCode = *[[theEvent charactersIgnoringModifiers] UTF8String];
        if ([theEvent modifierFlags] & NSCommandKeyMask)
            control->processKey(keyCode, DGKeyEventModified);
        else
            control->processKey(keyCode, DGKeyEventDown);
	}
}

- (void)keyUp:(NSEvent *)theEvent {
    int keyCode;
    
    keyCode = *[[theEvent charactersIgnoringModifiers] UTF8String];
    control->processKey(keyCode, DGKeyEventUp);
}

- (void)mouseEntered:(NSEvent *)theEvent {
    [NSCursor hide];
}

- (void)mouseExited:(NSEvent *)theEvent {
    // We do this only when the Free mode is on
    if (config->controlMode == DGMouseFree)
        control->processMouse(config->displayWidth / 2, config->displayHeight / 2, DGMouseEventMove);
    [NSCursor unhide];
}

- (void)mouseDown:(NSEvent *)theEvent {
    if (isMouseInside) {
        NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        control->processMouse(mouseLoc.x, mouseLoc.y, DGMouseEventDown);
    }
}

- (void)mouseDragged:(NSEvent *)theEvent {
    if (isMouseInside) {
        NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        control->processMouse(mouseLoc.x, mouseLoc.y, DGMouseEventDrag);
    }
}

- (void)mouseMoved:(NSEvent *)theEvent {
    static bool shouldHideCursor = false;
    NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    isMouseInside = ([self hitTest:mouseLoc] == self);
    if (isMouseInside)
        control->processMouse(mouseLoc.x, mouseLoc.y, DGMouseEventMove);
    
    if (config->fullScreen) {
        // Left border
        if (mouseLoc.x < 2) {
            shouldHideCursor = true;
        }
        // Right border
        else if (mouseLoc.x > config->displayWidth - 2) {
            shouldHideCursor = true;
        }
        // Bottom border
        else if (mouseLoc.y > config->displayHeight - 2) {
            shouldHideCursor = true;
        }
        else {
            if (shouldHideCursor) {
                [NSCursor hide];
                shouldHideCursor = false;
            }
        }
    }
}

- (void)mouseUp:(NSEvent *)theEvent {
    if (isMouseInside) {
        NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        control->processMouse(mouseLoc.x, mouseLoc.y, DGMouseEventUp);
    }
}

- (void)rightMouseDown:(NSEvent *)theEvent {
    if (isMouseInside) {
        NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        control->processMouse(mouseLoc.x, mouseLoc.y, DGMouseEventRightDown);
    }
}

- (void)rightMouseUp:(NSEvent *)theEvent {
    if (isMouseInside) {
        NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        control->processMouse(mouseLoc.x, mouseLoc.y, DGMouseEventRightUp);
    }
}

- (void)scrollWheel:(NSEvent *)theEvent {
    if (isMouseInside) {
        // TODO: Flawed experiment, should implement this
        /*float x = [theEvent deltaX];
        float y = [theEvent deltaY];
        NSLog(@"(%0.0f, %0.0f)", x, y);*/
        // This should modify the camera directly, the panning method more precisely
    }
}

- (void)update {    
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
    
    [self removeTrackingRect:boundsTrackingTag];
    boundsTrackingTag = [self addTrackingRect:[self visibleRect] owner:self userData:nil assumeInside:isMouseInside];
    
    // TODO: Ensure this is thread-safe for future versions
    control->update();
    
    [super viewWillDraw];
}

@end
