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

#ifndef DAGON_SERIALIZER_H_
#define DAGON_SERIALIZER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <lua.hpp>
#include <SDL2/SDL_rwops.h>

#include <string>
#include <unordered_map>

#define kTablePrefix "DG_TABLE_"

namespace dagon {

class Serializer {
  lua_State *_L;
  SDL_RWops *_rw;

  std::unordered_map<std::string, std::string> _tblMap;

  bool writeGlobalField(const std::string &key, const std::string &val);
  // This assumes the table is at stack position -1
  std::string stringifyTable(const std::string parentKey);
  // Callback for Lua when dumping a function
  static int writeFunction(lua_State *L, const void *p, size_t sz, void *ud);

public:
  Serializer(lua_State *L, SDL_RWops *rw);
  ~Serializer();

  bool writeHeader();
  bool writeGlobals();
  bool writeRoomData();
};

}

#endif // DAGON_SERIALIZER_H_
