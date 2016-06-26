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

#include "Serializer.h"
#include "Node.h"
#include "Control.h"
#include "Version.h"

#include <cstring>
#include <climits>

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor & Destructor
////////////////////////////////////////////////////////////

Serializer::Serializer(lua_State *L, SDL_RWops *rw) : _L(L), _rw(rw) {}

Serializer::~Serializer() {
  SDL_RWclose(_rw);
}

////////////////////////////////////////////////////////////
// Implementation - Private
////////////////////////////////////////////////////////////

bool Serializer::writeGlobalField(const std::string &key, const std::string &val) {
  return SDL_WriteBE32(_rw, key.length() + val.length() + 1) && // +1 because of =
         SDL_RWwrite(_rw, key.c_str(), key.length(), 1) &&
         SDL_WriteU8(_rw, '=') &&
         SDL_RWwrite(_rw, val.c_str(), val.length(), 1);
}

std::string Serializer::stringifyTable(const std::string parentKey) {
  // Handle cycles
  for (int i = 0; i < _tblMap.size(); i++) {
    const std::string regKey = kTablePrefix + std::to_string(i);
    lua_pushstring(_L, regKey.c_str());
    lua_gettable(_L, LUA_REGISTRYINDEX);

    if (lua_equal(_L, -1, -2)) { // Maybe a cycle...
      lua_pop(_L, 1);
      return _tblMap[regKey];
    }
    else
      lua_pop(_L, 1);
  }

  { // Store this table
    const std::string regKey = kTablePrefix + std::to_string(_tblMap.size());
    lua_pushstring(_L, regKey.c_str());
    lua_pushvalue(_L, -2);
    lua_settable(_L, LUA_REGISTRYINDEX);
    _tblMap[regKey] = parentKey;
  }

  std::string str;
  str += '{';

  lua_pushnil(_L);
  bool first = true;
  while (lua_next(_L, -2) != 0) {
    if (!lua_isstring(_L, -2)) {
      lua_pop(_L, 1);
      continue;
    }

    std::string val;
    std::string key;
    if (lua_type(_L, -2) == LUA_TSTRING) {
      lua_pushvalue(_L, -2);
      key = lua_tostring(_L, -1);
      key += '=';
      lua_pop(_L, 1);
    }

    switch (lua_type(_L, -1)) {
    case LUA_TTABLE: {
      std::string fullKey = parentKey + '[';
      if (lua_type(_L, -2) == LUA_TSTRING)
        fullKey += '"';
      lua_pushvalue(_L, -2);
      fullKey += lua_tostring(_L, -1);
      lua_pop(_L, 1);
      if (lua_type(_L, -2) == LUA_TSTRING)
        fullKey += '"';
      fullKey += ']';

      val = stringifyTable(fullKey);
      break;
    }
    case LUA_TBOOLEAN: {
      val = lua_toboolean(_L, -1) ? "true" : "false";
      break;
    }
    case LUA_TSTRING:
      val += '"';
      // Fallthrough
    case LUA_TNUMBER: {
      val += lua_tostring(_L, -1);
      if (val[0] == '"')
        val += '"';
      break;
    }
    default: {
      lua_pop(_L, 1);
      continue;
    }
    }

    if (!first)
      str += ',';
    first = false;
    str += key;
    str += val;

    lua_pop(_L, 1);
  }

  str += '}';
  return str;
}

////////////////////////////////////////////////////////////
// Implementation - Public
////////////////////////////////////////////////////////////

bool Serializer::writeHeader() {
  // Write version information
  {
    const size_t len = UCHAR_MAX > strlen(DAGON_VERSION_STRING) ? strlen(DAGON_VERSION_STRING) :
                                                                  UCHAR_MAX;
    if (!SDL_WriteU8(_rw, len))
      return false;
    if (!SDL_RWwrite(_rw, DAGON_VERSION_STRING, len, 1))
      return false;
  }

  // Write preview
  {
    const std::string desc = Control::instance().currentNode()->description();
    const size_t len = UCHAR_MAX > desc.length() ? desc.length() : UCHAR_MAX;

    if (!SDL_WriteU8(_rw, len))
      return false;
    if (!SDL_RWwrite(_rw, desc.c_str(), len, 1))
      return false;
  }

  return true;
}

bool Serializer::writeGlobals() {
  int64_t fPtr = SDL_RWtell(_rw);
  if (fPtr < 0)
    return false;
  if (!SDL_WriteBE32(_rw, 0)) // 4 placeholder bytes for the actual number of globals
    return false;

  lua_gettable(_L, LUA_GLOBALSINDEX);
  lua_pushnil(_L);
  uint32_t numGlobals = 0;
  while (lua_next(_L, LUA_GLOBALSINDEX) != 0) {
    if (!lua_isstring(_L, -2)) {
      lua_pop(_L, 1);
      continue;
    }

    std::string val;
    lua_pushvalue(_L, -2);
    const std::string key = lua_tostring(_L, -1);
    lua_pop(_L, 1);

    switch (lua_type(_L, -1)) {
    case LUA_TBOOLEAN: {
      val = lua_toboolean(_L, -1) ? "true" : "false";
      break;
    }
    case LUA_TSTRING:
      val += '"';
      // Fallthrough
    case LUA_TNUMBER: {
      val += lua_tostring(_L, -1);
      if (val[0] == '"')
        val += '"';
      break;
    }
    case LUA_TTABLE: {
      // This makes the assumption that all values in the global table have a string key
      val = stringifyTable("_G[\"" + key + "\"]");
      break;
    }
    default: {
      lua_pop(_L, 1);
      continue;
    }
    }

    numGlobals++;
    if (!writeGlobalField(key, val))
      return false;
    lua_pop(_L, 1);
  }

  if (SDL_RWseek(_rw, fPtr, RW_SEEK_SET) < 0) // Restore file pointer to 4 placeholder bytes
    return false;
  if (!SDL_WriteBE32(_rw, numGlobals))
    return false;
  if (SDL_RWseek(_rw, 0, RW_SEEK_END) < 0) // Restore file pointer to end
    return false;

  return true;
}

}
