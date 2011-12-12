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

#import <dispatch/dispatch.h>


@implementation DGAppDelegate

dispatch_queue_t audio_queue;
dispatch_queue_t video_queue;

- (void)audioLoop {
    /*dispatch_queue_t queue = dispatch_get_global_queue(0,0);
    
    while (1) {
        dispatch_sync(queue,^{
            DGAudioLoop();
        });
        [NSThread sleepForTimeInterval:0.01f];
    }*/
    
    audio_queue = dispatch_get_global_queue(0,0);
    

}

- (void)mainLoop {
    dispatch_async(audio_queue,^{
        DGAudioLoop();
    });
    
    dispatch_async(video_queue,^{
        DGVideoLoop();
    });
    
    DGScriptLoop();
}

- (void)videoLoop {
    /*dispatch_queue_t queue = dispatch_get_global_queue(0,0);
    while (1) {
        dispatch_sync(queue,^{
            DGVideoLoop();
        });
        [NSThread sleepForTimeInterval:0.00001f];
    }*/
    
    video_queue = dispatch_get_global_queue(0,0);
}

@end
