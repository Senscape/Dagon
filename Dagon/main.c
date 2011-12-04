/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGConfig.h"
#include "DGScript.h"
#include "DGSystem.h"

int dg_main(int argc, char *argv[]) {
	// TODO: Init the script directly, then system inits the controller
	// TODO: Configuration should be handled via script as well
	// TODO: Test if dagon.lua exists, if not load argv[1], if not launch the embedded interpreter

    // Should allow automatically loading the default script dagon
    // Debug mode doesn't work as planned this way
    dg_config_init();
    
    // On Mac OS X, check if launched by Finder (non-standard)
    if ( argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) {
        dg_system_find_paths();
    }
    
	dg_script_init();
	dg_script_run();
	
	return 0;
}
