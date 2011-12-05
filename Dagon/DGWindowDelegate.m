//
//  DagonWindowDelegate.m
//  Dagon
//
//  Created by Agustín Cordes on 5/5/11.
//  Copyright 2011 Senscape s.r.l. All rights reserved.
//

#import "DGConfig.h"
#import "DGWindowDelegate.h"


@implementation DGWindowDelegate

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle 
                  backing:(NSBackingStoreType)bufferingType defer:(BOOL)defer {
    
    NSRect mainDisplayRect, viewRect;
	mainDisplayRect = NSMakeRect(0, 0, DGConfig.display_width, DGConfig.display_height);
    
    self = [super initWithContentRect:contentRect styleMask:windowStyle backing:bufferingType defer:defer];
    if (self) {
        viewRect = NSMakeRect(0.0, 0.0, mainDisplayRect.size.width, mainDisplayRect.size.height);
        viewDelegate = [[DGViewDelegate alloc] initWithFrame:viewRect];
    }
    
    return self;
}

- (BOOL)canBecomeKeyWindow {
    return YES;
}

- (void)dealloc {
	[viewDelegate release];
	[super dealloc];
}

- (id)view {
    return viewDelegate;
}

@end
