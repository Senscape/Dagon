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

- (id)init {
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

- (void)setWindow:(NSWindow *)theWindow {
    window = theWindow;
}

- (void)windowWillClose:(NSNotification *)notification {
    system->terminate(); // Hard shutdown
}

- (void)windowWillEnterFullScreen:(NSNotification *)notification {
    config->fullScreen = true;
    [NSCursor hide];
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification {
    [window makeKeyAndOrderFront:window];
}

- (void)windowDidExitFullScreen:(NSNotification *)notification {
    config->fullScreen = false;
    [NSCursor unhide];
}

- (NSApplicationPresentationOptions) window:(NSWindow *)window willUseFullScreenPresentationOptions: (NSApplicationPresentationOptions)proposedOptions {
	NSApplicationPresentationOptions customOptions = NSApplicationPresentationFullScreen | NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar;
	return customOptions;
}

@end
