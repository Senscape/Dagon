/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#import "DGAudio.h"
#import "DGScript.h"
#import "DGVideo.h"

#import "DGAppDelegate.h"


@implementation DGAppDelegate

- (void)audioLoop {
	while (DGAudioLoop()) {
		[NSThread sleepForTimeInterval:0.01f];
	}
}

- (void)mainLoop {
	DGScriptLoop();
}

- (void)videoLoop {
	while (DGVideoLoop()) {
		//[NSThread sleepForTimeInterval:0.0000001f];
        [NSThread sleepForTimeInterval:0.0000001f];
	}
}

@end
