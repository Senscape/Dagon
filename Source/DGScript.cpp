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

#include "DGConfig.h"
#include "DGAudioManager.h"
#include "DGCursorManager.h"
#include "DGFeedManager.h"
#include "DGLog.h"
#include "DGProxy.h"
#include "DGScript.h"
#include "DGSystem.h"
#include "DGTimerManager.h"

#include "Luna.h"

using namespace std;

// The way the engine is designed, all static Lua functions will have
// to grab a reference to the Control singleton and Log when required.
// No performance hit whatsoever but something to keep in mind.

// Spots launch audio and video, then perform a switch in that order.
// This is to avoid doing many customs.

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGScript::DGScript() {
    log = &DGLog::getInstance();
    config = &DGConfig::getInstance();        
    system = &DGSystem::getInstance();
    
    _isInitialized = false;
    _isSuspended = false;    
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGScript::~DGScript() {
    if (_isInitialized)
        lua_close(_L);
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

// TODO: Save system: in every operation, save to a file the
// Lua object being modified (retrieve ID from DGOBject), then
// create a string with the actual modification, ie: spot:enable()
// Filter switch and other heavy duty operations.

void DGScript::execute() {
    if (_isInitialized) {
        if (int result = lua_resume(_thread, 0))
            _error(result);
    }
}

// TODO: Support loading script from parameters
// TODO: Consider seeking paths again if debug mode was enabled
void DGScript::init(int argc, char* argv[]) {
    char script[DGMaxFileLength];
    
    // First thing we do is get the paths to load the script
    // (note that it's not necessary to init the system)
    system->findPaths(argc, argv);
    
    _L = lua_open();
    luaL_openlibs(_L);
    
    // The following code attempts to load a config file, and if it does exist
    // copies the created table to the DGConfig metatable
    if (luaL_loadfile(_L, config->path(DGPathApp, DGDefConfigFile)) == 0) {
		lua_newtable(_L);
		lua_pushvalue(_L, -1);
		int ref = lua_ref(_L, LUA_REGISTRYINDEX);
		lua_setfenv(_L, -2);

		lua_pcall(_L, 0, 0, 0);
		
		lua_rawgeti(_L, LUA_REGISTRYINDEX, ref);
		lua_pushnil(_L);
		while (lua_next(_L, 1) != 0) {
			DGConfigLibSet(_L);
			
			lua_pop(_L, 1);
		}

		log->trace(DGModScript, "========================================");
	}
    else {
		log->trace(DGModScript, "========================================");
		log->error(DGModScript, "%s", DGMsg250012);
	}
    
    log->trace(DGModScript, "%s", DGMsg050000);
    log->info(DGModScript, "%s: %s", DGMsg050001, LUA_RELEASE);
    
    lua_getglobal(_L, "_G");
    _registerEnums();
    
    // Config modules path
    luaL_dostring(_L, "package.path = package.path .. \";Modules/?.lua\"");
    
    // Register all proxys
    Luna<DGAudioProxy>::Register(_L);
    Luna<DGButtonProxy>::Register(_L);       
    Luna<DGImageProxy>::Register(_L);    
    Luna<DGNodeProxy>::Register(_L);
    Luna<DGOverlayProxy>::Register(_L);    
    Luna<DGRoomProxy>::Register(_L);
    Luna<DGSlideProxy>::Register(_L);
    Luna<DGSpotProxy>::Register(_L);
    
    // Register all libs
    luaL_register(_L, "cursor", DGCursorLib);
    luaL_register(_L, "system", DGSystemLib);
    
    // The config lib requires a special treatment because
    // it exports properties, not methods
    lua_newuserdata(_L, sizeof(void*));
    
    lua_pushvalue(_L, -1);
	
	luaL_newmetatable(_L, "DGConfigLib");
	luaL_register(_L, NULL, DGConfigLib);
	lua_setmetatable(_L, -2);
	
	lua_newtable(_L);
    lua_setfenv(_L, -2);
	
	lua_setglobal(_L, "config");
    
    // Same with the camera settings
    lua_newuserdata(_L, sizeof(void*));
    
    lua_pushvalue(_L, -1);
	
	luaL_newmetatable(_L, "DGCamLib");
	luaL_register(_L, NULL, DGCamLib);
	lua_setmetatable(_L, -2);
	
	lua_newtable(_L);
    lua_setfenv(_L, -2);
	
	lua_setglobal(_L, "camera");
    
    // Same with the effects properties
    lua_newuserdata(_L, sizeof(void*));
    
    lua_pushvalue(_L, -1);
	
	luaL_newmetatable(_L, "DGEffectsLib");
	luaL_register(_L, NULL, DGEffectsLib);
	lua_setmetatable(_L, -2);
	
	lua_newtable(_L);
    lua_setfenv(_L, -2);
	
	lua_setglobal(_L, "effects");
    
    // Now we register the global functions that don't belong to any library
    _registerGlobals();
    
    // If autorun is enabled, automatically init the system
    if (config->autorun)
        system->init();
    
    // We're ready to roll, let's attempt to load the script in a Lua thread
    _thread = lua_newthread(_L);
    snprintf(script, DGMaxFileLength, "%s.lua", config->script());
    if (luaL_loadfile(_thread, config->path(DGPathApp, script)) == 0)
        _isInitialized = true;
    else {
        // Not found!
        log->error(DGModScript, "%s: %s", DGMsg250003, script);
        DGControl::getInstance().processKey(DGKeyTab, false); // Simulate tab key to open the console
        system->run();
    }
}

const char* DGScript::module() {
    return _arrayOfModuleNames.back().c_str();
}

bool DGScript::isExecutingModule() {
    // We simply test if the vector is empty. If it is,
    // it means no modules are stacked.
    return !_arrayOfModuleNames.empty();
}

void DGScript::processCallback(int handler, int object) {
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

void DGScript::processCommand(const char *command) {
    if (int result = luaL_loadbuffer(_thread, command, strlen(command), "command") ||
                    lua_pcall(_thread, 0, 0, 0))
        _error(result);
}

void DGScript::resume() {
    if (_isSuspended) {
        _isSuspended = false;
        if (int result = lua_resume(_thread, 0))
            _error(result);
    }
}

void DGScript::run() {
    if (_isInitialized) {
        // TODO: This doesn't work if autorun is disabled
        if (!config->showSplash)
            this->execute();

        // Check if we must start the main loop ourselves
        if (config->autorun)
            system->run();
    }
}

void DGScript::setModule(const char* module) {
    _arrayOfModuleNames.push_back(module);
}

int DGScript::suspend() {
    if (!_isSuspended) {
        _isSuspended = true;
        return lua_yield(_thread, 0);
    }
    
    return 0;
}

void DGScript::unsetModule() {
    _arrayOfModuleNames.pop_back();
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGScript::_error(int result) {
    if (result != LUA_YIELD) {
        switch (result) {
            case LUA_ERRRUN:
                log->error(DGModScript, "%s", DGMsg250007);
                break;
            case LUA_ERRMEM:
                log->error(DGModScript, "%s", DGMsg250008);
                break;
            case LUA_ERRERR:
                log->error(DGModScript, "%s", DGMsg250009);
                break;
            case LUA_ERRSYNTAX:
                log->error(DGModScript, "%s", DGMsg250010);
                break;            
        }
        
        // Now print the last Lua string in the stack, which should indicate the error
        log->error(DGModScript, "%s", lua_tostring(_L, -1));
    }
}

int DGScript::_globalCurrentNode(lua_State *L) {
    DGNode* node = DGControl::getInstance().currentNode();
    
    // Grab the reference to the Lua object and set it as 'self'
    if (node) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, node->luaObject());
    
        return 1;
    }
    
    return 0;
}

int DGScript::_globalCurrentRoom(lua_State *L) {
    DGRoom* room = DGControl::getInstance().currentRoom();
    
    // Grab the reference to the Lua object and set it as 'self'
    if (room) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, room->luaObject());
        
        return 1;
    }
    
    return 0;
}

int DGScript::_globalCutscene(lua_State *L) {
    DGControl::getInstance().cutscene(luaL_checkstring(L, 1));
    
    return DGScript::getInstance().suspend();
}

int DGScript::_globalFeed(lua_State *L) {
    if (lua_isstring(L, 2)) {
        DGFeedManager::getInstance().showAndPlay(luaL_checkstring(L, 1), lua_tostring(L, 2));
    }
    else DGFeedManager::getInstance().show(luaL_checkstring(L, 1));
	
	return 0;
}

int DGScript::_globalHotkey(lua_State *L) {
    DGControl::getInstance().registerHotkey((int)luaL_checknumber(L, 1), luaL_checkstring(L, 2));
	
	return 0;
}

int DGScript::_globalLookAt(lua_State *L) {
    // NOTE: The user cannot use a custom angle between 0 and 5
    
    int horizontal = luaL_checknumber(L, 1);
    int vertical = 0; // Always assumes a 'flat' vertical angle
    bool instant = false;
    
    switch (horizontal) {
        case DGNorth:   horizontal = 0;     break;
        case DGEast:    horizontal = 90;    break;
        case DGSouth:   horizontal = 180;   break;
        case DGWest:    horizontal = 270;   break;
    }
    
    if (lua_gettop(L) > 1) {
        vertical = luaL_checknumber(L, 2);
        
        switch (vertical) {
            case DGUp:      vertical = 360;     break;
            case DGDown:    vertical = -360;    break;
        }    
    }
    
    if (lua_gettop(L) == 3) {
        instant = lua_toboolean(L, 3);
    }
      
    
    DGControl::getInstance().lookAt(horizontal, vertical, instant);
    
    if (instant) return 0;
    else return DGScript::getInstance().suspend();
}

int DGScript::_globalPlay(lua_State *L) {
    DGAudio* audio = new DGAudio;
    
    audio->setResource(luaL_checkstring(L, 1));
    DGAudioManager::instance().registerAudio(audio);
    DGAudioManager::instance().requestAudio(audio);
    audio->play();
    
	return 0;
}

int DGScript::_globalQueue(lua_State *L) {
    DGFeedManager::getInstance().queue(luaL_checkstring(L, 1), lua_tostring(L, 2));
	
	return 0;
}

int DGScript::_globalPrint (lua_State *L) {
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
        DGLog::getInstance().trace(DGModNone, "%s", s);
		lua_pop(L, 1);  /* pop result */
	}
	return 0;
}

int DGScript::_globalRegister(lua_State *L) {
	if (!lua_isfunction(L, -1)) {
        DGLog::getInstance().error(DGModScript, DGMsg250011);
		
		return 0;
	}
	
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);  // pop and return a reference to the table.
	
    DGControl::getInstance().registerGlobalHandler((unsigned int)luaL_checknumber(L, 1), ref);
	
	return 0;
}

int DGScript::_globalRoom(lua_State *L) {
    // NOTE: This is a convenience Lua hack, which in theory is 100% safe.
    // Must do lots of testing. Basically, this helper creates a room instance,
    // loads the corresponding Lua file and makes sure it's never loaded twice.
    //
    // UPDATE: So far it has worked great. Our motto is: "if it works, it stays".
    // PS: Not really.
    
    char module[DGMaxObjectName];
    strncpy(module, luaL_checkstring(L, 1), DGMaxObjectName);
    
    // We first check if the object already exists
    lua_getglobal(L, module);
    if (!lua_isuserdata(L, -1)) {
        char line[DGMaxLogLength], script[DGMaxFileLength];
        
        // Parse line to automatically create the room
        snprintf(line, DGMaxLogLength, "%s = Room(\"%s\")", module, module);
        luaL_dostring(L, line);
        
        // Load the corresponding Lua file
        // TODO: Read rooms from path
        snprintf(script, DGMaxFileLength, "%s.lua", module);
        
        if (luaL_loadfile(L, DGConfig::getInstance().path(DGPathApp, script, DGObjectRoom)) == 0) {
            DGScript::getInstance().setModule(module);
            lua_pcall(L, 0, 0, 0);
            DGScript::getInstance().unsetModule();
        }
    }
    
    // Nothing else to do...
    
    return 0;
}

int DGScript::_globalSleep(lua_State *L) {
    DGControl::getInstance().sleep((int)luaL_checknumber(L, 1));
    
    return DGScript::getInstance().suspend();
}

int DGScript::_globalSetFont(lua_State *L) {
    switch ((int)luaL_checknumber(L, 1)) {
        case FEED:
            DGFeedManager::getInstance().setFont(luaL_checkstring(L, 2), luaL_checknumber(L, 3));
            break;
    }
    
    return 0;
}

int DGScript::_globalSnap(lua_State *L) {
    DGControl::getInstance().takeSnapshot();
	
	return 0;
}

int DGScript::_globalSwitch(lua_State *L) {
    switch (DGCheckProxy(L, 1)) {
        case DGObjectNode:
            DGControl::getInstance().switchTo(DGProxyToNode(L, 1), lua_toboolean(L, 2));
            break;
        case DGObjectRoom:
            DGControl::getInstance().switchTo(DGProxyToRoom(L, 1), lua_toboolean(L, 2));
            break;
        case DGObjectSlide:
            DGControl::getInstance().switchTo(DGProxyToSlide(L, 1), lua_toboolean(L, 2));
            break;
        case DGObjectGeneric:
            DGLog::getInstance().error(DGModScript, "%s", DGMsg250000);
            break;
            
        case DGObjectNone:
            DGLog::getInstance().error(DGModScript, "%s", DGMsg250001);
            break;
    }
    
	return 0;
}

int DGScript::_globalStartTimer(lua_State *L) {
	if (!lua_isfunction(L, -1)) {
		DGLog::getInstance().trace(DGModScript, "%s", DGMsg250006);
        
		return 0;
	}
	
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);  // Pop and return a reference to the table.
    bool shouldLoop = lua_toboolean(L, 2);
	int handle = DGTimerManager::getInstance().create(luaL_checknumber(L, 1), shouldLoop, ref);
    
    lua_pushnumber(L, handle);
	
	return 1;
}

int DGScript::_globalStopTimer(lua_State *L) {
    DGTimerManager::getInstance().disable(luaL_checknumber(L, 1));
	
	return 0;
}

int DGScript::_globalVersion(lua_State *L) {
    lua_pushstring(L, DGVersionString);
	
	return 1;
}

void DGScript::_registerEnums() {
    // Push all enum values
    DGLuaEnum(_L, AUDIO, DGObjectAudio);
    DGLuaEnum(_L, FUNCTION, DGActionFunction);
    DGLuaEnum(_L, IMAGE, DGObjectTexture);
    DGLuaEnum(_L, FEED, DGActionFeed);
    DGLuaEnum(_L, SWITCH, DGActionSwitch);
    DGLuaEnum(_L, VIDEO, DGObjectVideo);
    
    DGLuaEnum(_L, NORMAL, DGCursorNormal);
    DGLuaEnum(_L, DRAGGING, DGCursorDrag);
    DGLuaEnum(_L, LEFT, DGCursorLeft);
    DGLuaEnum(_L, RIGHT, DGCursorRight);
    DGLuaEnum(_L, UP, DGCursorUp);
    DGLuaEnum(_L, DOWN, DGCursorDown);
    DGLuaEnum(_L, UP_LEFT, DGCursorUpLeft);
    DGLuaEnum(_L, UP_RIGHT, DGCursorUpRight);
    DGLuaEnum(_L, DOWN_LEFT, DGCursorDownLeft);
    DGLuaEnum(_L, DOWN_RIGHT, DGCursorDownRight);
    DGLuaEnum(_L, FORWARD, DGCursorForward);
    DGLuaEnum(_L, BACKWARD, DGCursorBackward);
    DGLuaEnum(_L, USE, DGCursorUse);
    DGLuaEnum(_L, LOOK, DGCursorLook);
    DGLuaEnum(_L, TALK, DGCursorTalk);
    DGLuaEnum(_L, CUSTOM, DGCursorCustom);
    
    DGLuaEnum(_L, SLIDE, DGNorth);
    DGLuaEnum(_L, NORTH, DGNorth);
    DGLuaEnum(_L, EAST, DGEast);
    DGLuaEnum(_L, WEST, DGWest);
    DGLuaEnum(_L, SOUTH, DGSouth);
    DGLuaEnum(_L, UP, DGUp);
    DGLuaEnum(_L, DOWN, DGDown);
    DGLuaEnum(_L, NORTHEAST, DGNorthEast);
    DGLuaEnum(_L, SOUTHEAST, DGSouthEast);
    DGLuaEnum(_L, NORTHWEST, DGNorthWest);
    DGLuaEnum(_L, SOUTHWEST, DGSouthWest);
    DGLuaEnum(_L, CURRENT, DGCurrent);
    
    DGLuaEnum(_L, DRAG, DGMouseDrag);
    DGLuaEnum(_L, FIXED, DGMouseFixed);
    DGLuaEnum(_L, FREE, DGMouseFree);
    
    DGLuaEnum(_L, ENTER_NODE, DGEventEnterNode);
    DGLuaEnum(_L, ENTER_ROOM, DGEventEnterRoom);
    DGLuaEnum(_L, LEAVE_NODE, DGEventLeaveNode);
    DGLuaEnum(_L, LEAVE_ROOM, DGEventLeaveRoom);
    DGLuaEnum(_L, PRE_RENDER, DGEventPreRender);
    DGLuaEnum(_L, POST_RENDER, DGEventPostRender);
    DGLuaEnum(_L, MOUSE_BUTTON, DGEventMouseButton);
    DGLuaEnum(_L, MOUSE_RIGHT_BUTTON, DGEventMouseRightButton);
    DGLuaEnum(_L, MOUSE_MOVE, DGEventMouseMove);
    DGLuaEnum(_L, RESIZE, DGEventResize);

	DGLuaEnum(_L, BLACK, DGColorBlack);
	DGLuaEnum(_L, BLUE, DGColorBlue);
	DGLuaEnum(_L, GREEN, DGColorGreen);
	DGLuaEnum(_L, CYAN, DGColorCyan);
	DGLuaEnum(_L, RED, DGColorRed);
	DGLuaEnum(_L, MAGENTA, DGColorMagenta);
	DGLuaEnum(_L, BROWN, DGColorBrown);
	DGLuaEnum(_L, LIGHTGRAY, DGColorLightGray);
	DGLuaEnum(_L, DARKGRAY, DGColorDarkGray);
	DGLuaEnum(_L, BRIGHTBLUE, DGColorBrightBlue);
	DGLuaEnum(_L, BRIGHTGREEN, DGColorBrightGreen);
	DGLuaEnum(_L, BRIGHTCYAN, DGColorBrightCyan);
	DGLuaEnum(_L, BRIGHTRED, DGColorBrightRed);
	DGLuaEnum(_L, BRIGHTMAGENTA, DGColorBrightMagenta);
	DGLuaEnum(_L, YELLOW, DGColorYellow);
	DGLuaEnum(_L, WHITE, DGColorWhite);
    
    DGLuaEnum(_L, F1, DGKeyF1);
	DGLuaEnum(_L, F2, DGKeyF2);
	DGLuaEnum(_L, F3, DGKeyF3);
	DGLuaEnum(_L, F4, DGKeyF4);
    DGLuaEnum(_L, F5, DGKeyF5);
	DGLuaEnum(_L, F6, DGKeyF6);
	DGLuaEnum(_L, F7, DGKeyF7);
	DGLuaEnum(_L, F8, DGKeyF8);
	DGLuaEnum(_L, F9, DGKeyF9);
	DGLuaEnum(_L, F10, DGKeyF10);
	DGLuaEnum(_L, F11, DGKeyF11);
	DGLuaEnum(_L, F12, DGKeyF12);

    DGLuaEnum(_L, SLOW, DGFadeSlow);
    DGLuaEnum(_L, SLOWEST, DGFadeSlowest);
    DGLuaEnum(_L, FAST, DGFadeFast);
    DGLuaEnum(_L, FASTEST, DGFadeFastest);
}

void DGScript::_registerGlobals() {
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
