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

#include "DGScript.h"

int main(int argc, char *argv[]) {
    DGScript* script;
    
    script = &DGScript::getInstance();
    
    script->init(argc, argv);
    script->run();
    
	return 0;
}
