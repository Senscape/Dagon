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

#import "DGAudioManager.h"
#import "DGConfig.h"
#import "DGControl.h"
#import "DGLog.h"
#import "DGSystem.h"
#import "DGTimerManager.h"
#import "DGViewDelegate.h"
#import "DGVideoManager.h"
#import "DGWindowDelegate.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

// These are static and private in order to keep a clean and
// portable header.

bool lionFullscreen = false;

NSWindow* window;
DGViewDelegate* view;
DGWindowDelegate* windowDelegate;

dispatch_semaphore_t _semaphores[DGNumberOfThreads];

dispatch_source_t _mainLoop;
dispatch_source_t _audioThread;
dispatch_source_t _timerThread;
dispatch_source_t _profilerThread;
dispatch_source_t _videoThread;
dispatch_source_t CreateDispatchTimer(uint64_t interval,
                                      uint64_t leeway,
                                      dispatch_queue_t queue,
                                      dispatch_block_t block);

#define DGFullscreenFadeOutDuration	0.2f
#define DGFullscreenFadeInDuration	0.4f

NSString * const DGSessionWillEnterFullScreenNotification	= @"DGSessionWillEnterFullScreen";
NSString * const DGSessionDidEnterFullScreenNotification	= @"DGSessionDidEnterFullScreen";
NSString * const DGSessionWillExitFullScreenNotification	= @"DGSessionWillExitFullScreen";
NSString * const DGSessionDidExitFullScreenNotification		= @"DGSessionDidExitFullScreen";

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
    
    _areThreadsActive = false;
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

void DGSystem::browse(const char* url) {
    if (config->fullScreen && !lionFullscreen) {
        config->fullScreen = false;
        this->toggleFullScreen();
    }
    
    NSString* aux = [NSString stringWithUTF8String:url];
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:aux]];
}

void DGSystem::createThreads() {
    // Create the semaphores
    _semaphores[DGAudioThread] = dispatch_semaphore_create(0);
    _semaphores[DGTimerThread] = dispatch_semaphore_create(0);
    _semaphores[DGVideoThread] = dispatch_semaphore_create(0);
    
    // Send the first signal
    dispatch_semaphore_signal(_semaphores[DGAudioThread]);
    dispatch_semaphore_signal(_semaphores[DGTimerThread]); 
    dispatch_semaphore_signal(_semaphores[DGVideoThread]);
    
    _audioThread = CreateDispatchTimer(0.01f * NSEC_PER_SEC, 0,
                                       dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0),
                                       ^{ dispatch_semaphore_wait(_semaphores[DGAudioThread], DISPATCH_TIME_FOREVER);
                                           audioManager->update();
                                           dispatch_semaphore_signal(_semaphores[DGAudioThread]); });    
    
    _timerThread = CreateDispatchTimer(0.001f * NSEC_PER_SEC, 0,
                                       dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0),
                                       ^{ dispatch_semaphore_wait(_semaphores[DGTimerThread], DISPATCH_TIME_FOREVER);
                                           timerManager->update(); 
                                           dispatch_semaphore_signal(_semaphores[DGTimerThread]); });
    
    _videoThread = CreateDispatchTimer(0.001f * NSEC_PER_SEC, 0,
                                       dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0),
                                       ^{ dispatch_semaphore_wait(_semaphores[DGVideoThread], DISPATCH_TIME_FOREVER);
                                           videoManager->update();
                                           dispatch_semaphore_signal(_semaphores[DGVideoThread]); });
    
    if (config->debugMode) {
        _profilerThread = CreateDispatchTimer(1.0f * NSEC_PER_SEC, 0,
                                              dispatch_get_main_queue(),
                                              ^{ control->profiler(); });
    }
    
    _areThreadsActive = true;
}

void DGSystem::destroyThreads() {    
    // Suspend and release all threads
    this->suspendThread(DGAudioThread);
    this->suspendThread(DGTimerThread);
    this->suspendThread(DGVideoThread);
    
    dispatch_source_cancel(_audioThread);
    dispatch_source_cancel(_timerThread);
    dispatch_source_cancel(_videoThread);
    
    dispatch_release(_semaphores[DGAudioThread]);
    dispatch_release(_semaphores[DGTimerThread]);
    dispatch_release(_semaphores[DGVideoThread]);
    
    // If in debug mode, release the profiler
    if (config->debugMode) {
        dispatch_source_cancel(_profilerThread);
        dispatch_release(_profilerThread);
    }
    
    _areThreadsActive = false;
}

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
            resDirectory = [[resDirectory stringByAppendingString:@"/"] stringByAppendingString:@DGDefResourcePath];
            config->setPath(DGPathRes, [resDirectory UTF8String]);
            
            chdir([appDirectory UTF8String]);
        }   
        
        [pool release];
    }
}

void DGSystem::init() {
    if (!_isInitialized) {
        log->trace(DGModSystem, "%s", DGMsg040000);
        
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        NSScreen *mainScreen = [NSScreen mainScreen];
        NSRect screenRect = [mainScreen visibleFrame];
        NSPoint origin;
        
        if (config->displayWidth > screenRect.size.width) {
            config->displayWidth = screenRect.size.width;
            origin.x = 0;
        }
        else {
            origin.x = (screenRect.size.width - config->displayWidth) / 2;
        }
        
        if (config->displayHeight > screenRect.size.height) {
            config->displayHeight = screenRect.size.height;
            origin.y = 0;
        }
        else {
            origin.y = (screenRect.size.height - config->displayHeight) / 2;
        }
        
        // Programmatically create a window
        window = [[NSWindow alloc]
                  initWithContentRect: NSMakeRect(origin.x, origin.y, config->displayWidth, config->displayHeight)
                  styleMask: NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
                  backing: NSBackingStoreBuffered
                  defer: NO
                  screen: mainScreen];
        
        // Programmatically create our NSView delegate
        NSRect mainDisplayRect, viewRect;
        mainDisplayRect = NSMakeRect(0, 0, config->displayWidth, config->displayHeight);
        
        viewRect = NSMakeRect(0.0, 0.0, mainDisplayRect.size.width, mainDisplayRect.size.height);
        view = [[DGViewDelegate alloc] initWithFrame:viewRect];
        
        // Now we're ready to init the controller instance
        control = &DGControl::getInstance();
        control->init();
       
        [window setAcceptsMouseMovedEvents:YES];
        [window makeFirstResponder:view];
        [window setTitle:[NSString stringWithUTF8String:config->script()]];
        [window setContentView:view];
        
        windowDelegate = [[DGWindowDelegate alloc] init];
        [windowDelegate setWindow:window];
        [window setDelegate:windowDelegate];
        
        SInt32 OSXversionMajor, OSXversionMinor;
        if (Gestalt(gestaltSystemVersionMajor, &OSXversionMajor) == noErr &&
            Gestalt(gestaltSystemVersionMinor, &OSXversionMinor) == noErr) {
            if(OSXversionMajor == 10 && OSXversionMinor >= 7) {
                lionFullscreen = true;
                [window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];
            }
        }
        
        if (config->fullScreen) {
            if (!lionFullscreen)
                [window makeKeyAndOrderFront:window];
                
            this->toggleFullScreen();
        }
        else [window makeKeyAndOrderFront:window];

        [pool release];
        
        _isInitialized = true;
        log->trace(DGModSystem, "%s", DGMsg040001);
    }
    else log->warning(DGModSystem, "%s", DGMsg140002);
}

void DGSystem::resumeThread(int threadID) {
    if (_areThreadsActive) {
        switch (threadID) {
            case DGAudioThread:
                dispatch_resume(_audioThread);
                break;
            case DGTimerThread:
                dispatch_resume(_timerThread);
                break;
            case DGVideoThread:
                dispatch_resume(_videoThread);
                break;                
        }
    }
}

// TODO: Note this isn't quite multithreaded yet.
void DGSystem::run() {
    // The timer is running in the main process and thus the OpenGL context doesn't
    // have to be shared.
    _mainLoop = CreateDispatchTimer((1.0f / config->framerate) * NSEC_PER_SEC, 0,
                                    dispatch_get_main_queue(),
                                    ^{ control->update(); });
    
    _isRunning = true;
}

void DGSystem::setTitle(const char* title) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSString* aux = [NSString stringWithUTF8String:title];
    
    [window setTitle:aux];

    [pool release];
}

void DGSystem::suspendThread(int threadID) {
    if (_areThreadsActive) {
        dispatch_semaphore_wait(_semaphores[threadID], DISPATCH_TIME_FOREVER);
        
        switch (threadID) {
            case DGAudioThread:
                dispatch_suspend(_audioThread);
                break;
            case DGTimerThread:
                dispatch_suspend(_timerThread);
                break;
            case DGVideoThread:
                dispatch_suspend(_videoThread);
                break;                
        }
        
        dispatch_semaphore_signal(_semaphores[threadID]);
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
            // Release the main loop
            dispatch_source_cancel(_mainLoop);
            
            if (_areThreadsActive)
                destroyThreads();
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
        
        [NSApp terminate: nil];
    }
}

// 10.6 support adapted from Boxer app by Alun Bestor: http://boxerapp.com
void DGSystem::toggleFullScreen() {
    // TODO: Suspend the timer to avoid multiple redraws
    if (lionFullscreen) {
        [window toggleFullScreen: nil];
    }
    else {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        NSDictionary* FullScreen_Options; // Not in the pool
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        NSString *startNotification, *endNotification;
        
        if (config->fullScreen) {
            startNotification	= DGSessionWillEnterFullScreenNotification;
            endNotification		= DGSessionDidEnterFullScreenNotification;
        }
        else {
            startNotification	= DGSessionWillExitFullScreenNotification;
            endNotification		= DGSessionDidExitFullScreenNotification;
        }
        
        [center postNotificationName: startNotification object: window];
        
        //Set up a screen fade in and out of the fullscreen mode
        CGError acquiredToken;
        CGDisplayFadeReservationToken fadeToken;
        
        acquiredToken = CGAcquireDisplayFadeReservation(DGFullscreenFadeOutDuration + DGFullscreenFadeInDuration, &fadeToken);
        
        //First fade out to black synchronously
        if (acquiredToken == kCGErrorSuccess) {
            CGDisplayFade(fadeToken,
                          DGFullscreenFadeOutDuration,	//Fade duration
                          (CGDisplayBlendFraction)kCGDisplayBlendNormal,		//Start transparent
                          (CGDisplayBlendFraction)kCGDisplayBlendSolidColor,	//Fade to opaque
                          0.0f, 0.0f, 0.0f,				//Pure black (R, G, B)
                          true							//Synchronous
                          );
        }
        
        FullScreen_Options = [NSDictionary dictionaryWithObject: [NSNumber numberWithBool: YES] forKey: NSFullScreenModeAllScreens];
        
        if (config->fullScreen) {
            [view enterFullScreenMode: [NSScreen mainScreen] withOptions: FullScreen_Options];
            //[NSCursor hide];
        }
        else {
            [view exitFullScreenModeWithOptions: FullScreen_Options];
            [window makeFirstResponder:view];
            //[NSCursor unhide];
        }
        
        //And now fade back in from black asynchronously
        if (acquiredToken == kCGErrorSuccess) {
            CGDisplayFade(fadeToken,
                          DGFullscreenFadeInDuration,	//Fade duration
                          (CGDisplayBlendFraction)kCGDisplayBlendSolidColor,	//Start opaque
                          (CGDisplayBlendFraction)kCGDisplayBlendNormal,		//Fade to transparent
                          0.0f, 0.0f, 0.0f,				//Pure black (R, G, B)
                          false							//Asynchronous
                          );
        }
        CGReleaseDisplayFadeReservation(fadeToken);
        
        [center postNotificationName: endNotification object: window];
        
        [pool release];
    }
}

void DGSystem::update() {
    [view update];
}

time_t DGSystem::wallTime() {
    dispatch_time_t now = dispatch_time(DISPATCH_TIME_NOW, NULL);
    return (now / 1000);
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
