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

#include "Node.h"

#include <lua.hpp>
#include <SDL2/SDL_rwops.h>

#include <string>

namespace dagon {

typedef struct {
  SDL_RWops *rw;
  uint16_t numBytes;
  const char *buf;
  bool readError;
}LuaReaderData;

class Deserializer {
  lua_State *_L;
  SDL_RWops *_rw;

  std::string _dgVersion;
  std::string _preview;
  std::string _roomName;

  // Lua callback for reading a function
  static const char *readFunction(lua_State *L, void *data, size_t *size);

public:
  Deserializer(lua_State *L, SDL_RWops *rw);
  ~Deserializer();

  // Gets
  std::string version();
  std::string preview();
  std::string roomName();

  // State changes
  bool readHeader();
  bool readScriptData();
  void toggleSpots();
  Node *readNode();
  void adjustCamera();
  void toggleAudio();
  bool readTimers();
  void readControlMode();
  static std::string readPreview(const std::string &path);
};

}

#endif // DAGON_DESERIALIZER_H_
