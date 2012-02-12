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
#import "DGControl.h"
#import "DGLog.h"
#import "DGSystem.h"
#import "DGViewDelegate.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// These are private in order to keep a clean and portable
// header

NSWindow* window;
DGViewDelegate* view;

dispatch_source_t mainTimer;

dispatch_source_t CreateDispatchTimer(uint64_t interval,
                                      uint64_t leeway,
                                      dispatch_queue_t queue,
                                      dispatch_block_t block);

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

// TODO: At this point the system module should copy the config file
// into the user folder
DGSystem::DGSystem() {  
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();
    
    _isInitialized = false;
    _isRunning = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGSystem::~DGSystem() {
    // The shutdown sequence is performed in the terminate() method
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGSystem::findPaths(int argc, char* argv[]) {
    // Check if launched by Finder (non-standard but safe)
    if (argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        if (config->debugMode) {
            // Set working directory to parent of bundle
            NSString *bundleDirectory = [[NSBundle mainBundle] bundlePath];
            NSString *parentDirectory = [bundleDirectory stringByDeletingLastPathComponent];
            chdir([parentDirectory UTF8String]);
        }
        else {
            // Get Application Support folder
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
            NSString *appSupportDirectory = [paths objectAtIndex:0];
            
            appSupportDirectory = [appSupportDirectory stringByAppendingString:@"/Dagon/"];
            
            // Create if it doesn't exist
            if (![[NSFileManager defaultManager] fileExistsAtPath:appSupportDirectory]) {
                [[NSFileManager defaultManager] createDirectoryAtPath:appSupportDirectory withIntermediateDirectories:YES attributes:nil error:nil];
            }
            
            config->setPath(DGPathUser, [appSupportDirectory UTF8String]);
            
            NSString *appDirectory = [[NSBundle mainBundle] resourcePath];
            appDirectory = [appDirectory stringByAppendingString:@"/"];
            config->setPath(DGPathApp, [appDirectory UTF8String]);
            
            // Get resource folder in bundle path
            NSString *resDirectory = [[NSBundle mainBundle] resourcePath];
            resDirectory = [[resDirectory stringByAppendingString:@"/"] stringByAppendingString:@DGDefCatalogPath];
            config->setPath(DGPathRes, [resDirectory UTF8String]);
        }   
        
        [pool release];
    }
}

void DGSystem::init() {
    if (!_isInitialized) {
        log->trace(DGModSystem, "========================================");
        log->trace(DGModSystem, "%s", DGMsg040000);
        
        // We manually create our NSApplication instance
        [NSApplication sharedApplication];
        
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        // Programmatically create a window
        window = [[NSWindow alloc] initWithContentRect:NSMakeRect(50, 100, config->displayWidth, config->displayHeight) 
                                                     styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
                                                       backing:NSBackingStoreBuffered defer:TRUE];
        
        // Programmatically create our NSView delegate
        NSRect mainDisplayRect, viewRect;
        mainDisplayRect = NSMakeRect(0, 0, config->displayWidth, config->displayHeight);
        
        viewRect = NSMakeRect(0.0, 0.0, mainDisplayRect.size.width, mainDisplayRect.size.height);
        view = [[DGViewDelegate alloc] initWithFrame:viewRect];
        
        // Now we're ready to init the controller instance
        control = &DGControl::getInstance();
        control->init();
        
        [window setAcceptsMouseMovedEvents:YES];
        [window setContentView:view];
        [window makeFirstResponder:view];
        [window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];
        [window setTitle:[NSString stringWithUTF8String:config->script()]];
        [window makeKeyAndOrderFront:window];
        
        if (config->fullScreen) {
            // Just one update before going fullscreen
            [view update];
            [window toggleFullScreen:nil];
        }
        
        [NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
        [pool release];
        
        _isInitialized = true;
        log->trace(DGModSystem, "%s", DGMsg040001);
    }
    else log->warning(DGModSystem, "%s", DGMsg140002);
}

void DGSystem::run() {
    mainTimer = CreateDispatchTimer((1.0f / config->framerate) * NSEC_PER_SEC,
                                    0,
                                    dispatch_get_main_queue(),
                                    ^{ update(); });
    
    _isRunning = true;
    [NSApp run];
}

void DGSystem::setTitle(const char* title) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSString* aux = [NSString stringWithUTF8String:title];
    
    [window setTitle:aux];

    [pool release];
}

void DGSystem::terminate() {
    static bool isTerminating = false;
    
    if (!isTerminating) {
        // This is crappy, yes, but the only way to peacefully
        // coexist with our delegate in some situations
        isTerminating = true;
        int r = arc4random() % 8; // Double the replies, so that the default one appears often
        
        if (_isRunning)
            dispatch_source_cancel(mainTimer);
        
        if (_isInitialized) {
            [view release];
            [window release];
        }
        
        switch (r) {
            default:
            case 0: log->trace(DGModSystem, "%s", DGMsg040100); break;
            case 1: log->trace(DGModSystem, "%s", DGMsg040101); break;
            case 2: log->trace(DGModSystem, "%s", DGMsg040102); break;
            case 3: log->trace(DGModSystem, "%s", DGMsg040103); break;
        }
        
        [NSApp terminate:nil];
    }
}

void DGSystem::toggleFullScreen() {
    config->fullScreen = !config->fullScreen;
    if (_isRunning) {
        // Suspend the timer to avoid multiple redraws
        dispatch_suspend(mainTimer);
        [window toggleFullScreen:nil];
        dispatch_resume(mainTimer);
    }
    else [window toggleFullScreen:nil];
}

void DGSystem::update() {
    [view update];
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

dispatch_source_t CreateDispatchTimer(uint64_t interval,
                                      uint64_t leeway,
                                      dispatch_queue_t queue,
                                      dispatch_block_t block) {
    dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,
                                                     0, 0, queue);
    if (timer) {
        dispatch_source_set_timer(timer, dispatch_time(NULL, 0), interval, leeway);
        dispatch_source_set_event_handler(timer, block);
        dispatch_resume(timer);
    }
    
    return timer;
}
