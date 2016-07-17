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
#include "Audio.h"
#include "CameraManager.h"
#include "Config.h"
#include "Control.h"
#include "CursorManager.h"
#include "Log.h"
#include "Room.h"
#include "Spot.h"
#include "TimerManager.h"

#include <vector>

namespace dagon {

const char SAVIdent[] = { '\x44', '\x41', '\x47', '\x4F', '\x4E', '\x53', '\x41', '\x56', '\x45' };

////////////////////////////////////////////////////////////
// Implementation - Constructor & Destructor
////////////////////////////////////////////////////////////

Deserializer::Deserializer(lua_State *L, SDL_RWops *rw) : _L(L),_rw(rw) {}

Deserializer::~Deserializer() {
  SDL_RWclose(_rw);
}

////////////////////////////////////////////////////////////
// Implementation - Private
////////////////////////////////////////////////////////////

const char *Deserializer::readFunction(lua_State *L, void *data, size_t *size) {
  LuaReaderData *readerData = reinterpret_cast<LuaReaderData*>(data);
  void *p = const_cast<char*>(readerData->buf);
  if (!SDL_RWread(readerData->rw, p, readerData->numBytes, 1)) {
    readerData->readError = true;
    *size = 0;
    return nullptr;
  }

  *size = readerData->numBytes;
  return readerData->buf;
}

////////////////////////////////////////////////////////////
// Implementation - Gets
////////////////////////////////////////////////////////////

std::string Deserializer::version() {
  return _dgVersion;
}

std::string Deserializer::preview() {
  return _preview;
}

std::string Deserializer::roomName() {
  return _roomName;
}

////////////////////////////////////////////////////////////
// Implementation - State changes
////////////////////////////////////////////////////////////

bool Deserializer::readHeader() {
  { // Read magic number
    char buf[sizeof(SAVIdent)];
    if (!SDL_RWread(_rw, buf, sizeof(SAVIdent), 1))
      return false;
    if (std::memcmp(SAVIdent, buf, sizeof(SAVIdent)) != 0) {
      Log::instance().error(kModScript, "Unexpected magic number. This is not a Dagon save file.");
      return false;
    }
  }

  { // Read Dagon version string
    uint8_t len = SDL_ReadU8(_rw);
    std::vector<uint8_t> buf(len);
    if (len > 0 && !SDL_RWread(_rw, buf.data(), buf.capacity(), 1))
      return false;
    _dgVersion = std::string(buf.begin(), buf.end());
    if (_dgVersion.empty()) {
      Log::instance().error(kModScript, "Empty version string.");
      return false;
    }
  }

  { // Read preview text
    uint8_t len = SDL_ReadU8(_rw);
    std::vector<uint8_t> buf(len);
    if (len > 0 && !SDL_RWread(_rw, buf.data(), buf.capacity(), 1))
      return false;
    _preview = std::string(buf.begin(), buf.end());
  }

  { // Read Room name
    uint8_t len = SDL_ReadU8(_rw);
    std::vector<uint8_t> buf(len);
    if (len > 0 && !SDL_RWread(_rw, buf.data(), buf.capacity(), 1))
      return false;
    _roomName = std::string(buf.begin(), buf.end());
    if (_roomName.empty()) {
      Log::instance().error(kModScript, "Empty room name.");
      return false;
    }
  }

  return true;
}

bool Deserializer::readScriptData() {
  uint32_t numFields = SDL_ReadBE32(_rw);
  for (uint32_t i = 0; i < numFields; i++) {
    uint16_t lineLen = SDL_ReadBE16(_rw);
    std::vector<uint8_t> lineBuf(lineLen);
    if (!SDL_RWread(_rw, lineBuf.data(), lineBuf.capacity(), 1))
      return false;

    std::string line(lineBuf.begin(), lineBuf.end());
    int errCode = luaL_dostring(_L, line.c_str());
    if (errCode != 0) { // Perhaps one of the tables we are inserting into doesn't exist
      // TODO: begin creating empty tables at the deepest level of the line
    }
  }

  return true;
}

void Deserializer::toggleSpots() {
  // Restore spot activation statuses
  uint16_t numSpots = SDL_ReadBE16(_rw);
  for (uint16_t i = 0; i < numSpots; i++) {
    uint64_t hash = SDL_ReadBE64(_rw);

    auto spotIter = Control::instance().invObjMap.find(hash);
    if (spotIter == Control::instance().invObjMap.end()) {
      Log::instance().warning(kModScript, "No Spot matches hash %llu", hash);
      SDL_ReadU8(_rw); // Read Spot status.
      continue;
    }

    Spot *spot = static_cast<Spot*>(spotIter->second);
    bool enable = static_cast<bool>(SDL_ReadU8(_rw));

    if (enable)
      spot->enable(true); // TODO: find out if we should always force enable.
    else
      spot->disable(true); // TODO: find out if we should always force disable.
  }
}

Node *Deserializer::readNode() {
  uint64_t hash = SDL_ReadBE64(_rw);

  auto nodeIter = Control::instance().invObjMap.find(hash);
  if (nodeIter == Control::instance().invObjMap.end()) {
    Log::instance().warning(kModScript, "No Node object matches hash %llu", hash);
    return nullptr;
  }

  return static_cast<Node*>(nodeIter->second);
}

void Deserializer::restorePreviousNodes() {
  uint16_t numSlides = SDL_ReadBE16(_rw);
  for (uint16_t i = 0; i < numSlides; i++) {
    uint64_t hash = SDL_ReadBE64(_rw);
    uint64_t prevHash = SDL_ReadBE64(_rw);

    auto nodeIter = Control::instance().invObjMap.find(hash);
    if (nodeIter == Control::instance().invObjMap.end()) {
      Log::instance().warning(kModScript, "No Slide matches hash %llu", hash);
      continue;
    }

    auto prevNodeIter = Control::instance().invObjMap.find(prevHash);
    if (prevNodeIter == Control::instance().invObjMap.end()) {
      Log::instance().warning(kModScript, "No Node matches hash %llu", prevHash);
      continue;
    }

    Node *node = static_cast<Node*>(nodeIter->second);
    Node *prevNode = static_cast<Node*>(prevNodeIter->second);
    node->setPreviousNode(prevNode);
  }
}

bool Deserializer::adjustCamera() {
  uint8_t hAngleLen = SDL_ReadU8(_rw);
  std::vector<uint8_t> hAngleBuf(hAngleLen);
  if (!SDL_RWread(_rw, hAngleBuf.data(), hAngleBuf.capacity(), 1))
    return false;
  const std::string hAngleStr(hAngleBuf.begin(), hAngleBuf.end());

  try {
    int hAngle = std::stoi(hAngleStr);
    CameraManager::instance().setAngleHorizontal(hAngle);
  }
  catch (std::exception &e) {
    Log::instance().warning(kModScript, "Unable to parse horizontal angle: %s", hAngleStr);
  }

  uint8_t vAngleLen = SDL_ReadU8(_rw);
  std::vector<uint8_t> vAngleBuf(vAngleLen);
  if (!SDL_RWread(_rw, vAngleBuf.data(), vAngleBuf.capacity(), 1))
    return false;
  const std::string vAngleStr(vAngleBuf.begin(), vAngleBuf.end());

  try {
    int vAngle = std::stoi(vAngleStr);
    CameraManager::instance().setAngleVertical(vAngle);
  }
  catch (std::exception &e) {
    Log::instance().warning(kModScript, "Unable to parse vertical angle: %s", vAngleStr);
  }

  uint8_t fovLen = SDL_ReadU8(_rw);
  std::vector<uint8_t> fovBuf(fovLen);
  if (!SDL_RWread(_rw, fovBuf.data(), fovBuf.capacity(), 1))
    return false;
  const std::string fovStr(fovBuf.begin(), fovBuf.end());

  try {
    float fov = std::stof(fovStr);
    bool lock = CameraManager::instance().isLocked();
    if (lock)
      CameraManager::instance().unlock();
    CameraManager::instance().setFieldOfView(fov);
    if (lock)
      CameraManager::instance().lock();
  }
  catch (std::exception &e) {
    Log::instance().warning(kModScript, "Unable to parse field of view: %s", fovStr);
  }

  return true;
}

void Deserializer::toggleAudio() {
  uint16_t numAudios = SDL_ReadBE16(_rw);
  for (uint16_t i = 0; i < numAudios; i++) {
    uint64_t hash = SDL_ReadBE64(_rw);
    uint8_t state = SDL_ReadU8(_rw);

    auto audioIter = Control::instance().invObjMap.find(hash);
    if (audioIter == Control::instance().invObjMap.end()) {
      Log::instance().warning(kModScript, "No Audio object matches hash %llu", hash);
      continue;
    }

    Audio *audio = static_cast<Audio*>(audioIter->second);
    switch (state) {
    case kAudioInitial: {
      // TODO: find out what to do now?
      break;
    }
    case kAudioPlaying: {
      if (!audio->isPlaying())
        audio->play();
      break;
    }
    case kAudioPaused: {
      audio->pause();
      break;
    }
    case kAudioStopped: {
      audio->stop();
      break;
    }
    default: {
      Log::instance().warning(kModScript, "Unknown audio state read: %d", state);
    }
    }
  }
}

bool Deserializer::readTimers() {
  uint16_t numTimers = SDL_ReadBE16(_rw);

  for (uint16_t i = 0; i < numTimers; i++) {
    bool isLoopable = SDL_ReadU8(_rw);

    uint8_t triggerLen = SDL_ReadU8(_rw);
    std::vector<uint8_t> triggerBuf(triggerLen);
    if (!SDL_RWread(_rw, triggerBuf.data(), triggerBuf.capacity(), 1))
      return false;
    const std::string triggerStr(triggerBuf.begin(), triggerBuf.end());

    try {
      double trigger = std::stod(triggerStr);

      uint8_t elapsedLen = SDL_ReadU8(_rw);
      std::vector<uint8_t> elapsedBuf(elapsedLen);
      if (!SDL_RWread(_rw, elapsedBuf.data(), elapsedBuf.capacity(), 1))
        return false;
      const std::string elapsedStr(elapsedBuf.begin(), elapsedBuf.end());

      try {
        double elapsed = std::stod(elapsedStr);

        LuaReaderData data;
        uint16_t funcLen = SDL_ReadBE16(_rw);
        std::vector<char> funcBuf(funcLen);
        data.rw = _rw;
        data.numBytes = funcLen;
        data.buf = funcBuf.data();
        data.readError = false;

        int errCode = lua_load(_L, readFunction, &data, "");
        if (data.readError)
          return false;
        if (errCode != 0) {
          Log::instance().warning(kModScript, "Couldn't read timer function. Error: %s",
                                  lua_tostring(_L, -1));
          lua_pop(_L, 1);
          continue;
        }
        
        int ref = luaL_ref(_L, LUA_REGISTRYINDEX);
        TimerManager::instance().create(trigger, isLoopable, ref);
        TimerManager::instance().timers().back().lastTime -= elapsed; // Rewind start time.
      }
      catch (std::exception &e) {
        Log::instance().warning(kModScript, "Malformed timer. Couldn't parse time %s",
                                elapsedStr);
        { // Read function block
          uint16_t len = SDL_ReadBE16(_rw);
          std::vector<uint8_t> buf(len);
          if (!SDL_RWread(_rw, buf.data(), buf.capacity(), 1))
            return false;
        }
        continue;
      }
    }
    catch (std::exception &e) {
      Log::instance().warning(kModScript, "Malformed timer. Couldn't parse time %s", triggerStr);
      { // Read string block
        uint8_t len = SDL_ReadU8(_rw);
        std::vector<uint8_t> buf(len);
        if (!SDL_RWread(_rw, buf.data(), buf.capacity(), 1))
          return false;
      }
      { // Read function block
        uint16_t len = SDL_ReadBE16(_rw);
        std::vector<uint8_t> buf(len);
        if (!SDL_RWread(_rw, buf.data(), buf.capacity(), 1))
          return false;
      }
      continue;
    }
  }

  return true;
}

void Deserializer::readControlMode() {
  uint8_t controlMode = SDL_ReadU8(_rw);
  switch (controlMode) {
  case kControlDrag:
  case kControlFixed:
  case kControlFree: {
    Config::instance().controlMode = controlMode;
    break;
  }
  default: {
    Log::instance().warning(kModScript, "Unknown control mode: %d", controlMode);
  }
  }
}

void Deserializer::readCursorState() {
  uint8_t cursorType = SDL_ReadU8(_rw);
  if (cursorType >= kCursorNormal && cursorType <= kCursorCustom) { // Not very nice, but better
                                                                    // than switching over the
                                                                    // large enum CursorTypes
    CursorManager::instance().setType(cursorType);
  }
  else {
    Log::instance().warning(kModScript, "Unknown cursor state: %d", cursorType);
  }
}

}
