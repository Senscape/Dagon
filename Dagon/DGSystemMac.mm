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

#import "DGAudioManager.h"
#import "DGConfig.h"
#import "DGControl.h"
#import "DGLog.h"
#import "DGSystem.h"
#import "DGTimerManager.h"
#import "DGViewDelegate.h"
#import "DGVideoManager.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// These are static private in order to keep a clean and
// portable header

NSWindow* window;
DGViewDelegate* view;

dispatch_semaphore_t _semaphores[DGMaxSystemSemaphores];

dispatch_source_t _audioTimer;
dispatch_source_t _mainLoop;
dispatch_source_t _mainTimer;
dispatch_source_t _profilerTimer;
dispatch_source_t _videoTimer;
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
    audioManager = &DGAudioManager::getInstance();
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();
    timerManager = &DGTimerManager::getInstance();  
    videoManager = &DGVideoManager::getInstance();    
    
    _semaphoresIndex = 0;
    
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

bool DGSystem::getSemaphore(int* pointerToID) {
    if (_semaphoresIndex + 1 > DGMaxSystemSemaphores)
        return false;
    
    _semaphores[_semaphoresIndex] = dispatch_semaphore_create(0);
    *pointerToID = _semaphoresIndex;
    
    _semaphoresIndex++;
    
    return true;
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
            [window toggleFullScreen:nil];
            [NSCursor hide];
        }
        
        [NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
        [pool release];
        
        _isInitialized = true;
        log->trace(DGModSystem, "%s", DGMsg040001);
    }
    else log->warning(DGModSystem, "%s", DGMsg140002);
}

void DGSystem::releaseSemaphore(int ID) {
    dispatch_semaphore_signal(_semaphores[ID]);
}

void DGSystem::resumeManager() {
    if (_isRunning) {
        dispatch_resume(_videoTimer);
    }
}

// TODO: Note this isn't quite multithreaded yet.
// The timer is running in the main process and thus the OpenGL context doesn't
// have to be shared.
void DGSystem::run() {
    _semaphores[0] = dispatch_semaphore_create(0);
      dispatch_semaphore_signal(_semaphores[0]);
    
    _mainLoop = CreateDispatchTimer((1.0f / config->framerate) * NSEC_PER_SEC, 0,
                                     dispatch_get_main_queue(),
                                     ^{ control->update(); });
    
    _mainTimer = CreateDispatchTimer(0.01f * NSEC_PER_SEC, 0,
                                     dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0),
                                     ^{ timerManager->update(); });

    _audioTimer = CreateDispatchTimer(0.01f * NSEC_PER_SEC, 0,
                                     dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0),
                                     ^{ audioManager->update(); });
    
    _videoTimer = CreateDispatchTimer(0.01f * NSEC_PER_SEC, 0,
                                      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0),
                                      ^{  dispatch_semaphore_wait(_semaphores[0], DISPATCH_TIME_FOREVER);
                                          videoManager->update();
                                          dispatch_semaphore_signal(_semaphores[0]); });
    
    if (config->debugMode) {
        _profilerTimer = CreateDispatchTimer(1.0f * NSEC_PER_SEC, 0,
                                             dispatch_get_main_queue(),
                                             ^{ control->profiler(); });
    }
    
    _isRunning = true;
    
    [NSApp run];
}

void DGSystem::setTitle(const char* title) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSString* aux = [NSString stringWithUTF8String:title];
    
    [window setTitle:aux];

    [pool release];
}

void DGSystem::suspendManager() {
    if (_isRunning) {
        dispatch_semaphore_wait(_semaphores[0], DISPATCH_TIME_FOREVER);
        dispatch_suspend(_videoTimer);
        dispatch_semaphore_signal(_semaphores[0]);
    }
}

void DGSystem::terminate() {
    static bool isTerminating = false;
    
    if (!isTerminating) {
        // This is crappy, yes, but the only way to peacefully
        // coexist with our delegate in some situations
        isTerminating = true;
        int r = arc4random() % 8; // Double the replies, so that the default one appears often
        
        if (_isRunning) {
            dispatch_release(_mainLoop);
            dispatch_release(_mainTimer);
            dispatch_release(_audioTimer);
            dispatch_release(_videoTimer);            
            
            if (config->debugMode)
                dispatch_release(_profilerTimer);
        }    
        
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
        // TODO: Suspend the timer to avoid multiple redraws
        [window toggleFullScreen:nil];
    }
    else [window toggleFullScreen:nil];
}

void DGSystem::update() {
    [view update];
}

time_t DGSystem::wallTime() {
    dispatch_time_t now = dispatch_time(DISPATCH_TIME_NOW, NULL);
    return (now / 1000);
}

void DGSystem::waitForSemaphore(int ID) {
    dispatch_semaphore_wait(_semaphores[ID], DISPATCH_TIME_FOREVER);
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
