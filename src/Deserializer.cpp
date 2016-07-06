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
  uint16_t numRooms = SDL_ReadBE16(_rw);
  if (numRooms != Control::instance().numRooms()) {
    Log::instance().warning(kModScript, "Room number mismatch. Expected %d. Got %d", numRooms,
                            Control::instance().numRooms());
  }

  for (Room *room : Control::instance().rooms()) {
    if (numRooms == 0)
      break;

    uint16_t numNodes = SDL_ReadBE16(_rw);
    if (numNodes != room->numNodes()) {
      Log::instance().warning(kModScript, "Node number mismatch. Expected %d. Got %d", numNodes,
                              room->numNodes());
    }

    if (room->hasNodes()) {
      room->beginIteratingNodes();

      do {
        Node *node = room->iterator();

        uint16_t numSpots = SDL_ReadBE16(_rw);
        if (numSpots != node->numSpots()) {
          Log::instance().warning(kModScript, "Spot number mismatch. Expected %d. Got %d",
                                  numSpots, node->numSpots());
        }

        if (node->hasSpots()) {
          node->beginIteratingSpots();

          do {
            Spot *spot = node->currentSpot();

            bool enable = SDL_ReadU8(_rw);
            if (enable)
              spot->enable(true); // TODO: find out if we should always force enable.
            else
              spot->disable(true); // TODO: find out if we should always force disable.

            numSpots--;
          } while (node->iterateSpots() && numSpots > 0);

          // Read remaining spots (necessary if number of spots has been reduced)
          for (size_t i = 0; i < numSpots; i++)
            SDL_ReadU8(_rw);
        }

        numNodes--;
      } while (room->iterateNodes() && numNodes > 0);

      // Read remaining nodes (necessary if number of nodes has been reduced)
      for (size_t i = 0; i < numNodes; i++) {
        uint16_t numSpots = SDL_ReadBE16(_rw);
        for (size_t j = 0; j < numSpots; j++)
          SDL_ReadU8(_rw);
      }
    }

    numRooms--;
  }

  // Read remaining rooms (necessary if number of rooms has been reduced)
  for (size_t i = 0; i < numRooms; i++) {
    uint16_t numNodes = SDL_ReadBE16(_rw);
    for (size_t j = 0; j < numNodes; j++) {
      uint16_t numSpots = SDL_ReadBE16(_rw);
      for (size_t k = 0; k < numSpots; k++)
        SDL_ReadU8(_rw);
    }
  }
}

Node *Deserializer::readNode() {
  Room *room = Control::instance().currentRoom();
  uint16_t nodeIdx = SDL_ReadBE16(_rw);
  Node *node;

  if (room->hasNodes()) {
    room->beginIteratingNodes();

    do {
      node = room->iterator();

      if (nodeIdx == 0)
        break;
      nodeIdx--;
    } while (room->iterateNodes());
  }

  return node;
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
  Room *room = Control::instance().currentRoom();

  uint16_t numAudios = SDL_ReadBE16(_rw);
  if (numAudios != room->arrayOfAudios().size()) {
    Log::instance().warning(kModScript, "Audio number mismatch. Expected %d. Got %d", numAudios,
                            room->arrayOfAudios().size());
  }

  for (Audio *audio : room->arrayOfAudios()) {
    uint8_t state = SDL_ReadU8(_rw);

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

    numAudios--;
  }

  // Read remaining audios (necessary if number of audios has been reduced)
  for (uint16_t i = 0; i < numAudios; i++)
    SDL_ReadU8(_rw);
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

}
