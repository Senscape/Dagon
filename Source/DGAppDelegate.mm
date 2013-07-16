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

#import "DGAppDelegate.h"
#import "DGScript.h"

#import <crt_externs.h>

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

@implementation DGAppDelegate

- (void)dealloc {
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    DGScript* script;
    CGDisplayHideCursor(kCGDirectMainDisplay);
    
    script = &DGScript::getInstance();
    script->init(*_NSGetArgc(), *_NSGetArgv());
    script->run();
}

@end
