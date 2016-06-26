////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2016 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_DESERIALIZER_H_
#define DAGON_DESERIALIZER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <lua.hpp>
#include <SDL2/SDL_rwops.h>

#include <string>

namespace dagon {

class Deserializer {
public:
  static std::string readPreview(const std::string &path);
};

}

#endif // DAGON_DESERIALIZER_H_
