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

#import "DGConfig.h"
#import "DGSystem.h"
#import "DGWindowDelegate.h"

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

@implementation DGWindowDelegate

- (id) init {
    self = [super init];
    if (self) {
        config = &DGConfig::getInstance();
        system = &DGSystem::getInstance();
    }
	
    return self;
}

- (void)dealloc {
    [super dealloc];
}

- (void) windowWillClose:(NSNotification *)notification {
    system->terminate(); // Hard shutdown
}

- (void) windowWillEnterFullScreen:(NSNotification *)notification {
    config->fullScreen = true;
    [NSCursor hide];
    
    [[NSApplication sharedApplication]
     setPresentationOptions: NSApplicationPresentationHideMenuBar
     | NSApplicationPresentationHideDock];
}

- (void) windowDidExitFullScreen:(NSNotification *)notification {
    config->fullScreen = false;
    [NSCursor unhide];
    
    [[NSApplication sharedApplication]
     setPresentationOptions: NSApplicationPresentationDefault];
}

@end
