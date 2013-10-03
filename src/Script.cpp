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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Config.h"
#include "AudioManager.h"
#include "CursorManager.h"
#include "FeedManager.h"
#include "Log.h"
#include "Proxy.h"
#include "Script.h"
#include "TimerManager.h"

#include "Luna.h"

// The way the engine is designed, all static Lua functions will have
// to grab a reference to the Control singleton and Log when required.
// No performance hit whatsoever but something to keep in mind.

// Spots launch audio and video, then perform a switch in that order.
// This is to avoid doing many customs.

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

Script::Script() :
config(Config::instance()),
log(Log::instance())
{
  _isInitialized = false;
  _isSuspended = false;
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

Script::~Script() {
  if (_isInitialized)
    lua_close(_L);
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

// TODO: Save system: in every operation, save to a file the
// Lua object being modified (retrieve ID from Object), then
// create a string with the actual modification, ie: spot:enable()
// Filter switch and other heavy duty operations.

// TODO: Support loading script from parameters
// TODO: Consider seeking paths again if debug mode was enabled
void Script::init() {
  char script[kMaxFileLength];
  
  _L = lua_open();
  luaL_openlibs(_L);
  
  // The following code attempts to load a config file, and if it does exist
  // copies the created table to the Config metatable
  if (luaL_loadfile(_L, config.path(kPathApp, kDefConfigFile, kObjectGeneric).c_str()) == 0) {
    lua_newtable(_L);
    lua_pushvalue(_L, -1);
    int ref = lua_ref(_L, LUA_REGISTRYINDEX);
    lua_setfenv(_L, -2);
    
    lua_pcall(_L, 0, 0, 0);
    
    lua_rawgeti(_L, LUA_REGISTRYINDEX, ref);
    lua_pushnil(_L);
    while (lua_next(_L, 1) != 0) {
      ConfigLibSet(_L);
      
      lua_pop(_L, 1);
    }
  }
  else
    log.error(kModScript, "%s", kString14015);
  
  log.trace(kModScript, "%s", kString14001);
  log.info(kModScript, "%s: %s", kString14002, LUA_RELEASE);
  
  lua_getglobal(_L, "_G");
  _registerEnums();
  
  // Config modules path
  luaL_dostring(_L, "package.path = package.path .. \";Modules/?.lua\"");
  
  // Register all proxys
  Luna<AudioProxy>::Register(_L);
  Luna<ButtonProxy>::Register(_L);
  Luna<ImageProxy>::Register(_L);
  Luna<NodeProxy>::Register(_L);
  Luna<OverlayProxy>::Register(_L);
  Luna<RoomProxy>::Register(_L);
  Luna<SlideProxy>::Register(_L);
  Luna<SpotProxy>::Register(_L);
  
  // Register all libs
  luaL_register(_L, "cursor", kCursorLib);
  luaL_register(_L, "system", SystemLib);
  
  // The config lib requires a special treatment because
  // it exports properties, not methods
  lua_newuserdata(_L, sizeof(void*));
  
  lua_pushvalue(_L, -1);
  
  luaL_newmetatable(_L, "ConfigLib");
  luaL_register(_L, NULL, ConfigLib);
  lua_setmetatable(_L, -2);
  
  lua_newtable(_L);
  lua_setfenv(_L, -2);
  
  lua_setglobal(_L, "config");
  
  // Same with the camera settings
  lua_newuserdata(_L, sizeof(void*));
  
  lua_pushvalue(_L, -1);
  
  luaL_newmetatable(_L, "CameraLib");
  luaL_register(_L, NULL, CameraLib);
  lua_setmetatable(_L, -2);
  
  lua_newtable(_L);
  lua_setfenv(_L, -2);
  
  lua_setglobal(_L, "camera");
  
  // Same with the effects properties
  lua_newuserdata(_L, sizeof(void*));
  
  lua_pushvalue(_L, -1);
  
  luaL_newmetatable(_L, "EffectsLib");
  luaL_register(_L, NULL, EffectsLib);
  lua_setmetatable(_L, -2);
  
  lua_newtable(_L);
  lua_setfenv(_L, -2);
  
  lua_setglobal(_L, "effects");
  
  // Now we register the global functions that don't belong to any library
  _registerGlobals();
  
  // We're ready to roll, let's attempt to load the script in a Lua thread
  _thread = lua_newthread(_L);
  snprintf(script, kMaxFileLength, "%s.lua", config.script().c_str());
  if (luaL_loadfile(_thread, config.path(kPathApp, script, kObjectGeneric).c_str()) == 0)
    _isInitialized = true;
  else {
    // Not found!
    log.error(kModScript, "%s: %s", kString14006, script);
    Control::instance().processKey(kKeyTab, false); // Simulate tab key to open the console
  }
}

const char* Script::module() {
  return _arrayOfModuleNames.back().c_str();
}

bool Script::isExecutingModule() {
  // We simply test if the vector is empty. If it is,
  // it means no modules are stacked.
  return !_arrayOfModuleNames.empty();
}

void Script::processCallback(int handler, int object) {
  if (object) {
    // Grab the reference to the Lua object and set it to 'self'
    lua_rawgeti(_thread, LUA_REGISTRYINDEX, object);
    lua_setglobal(_thread, "self");
  }
  
  lua_rawgeti(_thread, LUA_REGISTRYINDEX, handler);
  
  if (_isSuspended) {
    if (int result =  lua_pcall(_thread, 0, 0, 0))
      _error(result);
  }
  else {
    if (int result = lua_resume(_thread, 0))
      _error(result);
  }
}

void Script::processCommand(const char *command) {
  if (int result = luaL_loadbuffer(_thread, command, strlen(command), "command") ||
      lua_pcall(_thread, 0, 0, 0))
    _error(result);
}

void Script::resume() {
  if (_isSuspended) {
    _isSuspended = false;
    if (int result = lua_resume(_thread, 0))
      _error(result);
  }
}

void Script::run() {
  if (_isInitialized) {
    if (int result = lua_resume(_thread, 0))
      _error(result);
  }
}

void Script::setModule(const char* theModule) {
  _arrayOfModuleNames.push_back(theModule);
}

int Script::suspend() {
  if (!_isSuspended) {
    _isSuspended = true;
    return lua_yield(_thread, 0);
  }
  
  return 0;
}

void Script::unsetModule() {
  _arrayOfModuleNames.pop_back();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void Script::_error(int result) {
  if (result != LUA_YIELD) {
    switch (result) {
      case LUA_ERRRUN:
        log.error(kModScript, "%s", kString14010);
        break;
      case LUA_ERRMEM:
        log.error(kModScript, "%s", kString14011);
        break;
      case LUA_ERRERR:
        log.error(kModScript, "%s", kString14012);
        break;
      case LUA_ERRSYNTAX:
        log.error(kModScript, "%s", kString14013);
        break;
    }
    
    // Now print the last Lua string in the stack, which should indicate the error
    log.error(kModScript, "%s", lua_tostring(_L, -1));
  }
}

int Script::_globalCurrentNode(lua_State *L) {
  Node* node = Control::instance().currentNode();
  
  // Grab the reference to the Lua object and set it as 'self'
  if (node) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, node->luaObject());
    
    return 1;
  }
  
  return 0;
}

int Script::_globalCurrentRoom(lua_State *L) {
  Room* room = Control::instance().currentRoom();
  
  // Grab the reference to the Lua object and set it as 'self'
  if (room) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, room->luaObject());
    
    return 1;
  }
  
  return 0;
}

int Script::_globalCutscene(lua_State *L) {
  Control::instance().cutscene(luaL_checkstring(L, 1));
  
  return Script::instance().suspend();
}

int Script::_globalFeed(lua_State *L) {
  if (lua_isstring(L, 2)) {
    FeedManager::instance().showAndPlay(luaL_checkstring(L, 1), lua_tostring(L, 2));
  }
  else FeedManager::instance().show(luaL_checkstring(L, 1));
  
  return 0;
}

int Script::_globalHotkey(lua_State *L) {
  Control::instance().registerHotkey((int)luaL_checknumber(L, 1), luaL_checkstring(L, 2));
  
  return 0;
}

int Script::_globalLookAt(lua_State *L) {
  // NOTE: The user cannot use a custom angle between 0 and 5
  
  int horizontal = static_cast<int>(luaL_checknumber(L, 1));
  int vertical = 0; // Always assumes a 'flat' vertical angle
  bool instant = false;
  
  switch (horizontal) {
    case kNorth:   horizontal = 0;     break;
    case kEast:    horizontal = 90;    break;
    case kSouth:   horizontal = 180;   break;
    case kWest:    horizontal = 270;   break;
  }
  
  if (lua_gettop(L) > 1) {
    vertical = static_cast<int>(luaL_checknumber(L, 2));
    
    switch (vertical) {
      case kUp:      vertical = 360;     break;
      case kDown:    vertical = -360;    break;
    }
  }
  
  if (lua_gettop(L) == 3) {
    instant = lua_toboolean(L, 3);
  }
  
  
  Control::instance().lookAt(horizontal, vertical, instant);
  
  if (instant) return 0;
  else return Script::instance().suspend();
}

int Script::_globalPlay(lua_State *L) {
  Audio* audio = new Audio;
  
  audio->setResource(luaL_checkstring(L, 1));
  audio->setStatic();
  AudioManager::instance().registerAudio(audio);
  AudioManager::instance().requestAudio(audio);
  audio->play();
  
  return 0;
}

int Script::_globalQueue(lua_State *L) {
  FeedManager::instance().queue(luaL_checkstring(L, 1), lua_tostring(L, 2));
  
  return 0;
}

int Script::_globalPrint (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  
  lua_getglobal(L, "tostring");
  for (i = 1; i <= n; i++) {
    const char *s;
    lua_pushvalue(L, -1);  /* function to be called */
    lua_pushvalue(L, i);   /* value to print */
    lua_call(L, 1, 1);
    s = lua_tostring(L, -1);  /* get result */
    if (s == NULL)
      return luaL_error(L, LUA_QL("tostring") " must return a string to "
                        LUA_QL("print"));
    if (i > 1) fputs("\t", stdout);
    Log::instance().trace(kModScript, "%s", s);
    lua_pop(L, 1);  /* pop result */
  }
  return 0;
}

int Script::_globalRegister(lua_State *L) {
  if (!lua_isfunction(L, -1)) {
    Log::instance().error(kModScript, kString14014);
    
    return 0;
  }
  
  int ref = luaL_ref(L, LUA_REGISTRYINDEX);  // pop and return a reference to the table.
  
  Control::instance().registerGlobalHandler((unsigned int)luaL_checknumber(L, 1), ref);
  
  return 0;
}

int Script::_globalRoom(lua_State *L) {
  // NOTE: This is a convenience Lua hack, which in theory is 100% safe.
  // Must do lots of testing. Basically, this helper creates a room instance,
  // loads the corresponding Lua file and makes sure it's never loaded twice.
  //
  // UPDATE: So far it has worked great. Our motto is: "if it works, it stays".
  // PS: Not really.
  
  char module[kMaxObjectName];
  strncpy(module, luaL_checkstring(L, 1), kMaxObjectName);
  
  // We first check if the object already exists
  lua_getglobal(L, module);
  if (!lua_isuserdata(L, -1)) {
    char line[kMaxLogLength], script[kMaxFileLength];
    
    // Parse line to automatically create the room
    snprintf(line, kMaxLogLength, "%s = Room(\"%s\")", module, module);
    luaL_dostring(L, line);
    
    // Load the corresponding Lua file
    // TODO: Read rooms from path
    snprintf(script, kMaxFileLength, "%s.lua", module);
    
    if (luaL_loadfile(L, Config::instance().path(kPathApp, script, kObjectRoom).c_str()) == 0) {
      Script::instance().setModule(module);
      lua_pcall(L, 0, 0, 0);
      Script::instance().unsetModule();
    }
  }
  
  // Nothing else to do...
  
  return 0;
}

int Script::_globalSleep(lua_State *L) {
  Control::instance().sleep((int)luaL_checknumber(L, 1));
  
  return Script::instance().suspend();
}

int Script::_globalSetFont(lua_State *L) {
  switch ((int)luaL_checknumber(L, 1)) {
    case FEED:
      FeedManager::instance().setFont(luaL_checkstring(L, 2),
                                        static_cast<unsigned int>(luaL_checknumber(L, 3)));
      break;
  }
  
  return 0;
}

int Script::_globalSnap(lua_State *L) {
  Control::instance().takeSnapshot();
  
  return 0;
}

int Script::_globalSwitch(lua_State *L) {
  switch (DGCheckProxy(L, 1)) {
    case kObjectNode:
      Control::instance().switchTo(ProxyToNode(L, 1), lua_toboolean(L, 2));
      break;
    case kObjectRoom:
      Control::instance().switchTo(ProxyToRoom(L, 1), lua_toboolean(L, 2));
      break;
    case kObjectSlide:
      Control::instance().switchTo(ProxyToSlide(L, 1), lua_toboolean(L, 2));
      break;
    case kObjectGeneric:
      Log::instance().error(kModScript, "%s", kString14003);
      break;
      
    case kObjectNone:
      Log::instance().error(kModScript, "%s", kString14004);
      break;
  }
  
  return 0;
}

int Script::_globalStartTimer(lua_State *L) {
  if (!lua_isfunction(L, -1)) {
    Log::instance().trace(kModScript, "%s", kString14009);
    
    return 0;
  }
  
  int ref = luaL_ref(L, LUA_REGISTRYINDEX);  // Pop and return a reference to the table.
  bool shouldLoop = lua_toboolean(L, 2);
  int handle = TimerManager::instance().create(luaL_checknumber(L, 1), shouldLoop, ref);
  
  lua_pushnumber(L, handle);
  
  return 1;
}

int Script::_globalStopTimer(lua_State *L) {
  TimerManager::instance().disable(static_cast<int>(luaL_checknumber(L, 1)));
  
  return 0;
}

int Script::_globalVersion(lua_State *L) {
  lua_pushstring(L, DAGON_VERSION_STRING);
  
  return 1;
}

void Script::_registerEnums() {
  // Push all enum values
  DGLuaEnum(_L, AUDIO, kObjectAudio);
  DGLuaEnum(_L, FUNCTION, kActionFunction);
  DGLuaEnum(_L, IMAGE, kObjectTexture);
  DGLuaEnum(_L, FEED, kActionFeed);
  DGLuaEnum(_L, SWITCH, kActionSwitch);
  DGLuaEnum(_L, VIDEO, kObjectVideo);
  
  DGLuaEnum(_L, NORMAL, kCursorNormal);
  DGLuaEnum(_L, DRAGGING, kCursorDrag);
  DGLuaEnum(_L, LEFT, kCursorLeft);
  DGLuaEnum(_L, RIGHT, kCursorRight);
  DGLuaEnum(_L, UP, kCursorUp);
  DGLuaEnum(_L, DOWN, kCursorDown);
  DGLuaEnum(_L, UP_LEFT, kCursorUpLeft);
  DGLuaEnum(_L, UP_RIGHT, kCursorUpRight);
  DGLuaEnum(_L, DOWN_LEFT, kCursorDownLeft);
  DGLuaEnum(_L, DOWN_RIGHT, kCursorDownRight);
  DGLuaEnum(_L, FORWARD, kCursorForward);
  DGLuaEnum(_L, BACKWARD, kCursorBackward);
  DGLuaEnum(_L, USE, kCursorUse);
  DGLuaEnum(_L, LOOK, kCursorLook);
  DGLuaEnum(_L, TALK, kCursorTalk);
  DGLuaEnum(_L, CUSTOM, kCursorCustom);
  
  DGLuaEnum(_L, SLIDE, kNorth);
  DGLuaEnum(_L, NORTH, kNorth);
  DGLuaEnum(_L, EAST, kEast);
  DGLuaEnum(_L, WEST, kWest);
  DGLuaEnum(_L, SOUTH, kSouth);
  DGLuaEnum(_L, UP, kUp);
  DGLuaEnum(_L, DOWN, kDown);
  DGLuaEnum(_L, NORTHEAST, kNorthEast);
  DGLuaEnum(_L, SOUTHEAST, kSouthEast);
  DGLuaEnum(_L, NORTHWEST, kNorthWest);
  DGLuaEnum(_L, SOUTHWEST, kSouthWest);
  DGLuaEnum(_L, CURRENT, kCurrent);
  
  DGLuaEnum(_L, DRAG, kControlDrag);
  DGLuaEnum(_L, FIXED, kControlFixed);
  DGLuaEnum(_L, FREE, kControlFree);
  
  DGLuaEnum(_L, ENTER_NODE, EventEnterNode);
  DGLuaEnum(_L, ENTER_ROOM, EventEnterRoom);
  DGLuaEnum(_L, LEAVE_NODE, EventLeaveNode);
  DGLuaEnum(_L, LEAVE_ROOM, EventLeaveRoom);
  DGLuaEnum(_L, PRE_RENDER, EventPreRender);
  DGLuaEnum(_L, POST_RENDER, EventPostRender);
  DGLuaEnum(_L, MOUSE_BUTTON, EventMouseDown);
  DGLuaEnum(_L, MOUSE_RIGHT_BUTTON, EventMouseRightDown);
  DGLuaEnum(_L, MOUSE_MOVE, EventMouseMove);
  DGLuaEnum(_L, RESIZE, EventResize);
  
  DGLuaEnum(_L, BLACK, kColorBlack);
  DGLuaEnum(_L, BLUE, kColorBlue);
  DGLuaEnum(_L, GREEN, kColorGreen);
  DGLuaEnum(_L, CYAN, kColorCyan);
  DGLuaEnum(_L, RED, kColorRed);
  DGLuaEnum(_L, MAGENTA, kColorMagenta);
  DGLuaEnum(_L, BROWN, kColorBrown);
  DGLuaEnum(_L, LIGHTGRAY, kColorLightGray);
  DGLuaEnum(_L, DARKGRAY, kColorDarkGray);
  DGLuaEnum(_L, BRIGHTBLUE, kColorBrightBlue);
  DGLuaEnum(_L, BRIGHTGREEN, kColorBrightGreen);
  DGLuaEnum(_L, BRIGHTCYAN, kColorBrightCyan);
  DGLuaEnum(_L, BRIGHTRED, kColorBrightRed);
  DGLuaEnum(_L, BRIGHTMAGENTA, kColorBrightMagenta);
  DGLuaEnum(_L, YELLOW, kColorYellow);
  DGLuaEnum(_L, WHITE, kColorWhite);
  
  DGLuaEnum(_L, F1, kKeyF1);
  DGLuaEnum(_L, F2, kKeyF2);
  DGLuaEnum(_L, F3, kKeyF3);
  DGLuaEnum(_L, F4, kKeyF4);
  DGLuaEnum(_L, F5, kKeyF5);
  DGLuaEnum(_L, F6, kKeyF6);
  DGLuaEnum(_L, F7, kKeyF7);
  DGLuaEnum(_L, F8, kKeyF8);
  DGLuaEnum(_L, F9, kKeyF9);
  DGLuaEnum(_L, F10, kKeyF10);
  DGLuaEnum(_L, F11, kKeyF11);
  DGLuaEnum(_L, F12, kKeyF12);
  
  DGLuaEnum(_L, SLOW, kFadeSlow);
  DGLuaEnum(_L, SLOWEST, kFadeSlowest);
  DGLuaEnum(_L, FAST, kFadeFast);
  DGLuaEnum(_L, FASTEST, kFadeFastest);
}

void Script::_registerGlobals() {
  static const struct luaL_reg globalLibs [] = {
    {"currentNode", _globalCurrentNode},
    {"currentRoom", _globalCurrentRoom},
    {"cutscene", _globalCutscene},
    {"feed", _globalFeed},
    {"hotkey", _globalHotkey},
    {"lookAt", _globalLookAt},
    {"play", _globalPlay},
    {"print", _globalPrint},
    {"queue", _globalQueue},
    {"register", _globalRegister},
    {"room", _globalRoom},
    {"setFont", _globalSetFont},
    {"snap", _globalSnap},
    {"sleep", _globalSleep},
    {"switch", _globalSwitch},
    {"startTimer", _globalStartTimer},
    {"stopTimer", _globalStopTimer},
    {"version", _globalVersion},
    {NULL, NULL}
  };
  
  lua_getglobal(_L, "_G");
  luaL_register(_L, NULL, globalLibs);
  lua_pop(_L, 1);
}
  
}
