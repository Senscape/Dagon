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
#import "DGLog.h"
#import "DGRender.h"
#import "DGSystem.h"
#import "DGAppDelegate.h"
#import "DGWindowDelegate.h"

#ifndef	DG_USE_SDL


DGAppDelegate* appDelegate;
DGWindowDelegate* window;

NSLock *theLock;
NSTimer* timer;

#define BXFullscreenFadeOutDuration	0.2f
#define BXFullscreenFadeInDuration	0.4f

NSString * const BXSessionWillEnterFullScreenNotification	= @"BXSessionWillEnterFullScreen";
NSString * const BXSessionDidEnterFullScreenNotification	= @"BXSessionDidEnterFullScreen";
NSString * const BXSessionWillExitFullScreenNotification	= @"BXSessionWillExitFullScreen";
NSString * const BXSessionDidExitFullScreenNotification		= @"BXSessionDidExitFullScreen";

void DGSystemInitialize() {    
	DGLogTrace(DG_MOD_SYSTEM, "Initializing system interface...");
	
	[NSApplication sharedApplication];
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	appDelegate = [[DGAppDelegate alloc] init];
	
#ifndef DG_TOOL    
    window = [[DGWindowDelegate alloc] initWithContentRect:NSMakeRect(50, 100, DGConfig.display_width, DGConfig.display_height) 
                                         styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
                                           backing:NSBackingStoreBuffered defer:TRUE];
    
	//[window setLevel:NSMainMenuWindowLevel+1];
	[window setOpaque:YES];
	[window setHidesOnDeactivate:YES];
	[window setAcceptsMouseMovedEvents:YES];
    [window makeKeyAndOrderFront:window];
    
	[window setContentView:[window view]];
	[window makeFirstResponder:[window view]];

    DGRenderClearScene();
    [[window view] update];
    
    [window setTitle:[NSString stringWithUTF8String:DGConfig.script]];
    
    // Hide cursor
    
    if (DGConfig.fullscreen)
        DGSystemToggleFullscreen();
    
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
#endif
	
	[pool release];
}

void DGSystemFindPaths() {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
	if (DGConfig.debug_mode) {
		// Set working directory to parent of bundle
		NSString *bundleDirectory = [[NSBundle mainBundle] bundlePath];
		NSString *parentDirectory = [bundleDirectory stringByDeletingLastPathComponent];
		//strcpy(config.res_path, [[parentDirectory stringByAppendingString:@"/"] UTF8String]);
		//strcpy(config.user_path, [[parentDirectory stringByAppendingString:@"/resources/"] UTF8String]);
		// Must recreate the path!!
        // Not needed if running from Xcode (because of the working directory)
        chdir([parentDirectory UTF8String]);
        //strcpy(config.app_path, "");
        //strcpy(config.res_path, "");
        //strcpy(config.user_path, "");
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
		
		strcpy(DGConfig.user_path, [appSupportDirectory UTF8String]);
		
		// Get resource folder in bundle path
		NSString *resDirectory = [[NSBundle mainBundle] resourcePath];
        resDirectory = [resDirectory stringByAppendingString:@"/"];
        strcpy(DGConfig.app_path, [resDirectory UTF8String]); // Maybe this isn't necessary after all...
		strcpy(DGConfig.res_path, [resDirectory UTF8String]);
	}   
    
    [pool release];
}

void DGSystemLock() {
    [theLock lock];
}

void DGSystemRun() {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    theLock = [[NSLock alloc] init];
	
	[NSThread detachNewThreadSelector:@selector(audioLoop)
							 toTarget:appDelegate withObject:nil];
	
	[NSThread detachNewThreadSelector:@selector(videoLoop)
							 toTarget:appDelegate withObject:nil];
	
	if (DGConfig.multithread) {
		[NSThread detachNewThreadSelector:@selector(mainLoop)
								 toTarget:appDelegate withObject:nil];
	}
	else {
		timer = [NSTimer scheduledTimerWithTimeInterval:1.0f/DGConfig.framerate // WARNING: This value should be closer to 60fps
												 target:appDelegate selector:@selector(mainLoop) userInfo:nil repeats:DG_YES];
	}

	[pool release];	
	
	[NSApp run];
}

void DGSystemTerminate() {
#ifndef DG_TOOL
	if (!DGConfig.multithread) {
		[timer invalidate];
	}
	
    // TODO: Ugly hack, this should be removed, @synchronize?
	//[NSThread sleepForTimeInterval:1.0f/config.framerate];

    [window release];
#endif
    
    [theLock release];

	[appDelegate release];
	
	[NSApp terminate: nil];
}

void DGSystemSetTitle(const char* theTitle) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSString* aux = [NSString string];
    
    aux = [aux stringByAppendingString:[NSString stringWithUTF8String:DGConfig.script]];
    aux = [aux stringByAppendingString:@" ("];
    aux = [aux stringByAppendingString:[NSString stringWithUTF8String:theTitle]];
    aux = [aux stringByAppendingString:@")"];
    
    [window setTitle:aux];
    
    [pool release];
}

void DGSystemToggleFullscreen() {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSDictionary* FullScreen_Options; // Not in the pool
    NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
    NSString *startNotification, *endNotification;
    
    if (DGConfig.fullscreen) 
    {
        startNotification	= BXSessionWillEnterFullScreenNotification;
        endNotification		= BXSessionDidEnterFullScreenNotification;
    }
    else
    {
        startNotification	= BXSessionWillExitFullScreenNotification;
        endNotification		= BXSessionDidExitFullScreenNotification;
    }
    
    [center postNotificationName: startNotification object: window];
    
    //Set up a screen fade in and out of the fullscreen mode
    CGError acquiredToken;
    CGDisplayFadeReservationToken fadeToken;
    
    acquiredToken = CGAcquireDisplayFadeReservation(BXFullscreenFadeOutDuration + BXFullscreenFadeInDuration, &fadeToken);
    
    //First fade out to black synchronously
    if (acquiredToken == kCGErrorSuccess)
    {
        CGDisplayFade(fadeToken,
                      BXFullscreenFadeOutDuration,	//Fade duration
                      (CGDisplayBlendFraction)kCGDisplayBlendNormal,		//Start transparent
                      (CGDisplayBlendFraction)kCGDisplayBlendSolidColor,	//Fade to opaque
                      0.0f, 0.0f, 0.0f,				//Pure black (R, G, B)
                      true							//Synchronous
                      );
    }
    
    FullScreen_Options = [NSDictionary dictionaryWithObject: [NSNumber numberWithBool: YES] forKey: NSFullScreenModeAllScreens];
    
    if (DGConfig.fullscreen) {
        [[window view] enterFullScreenMode: [NSScreen mainScreen] withOptions: FullScreen_Options];
    }
    else {
        [[window view] exitFullScreenModeWithOptions: FullScreen_Options];
        [window makeFirstResponder:[window view]];
    }
        
    DGRenderClearScene();
    
    [[window view] update];
    
    //And now fade back in from black asynchronously
    if (acquiredToken == kCGErrorSuccess)
    {
        CGDisplayFade(fadeToken,
                      BXFullscreenFadeInDuration,	//Fade duration
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

void DGSystemUnlock() {  
    [theLock unlock];
}

void DGSystemUpdate() {
	[[window view] update];
	
	if (DGConfig.multithread) 
		[NSThread sleepForTimeInterval:1.0f/DGConfig.framerate];
}

#endif
