////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#include "Control.h"
#include "Version.h"

bool shouldStartApp(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  if (!shouldStartApp(argc, argv)) {
    return 0;
  }

  dagon::Control& control = dagon::Control::instance();
  
  control.init(argc, argv);
  control.run();
  
  return 0;
}

void displayHelp(char* progName)
{
    printf("\n"
           "DAGON - An Adventure Game Engine\n\n"
           "To start a game, run Dagon in a directory containing a config.lua file.\n\n"
           "Usage:\n"
           "  %s\n"
           "  %s [-h|--help]\n"
           "  %s [-v|--version]\n",
           progName,
           progName,
           progName);
}

void displayVersion()
{
    printf(DAGON_VERSION_STRING "\n");
}

inline
bool shouldStartApp(int argc, char* argv[])
{
#define MATCHES_OPT(str, short, long) \
    strcmp(str, short)==0 || strcmp(str, long)==0
    
    if (argc > 1) {
        const char* const firstArg = argv[1];
        if (MATCHES_OPT(firstArg, "-h", "--help")) {
          displayHelp(argv[0]);
          return false;
        }
        
        if (MATCHES_OPT(firstArg, "-v", "--version")) {
          displayVersion();
          return false;
        }
        
    }
    return true;
    
#undef MATCHES_OPT
}
