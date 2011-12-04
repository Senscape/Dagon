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


@implementation DagonAppDelegate

- (void)audioLoop {
	while (dg_audio_loop()) {
		[NSThread sleepForTimeInterval:0.01f];
	}
}

- (void)mainLoop {
	dg_script_loop();
}

- (void)videoLoop {
	while (dg_video_loop()) {
		//[NSThread sleepForTimeInterval:0.0000001f];
        [NSThread sleepForTimeInterval:0.000001f];
	}
}

@end
