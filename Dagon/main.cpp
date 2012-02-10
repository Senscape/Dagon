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

#include "DGSystem.h"

int main(int argc, char *argv[]) {
    DGSystem* system;
    
    system = &DGSystem::getInstance();
    
    system->init(argc, (const char **) argv);
    system->run();
    
	return 0;
}
