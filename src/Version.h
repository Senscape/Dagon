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

#ifndef DAGON_VERSION_H_
#define DAGON_VERSION_H_

#define DAGON_VERSION_MAJOR 0
#define DAGON_VERSION_MINOR 6
#define DAGON_VERSION_RELEASE 5
#define DAGON_VERSION_TYPE "a"
#define DAGON_BUILD 1007

#define mkstr(a) # a
#define mkver(a, b, c) mkstr(a) "." mkstr(b) "." mkstr(c)

#define DAGON_VERSION_STRING mkver(DAGON_VERSION_MAJOR, DAGON_VERSION_MINOR, DAGON_VERSION_RELEASE) DAGON_VERSION_TYPE

#endif // DAGON_VERSION_H_
