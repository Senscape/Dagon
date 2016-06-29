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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Deserializer.h"
#include "Log.h"

#include <climits>

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Public
////////////////////////////////////////////////////////////
std::string Deserializer::readPreview(const std::string &path) {
  SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "rb");
  if (!rw) {
    Log::instance().error(kModScript, "%s", SDL_GetError());
    return "";
  }

  uint8_t len;
  if (!(len = SDL_ReadU8(rw))) {
    Log::instance().error(kModScript, "%s", SDL_GetError());
    return "";
  }

  char buf[UCHAR_MAX + 1]; // + 1 for \0
  if (!SDL_RWread(rw, buf, len, 1)) {
    Log::instance().error(kModScript, "%s", SDL_GetError());
    return "";
  }

  if (!(len = SDL_ReadU8(rw))) {
    Log::instance().error(kModScript, "%s", SDL_GetError());
    return "";
  }

  if (!SDL_RWread(rw, buf, len, 1)) {
    Log::instance().error(kModScript, "%s", SDL_GetError());
    return "";
  }
  SDL_RWclose(rw);

  buf[len] = '\0';
  return buf;
}

}
