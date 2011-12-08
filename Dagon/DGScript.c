/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#include "DGAudio.h"
#include "DGCamera.h"
#include "DGConsole.h"
#include "DGControl.h"
#include "DGConfig.h"
#include "DGLog.h"
#include "DGMemory.h"
#include "DGScript.h"
#include "DGState.h"
#include "DGSystem.h"
#include "DGTimer.h"
#include "DGVideo.h"
#include "DGWorld.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


static lua_State *L;
static lua_State *thread;

enum Node_Directions {
	N, W, S, E, NW, SW, SE, NE
};

#define	AUDIO		0
#define CUSTOM		1
#define	IMAGE		2
#define FEEDBACK	3
#define SWITCH		4
#define	VIDEO		5

#define AUDIO_OBJ	"Audio"
#define ROOM		"Room"
#define NODE		"Node"
#define SPOT		"Spot"

#define lua_boxpointer(L,u) (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))
#define lua_unboxpointer(L,i) (*(void **)(lua_touserdata(L, i)))
#define newtable(L) (lua_newtable(L), lua_gettop(L))

// FIXME: temporary test
static int room_idx = 0;
static char rooms[80][256];

int luaopen_audio(lua_State *L);
int luaopen_node(lua_State *L);
int luaopen_room(lua_State *L);
int luaopen_spot(lua_State *L);
int luaopen_global(lua_State *L);
int luaopen_camera(lua_State *L);
int luaopen_config(lua_State *L);
int luaopen_system(lua_State *L);
int luaopen_timer(lua_State *L);

// TODO: Also add the posibility to reference objects per id and name

static DGObject* checkobject(lua_State *L, unsigned type, int index) {
	if (lua_isuserdata(L, index)) {
        DGObject* obj = (DGObject*)lua_unboxpointer(L, index);
        lua_getmetatable(L, index);
        
        if (!lua_equal(L, lua_upvalueindex(1), -1)) {
            if (!DGObjectIsType(obj, type))
                return NULL;
        }
        
        lua_pop(L, 1);
        
        return obj;
    }
    
    DGLogError(DG_MOD_SCRIPT, "Object does not exist");
    lua_error(L);
    
    return NULL;
}

static DGObject* pushobject(lua_State *L, DGObject* obj) {
	DGObjectAddReference(obj);
	
	lua_boxpointer(L, obj);
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
	
	return obj;
}

static int protectobject(lua_State *L) {
	DGLogError(DG_MOD_SCRIPT, "Operation not permitted");
	
	return 0;
}

static int object_desc(lua_State *L) {
	lua_pushstring(L, "object");
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
///// Audio Object -- BEGIN												   /////
////////////////////////////////////////////////////////////////////////////////

static int audio_new(lua_State *L) {
	DGObject* audio;
	DGBool loop;
	
	if (lua_isboolean(L, 2))
		loop = lua_toboolean(L, 2);
	else
		loop = DG_NO;
	
	audio = DGAudioNew(luaL_checkstring(L, 1), loop);
	pushobject(L, audio);
	
	DGAudioLoad(audio);
	
	return 1;
}

static int audio_destroy(lua_State *L) {
	// WARNING: Very, very careful with this! Could break the arrays...
	
	DGObject* obj = (DGObject*)lua_unboxpointer(L, 1);
	//if (obj) dg_audio_release(obj);
	if (obj) DGAudioUnload(obj);
	
	return 0;
}

static int audio_fade_in(lua_State *L) {
	DGObject* audio = checkobject(L, DG_OBJECT_AUDIO, 1);
	
	DGAudioFadeIn(audio, (int)lua_tonumber(L, 2));
	
	return 0;
}

static int audio_fade_out(lua_State *L) {
	DGObject* audio = checkobject(L, DG_OBJECT_AUDIO, 1);
	
	DGAudioFadeOut(audio, (int)lua_tonumber(L, 2));
	
	return 0;
}

static int audio_is_playing(lua_State *L) {
	DGObject* audio = checkobject(L, DG_OBJECT_AUDIO, 1);
	DGBool playing;
	
	playing = DGAudioIsPlaying(audio);
	
	lua_pushboolean(L, playing);
	lua_pushvalue(L, -1);
	
	return 1;
}

static int audio_play(lua_State *L) {
	DGObject* audio = checkobject(L, DG_OBJECT_AUDIO, 1);
	
	DGAudioPlay(audio);
	
	return 0;
}

static int audio_pause(lua_State *L) {
	DGObject* audio = checkobject(L, DG_OBJECT_AUDIO, 1);
	
	DGAudioPause(audio);
	
	return 0;
}

static int audio_stop(lua_State *L) {
	DGObject* audio = checkobject(L, DG_OBJECT_AUDIO, 1);
	
	DGAudioStop(audio);
	
	return 0;
}

static int audio_volume(lua_State *L) {
	DGObject* audio = checkobject(L, DG_OBJECT_AUDIO, 1);
	
	DGAudioSetVolume(audio, (float)lua_tonumber(L, 2));
	
	return 0;
}

// Allow manual release of audios?

static const struct luaL_reg audiolib_f [] = {
	{"__gc", audio_destroy},
	{NULL, NULL}
};

static const struct luaL_reg audiolib_m [] = {
	{"new", audio_new},
	{"fadein", audio_fade_in},
	{"fadeout", audio_fade_out},
	{"isplaying", audio_is_playing},
	{"play", audio_play},
	{"pause", audio_pause},
	{"stop", audio_stop},
	{"volume", audio_volume},
	{NULL, NULL}
};

int luaopen_audio(lua_State *L) {
	int metatable, methods;
	
	lua_pushliteral(L, AUDIO_OBJ);         /* name of Spot table */
	methods   = newtable(L);           /* Node methods table */
	metatable = newtable(L);           /* Node metatable */
	lua_pushliteral(L, "__index");     /* add index event to metatable */
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);        /* metatable.__index = methods */
	lua_pushliteral(L, "__metatable"); /* hide metatable */
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);        /* metatable.__metatable = methods */
	luaL_openlib(L, 0, audiolib_f,  0); /* fill metatable */
	luaL_openlib(L, 0, audiolib_m, 1); /* fill Spot methods table */
	lua_settable(L, LUA_GLOBALSINDEX); /* add Spot to globals */
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
///// Audio Object -- END											       /////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///// Spot Object -- BEGIN				    							   /////
////////////////////////////////////////////////////////////////////////////////

static int spot_new(lua_State *L) {
	DGObject* spot;
	int i, size, face;
	short* coords;
	
	if (!lua_istable(L, 2)) {
		luaL_error(L, "Table with coordinates expected to create spot");
		
		return 0;
	}
	
	size = lua_objlen(L, 2);
	
	coords = (short*)DGAlloc(size * sizeof(short));
	lua_pushnil(L);  /* first key */
	i = 0;
	while (lua_next(L, 2) != 0) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		coords[i] = (short)lua_tonumber(L, -1);
		lua_pop(L, 1);
		i++;
	}
	
	// Convert from cam coordinates to array
	face = (int)luaL_checknumber(L, 1);
	
	switch (face) {
		case DG_CAM_FRONT: face = 0; break;
		case DG_CAM_RIGHT: face = 1; break;
		case DG_CAM_BACK: face = 2; break;	
		case DG_CAM_LEFT: face = 3; break;	
		case DG_CAM_TOP: face = 4; break;			
		case DG_CAM_BOTTOM: face = 5; break;
		default:
			DGLogError(DG_MOD_SCRIPT, "Invalid face specified");
			return 0;
			break;
	}
	
	spot = DGSpotNew(coords, size, face, 0);
	pushobject(L, spot);
	
	DGFree(coords);
	
	return 1;
}

static int spot_destroy(lua_State *L) {
	// WARNING: Very, very careful with this! Could break the arrays...
    // If the spot has been added to a node, it's reference count should be +1
    // It can be removed
	
	//DG_OBJECT* obj = (DG_OBJECT*)lua_unboxpointer(L, 1);
	//if (obj) dg_spot_release(obj);
	
	return 0;
}

static int spot_current(lua_State *L) {
	DGObject* spot = DGWorldCurrentSpot();
	pushobject(L, spot);
	
	return 1;
}

static int spot_desc(lua_State *L) {
	char aux[DG_MAX_LOGLEN];
	DGObject* self = lua_unboxpointer(L, 1);
	
	if (strcmp(self->name, "") == 0)
		sprintf(aux, "spot");
	else
		sprintf(aux, "%s (spot)", self->name);
	
	lua_pushstring(L, aux);
	
	return 1;
}


static int spot_attach(lua_State *L) {
	DGAction action;
	DGObject* spot = checkobject(L, DG_OBJECT_SPOT, 1);
    DGObject* audio;
	DGObject* video;
	
	DGBool autoplay = DG_NO;
	DGBool loop = DG_NO;
	DGBool sync = DG_NO;
    float volume = 1.0f;
	
	int ref, type = (int)luaL_checknumber(L, 2);
	
	switch (type) {
		case AUDIO:
            // TODO: Autoplay should NOT be handled in video but affect spot in general			
			if (lua_istable(L, 4)) {
				lua_pushnil(L);
				while (lua_next(L, 4) != 0) {
					const char* key = lua_tostring(L, -2);
					
                    if (strcmp(key, "loop") == 0) loop = lua_toboolean(L, -1);
					else if (strcmp(key, "volume") == 0) volume = lua_tonumber(L, -1);
					
					lua_pop(L, 1);
				}
			}
			else {
				loop = lua_toboolean(L, 4);
				//volume = lua_tonumber(L, 5); Should be max if this is nil
			}
            
            audio = DGAudioNew(luaL_checkstring(L, 3), loop);
            DGAudioSetVolume(audio, volume);
			DGSpotSetAudio(spot, audio);
            
			break;
		case CUSTOM:
			if (!lua_isfunction(L, -1)) {
				DGLogError(DG_MOD_SCRIPT, "Function expected as second parameter in attach()");
				
				return 0;
			}
			
			ref = luaL_ref(L, LUA_REGISTRYINDEX);  // pop and return a reference to the table.
			
			action.type = DG_ACTION_CUSTOM;
			action.custom_handler = ref;
			
			DGSpotSetAction(checkobject(L, DG_OBJECT_SPOT, 1), &action);
			if (!DGSpotHasColor(spot))
				DGSpotSetColor(spot, 0);
			
			break;
		case IMAGE:
			DGSpotSetImage(spot, luaL_checkstring(L, 3));
			break;
		case FEEDBACK:
			action.type = DG_ACTION_FEEDBACK;
			
			strcpy(action.feedback, luaL_checkstring(L, 3));
			
			if (lua_isstring(L, 4))
				strcpy(action.feedback_audio, lua_tostring(L, 4));
			
			DGSpotSetAction(checkobject(L, DG_OBJECT_SPOT, 1), &action);
			if (!DGSpotHasColor(spot))
				DGSpotSetColor(spot, 0);
			
			break;
		case SWITCH:
			action.type = DG_ACTION_SWITCH;
			action.target = (DGObject*)lua_unboxpointer(L, 3);
			
			DGSpotSetAction(checkobject(L, DG_OBJECT_SPOT, 1), &action);
			if (!DGSpotHasColor(spot))
				DGSpotSetColor(spot, 0);
			
			break;
		case VIDEO:
			// TODO: Autoplay should NOT be handled in video but affect spot in general			
			if (lua_istable(L, 4)) {
				lua_pushnil(L);
				while (lua_next(L, 4) != 0) {
					const char* key = lua_tostring(L, -2);
					
					if (strcmp(key, "autoplay") == 0) autoplay = lua_toboolean(L, -1);
					else if (strcmp(key, "loop") == 0) loop = lua_toboolean(L, -1);
					else if (strcmp(key, "sync") == 0) sync = lua_toboolean(L, -1);
					
					lua_pop(L, 1);
				}
			}
			else {
				autoplay = lua_toboolean(L, 4);
				loop = lua_toboolean(L, 5);
				sync = lua_toboolean(L, 6);
			}

			video = DGVideoNew(luaL_checkstring(L, 3), autoplay, loop, sync);
			DGSpotSetVideo(spot, video);
			
			if (autoplay)
				DGSpotPlay(spot);
			
			break;
		default:
			DGLogError(DG_MOD_SCRIPT, "Invalid type in attach() function");
			break;
	}
	
	return 0;
}

static int spot_enable(lua_State *L) {
	DGSpotEnable(checkobject(L, DG_OBJECT_SPOT, 1));
	
	return 0;
}

static int spot_disable(lua_State *L) {
	DGSpotDisable(checkobject(L, DG_OBJECT_SPOT, 1));
	
	return 0;
}

static int spot_is_enabled(lua_State *L) {
	DGBool ret = DGSpotIsEnabled(checkobject(L, DG_OBJECT_SPOT, 1));
	
	lua_pushboolean(L, ret);
	
	return 1;
}

static int spot_play(lua_State *L) {
	DGObject* spot = checkobject(L, DG_OBJECT_SPOT, 1);
	//DG_OBJECT* video = dg_spot_video(spot);
	
	DGSpotPlay(spot);
    
    if (DGSpotHasAudio(spot)) {
        DGObject* audio = DGSpotAudio(spot);
        
        if (!DGAudioIsPlaying(audio)) {
            DGAudioPlay(audio);
        }
    }
    
    // This can't happen here, should check if we're in the node first
	/*if (dg_video_is_synced(video))
		return lua_yield(thread, 1);*/
	
	return 0;
}

static int spot_stop(lua_State *L) {
    
    DGObject* spot = checkobject(L, DG_OBJECT_SPOT, 1);
	DGSpotStop(spot);
    
    // Hack: too many tasks performed here
    if (DGSpotHasVideo(spot)) {
        DGObject* video = DGSpotVideo(spot);
        DGVideoStop(video);
    }
	
	return 0;
}

static int spot_is_playing(lua_State *L) {
	DGBool ret = DGSpotIsPlaying(checkobject(L, DG_OBJECT_SPOT, 1));
	
	lua_pushboolean(L, ret);
	
	return 1;
}

static int spot_toggle(lua_State *L) {
	DGSpotToggle(checkobject(L, DG_OBJECT_SPOT, 1));
	
	return 0;
}

static const struct luaL_reg spotlib_f [] = {
	{"__gc", spot_destroy},
	{"__tostring", spot_desc},
	{NULL, NULL}
};

static const struct luaL_reg spotlib_m [] = {
	{"new", spot_new},
	{"current", spot_current},
	{"attach", spot_attach},
	{"enable", spot_enable},
	{"disable", spot_disable},
	{"isenabled", spot_is_enabled},
	{"toggle", spot_toggle},
	{"play", spot_play},
	{"stop", spot_stop},
	{"isplaying", spot_is_playing},	
	{NULL, NULL}
};

int luaopen_spot(lua_State *L) {
	int metatable, methods;
	
	lua_pushliteral(L, SPOT);         /* name of Spot table */
	methods   = newtable(L);           /* Node methods table */
	metatable = newtable(L);           /* Node metatable */
	lua_pushliteral(L, "__index");     /* add index event to metatable */
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);        /* metatable.__index = methods */
	lua_pushliteral(L, "__metatable"); /* hide metatable */
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);        /* metatable.__metatable = methods */
	luaL_openlib(L, 0, spotlib_f,  0); /* fill metatable */
	luaL_openlib(L, 0, spotlib_m, 1); /* fill Spot methods table */
	lua_settable(L, LUA_GLOBALSINDEX); /* add Spot to globals */
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
///// Spot Object -- END											       /////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///// Node Object -- BEGIN				    							   /////
////////////////////////////////////////////////////////////////////////////////

static int node_new(lua_State *L) {
	DGObject* node = DGNodeNew(luaL_checkstring(L, 1));
	pushobject(L, node);
	
	// Temporary
	DGWorldAdd(node);
	
	return 1;
}

static int node_destroy(lua_State *L) {
	// WARNING: Very, very careful with this! Could break the arrays...
	
	DGObject* obj = (DGObject*)lua_unboxpointer(L, 1);
	if (obj) DGNodeRelease(obj);
	
	return 0;
}

static int node_current(lua_State *L) {
	DGObject* node = DGWorldCurrentNode();
	pushobject(L, node);
	
	return 1;
}

static int node_desc(lua_State *L) {
	char aux[DG_MAX_LOGLEN];
	DGObject* self = lua_unboxpointer(L, 1);
	
	if (strcmp(self->name, "") == 0)
		sprintf(aux, "node");
	else
		sprintf(aux, "%s (node)", self->name);
	
	lua_pushstring(L, aux);
	
	return 1;
}

static int node_equal(lua_State *L) {
	DGObject* node1 = lua_unboxpointer(L, 1);
    DGObject* node2 = lua_unboxpointer(L, 2);
	
	if (strcmp(node1->name, node2->name) == 0)
        return 1;
	
	return 0;
}

static int node_add_spot(lua_State *L) {
	DGObject* target;
	DGObject* self = checkobject(L, DG_OBJECT_NODE, 1);
	
	if (lua_isnumber(L, 2)) { // Using the convenience syntax
		// Doesn't work yet!!
		lua_remove(L, 1); // Remove the first element so that the admitted values are shifted
		spot_new(L);
	}

	target = (DGObject*)lua_unboxpointer(L, -1);

	DGNodeAddSpot(self, target);
	
	return 1;
}

static int node_link(lua_State *L) {
	int size;
	DGObject* self = checkobject(L, DG_OBJECT_NODE, 1);
	
	if (!lua_istable(L, 2)) {
		luaL_error(L, "Table with coordinates expected to link node");
		
		return 0;
	}
	
	size = lua_objlen(L, 2);
	
	lua_pushnil(L);
	while (lua_next(L, 2) != 0) {
		const char* key = lua_tostring(L, -2);
		if (lua_isfunction(L, -1)) {
			int ref = luaL_ref(L, LUA_REGISTRYINDEX);
			
			// Ugly nesting of strcmps() but there's probably no better way of doing this...
			if (strcmp(key, "N") == 0) DGNodeAddCustomLink(self, DG_NORTH, ref);
			else if (strcmp(key, "W") == 0) DGNodeAddCustomLink(self, DG_WEST, ref);
			else if (strcmp(key, "S") == 0) DGNodeAddCustomLink(self, DG_SOUTH, ref);
			else if (strcmp(key, "E") == 0) DGNodeAddCustomLink(self, DG_EAST, ref);
			else if (strcmp(key, "NW") == 0) DGNodeAddCustomLink(self, DG_NORTHWEST, ref);
			else if (strcmp(key, "SW") == 0) DGNodeAddCustomLink(self, DG_SOUTHWEST, ref);
			else if (strcmp(key, "SE") == 0) DGNodeAddCustomLink(self, DG_SOUTHEAST, ref);
			else if (strcmp(key, "NE") == 0) DGNodeAddCustomLink(self, DG_NORTHEAST, ref);
			
			// Runtime error if we pop the 1st value here. Why?
		}
		else {
			DGObject* target = (DGObject*)lua_unboxpointer(L, -1);
			
			// Ugly nesting of strcmps() but there's probably no better way of doing this...
			if (strcmp(key, "N") == 0) DGNodeAddLink(self, DG_NORTH, target);
			else if (strcmp(key, "W") == 0) DGNodeAddLink(self, DG_WEST, target);
			else if (strcmp(key, "S") == 0) DGNodeAddLink(self, DG_SOUTH, target);
			else if (strcmp(key, "E") == 0) DGNodeAddLink(self, DG_EAST, target);
			else if (strcmp(key, "NW") == 0) DGNodeAddLink(self, DG_NORTHWEST, target);
			else if (strcmp(key, "SW") == 0) DGNodeAddLink(self, DG_SOUTHWEST, target);
			else if (strcmp(key, "SE") == 0) DGNodeAddLink(self, DG_SOUTHEAST, target);
			else if (strcmp(key, "NE") == 0) DGNodeAddLink(self, DG_NORTHEAST, target);
			
			lua_pop(L, 1);
		}
	}
	
	return 0;
}

static int node_switch(lua_State *L) {
    if (!checkobject(L, DG_OBJECT_NODE, 1)) {
        if (!checkobject(L, DG_OBJECT_ROOM, 1)) {
            DGLogError(DG_MOD_SCRIPT, "Object is not a room or node");
            
            return 0;
        }
        
         DGControlSwitchTo(checkobject(L, DG_OBJECT_ROOM, 1));
        
        return 0;
    }
            
    DGControlSwitchTo(checkobject(L, DG_OBJECT_NODE, 1));
	
	return 0;
}

static const luaL_reg nodelib_f[] = {
	{"__gc", node_destroy},
	{"__eq", node_equal},    
	{"__tostring", node_desc},
	{NULL, NULL}
};

static const luaL_reg nodelib_m[] = {
	{"new", node_new},
	{"current", node_current},
	{"addspot", node_add_spot},
	{"link", node_link},
	{"switch", node_switch},
	{NULL, NULL}
};

int luaopen_node(lua_State *L) {
	int metatable, methods;
	
	lua_pushliteral(L, NODE);         /* name of Node table */
	methods   = newtable(L);           /* Node methods table */
	metatable = newtable(L);           /* Node metatable */
	lua_pushliteral(L, "__index");     /* add index event to metatable */
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);        /* metatable.__index = methods */
	lua_pushliteral(L, "__metatable"); /* hide metatable */
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);        /* metatable.__metatable = methods */
	luaL_openlib(L, 0, nodelib_f,  0); /* fill metatable */
	luaL_openlib(L, 0, nodelib_m, 1); /* fill Node methods table */
	lua_settable(L, LUA_GLOBALSINDEX); /* add Node to globals */
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
///// Node Object -- END											       /////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///// Room Object -- BEGIN				    							   /////
////////////////////////////////////////////////////////////////////////////////

static int room_new(lua_State *L) {
	DGObject* room = DGRoomNew(luaL_checkstring(L, 1));
	pushobject(L, room);
	DGWorldAdd(room);
	
	return 1;
}

static int room_destroy(lua_State *L) {
	// WARNING: Very, very careful with this! Could break the arrays...
	
	DGObject* obj = (DGObject*)lua_unboxpointer(L, 1);
	if (obj) DGRoomRelease(obj);
	
	return 0;
}

static int room_current(lua_State *L) {
	DGObject* room = DGWorldCurrentRoom();
	pushobject(L, room);
	
	return 1;
}

static int room_desc(lua_State *L) {
	char aux[DG_MAX_LOGLEN];
	DGObject* self = lua_unboxpointer(L, 1);
	
	if (strcmp(self->name, "") == 0)
		sprintf(aux, "room");
	else
		sprintf(aux, "%s (room)", self->name);
	
	lua_pushstring(L, aux);
	
	return 1;
}


static int room_add_node(lua_State *L) {
	DGObject* self = checkobject(L, DG_OBJECT_ROOM, 1);
	DGObject* target = (DGObject*)lua_unboxpointer(L, -1);
	
	DGRoomAdd(self, target);
	
	return 1;
}

static const struct luaL_reg roomlib_f [] = {
	{"__gc", room_destroy},
	{"__tostring", room_desc},
	{NULL, NULL}
};

static const struct luaL_reg roomlib_m [] = {
	{"new", room_new},
	{"current", room_current},
	{"addnode", room_add_node},
	{NULL, NULL}
};

int luaopen_room(lua_State *L) {
	int metatable, methods;
	
	lua_pushliteral(L, ROOM);         /* name of Room table */
	methods   = newtable(L);           /* Node methods table */
	metatable = newtable(L);           /* Node metatable */
	lua_pushliteral(L, "__index");     /* add index event to metatable */
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);        /* metatable.__index = methods */
	lua_pushliteral(L, "__metatable"); /* hide metatable */
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);        /* metatable.__metatable = methods */
	luaL_openlib(L, 0, roomlib_f,  0); /* fill metatable */
	luaL_openlib(L, 0, roomlib_m, 1); /* fill Room methods table */
	lua_settable(L, LUA_GLOBALSINDEX); /* add Room to globals */
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
///// Room Object -- END											       /////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///// Camera Library -- BEGIN				    						   /////
////////////////////////////////////////////////////////////////////////////////

static int camera_set(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "breathe") == 0)
		DGCameraEnableBreathing(lua_toboolean(L, 3));
	
	if (strcmp(key, "fov") == 0)
		DGCameraSetFieldOfView((float)luaL_checknumber(L, 3));
	
	if (strcmp(key, "neutral_zone") == 0)
		DGCameraSetNeutralZone((int)luaL_checknumber(L, 3));
	
	if (strcmp(key, "pan_speed") == 0)
		DGCameraSetPanningSpeed((int)luaL_checknumber(L, 3));
	
	if (strcmp(key, "vertical_limit") == 0)
		DGCameraSetVerticalLimit((int)luaL_checknumber(L, 3));
	
	if (strcmp(key, "walk") == 0)
		DGCameraEnableWalking(lua_toboolean(L, 3));
	
	return 0;
}

static int camera_get(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	// From http://lua-users.org/wiki/ObjectProperties
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, key);
	
   	// Either key is name of a method in the metatable
	if(!lua_isnil(L, -1))
		return 1;
	
	// ... or its a field access, so recall as self.get(self, value).
	lua_settop(L, 2);
	
	if (strcmp(key, "breathe") == 0) {
		lua_pushboolean(L, DGCameraIsBreathingEnabled());
		return 1;
	}
	
	if (strcmp(key, "fov") == 0) {
		lua_pushnumber(L, DGCameraFieldOfView());
		return 1;
	}
	
	if (strcmp(key, "neutral_zone") == 0) {
		lua_pushnumber(L, DGCameraNeutralZone());
		return 1;
	}
	
	if (strcmp(key, "pan_speed") == 0) {
		lua_pushnumber(L, DGCameraPanningSpeed());
		return 1;
	}
	
	if (strcmp(key, "vertical_limit") == 0) {
		lua_pushnumber(L, DGCameraVerticalLimit());
		return 1;
	}
	
	if (strcmp(key, "walk") == 0) {
		lua_pushboolean(L, DGCameraIsWalkingEnabled());
		return 1;
	}
	
	return 0;
}

static int camera_lookat(lua_State *L) {
	int angle_h = (int)luaL_checknumber(L, 1);
	DGBool instant = lua_toboolean(L, 3);
	
	if (lua_isnumber(L, 2))
		DGCameraLookAt(angle_h, (int)lua_tonumber(L, 2), instant);
	else
		DGCameraLookAt(angle_h, DG_CAM_CURRENT, instant);
	
	if (instant)
		return 0;
	else
		return lua_yield(L, 0);
}

static int camera_get_angle(lua_State *L) {
	int angleh = DGCameraHorizontalAngle();
	int anglev = DGCameraVerticalAngle();
	
	lua_pushnumber(L, angleh);
	lua_pushnumber(L, anglev);
	
	return 2;
}

static int camera_get_origin(lua_State *L) {
	DGPoint point = DGCameraOrigin();
	
	lua_pushnumber(L, point.x);
	lua_pushnumber(L, point.y);
	
	return 2;
}

static int camera_set_origin(lua_State *L) {
	DGCameraSetOrigin((int)luaL_checknumber(L, 1), (int)luaL_checknumber(L, 2));
	
	return 0;
}

static int camera_get_viewport(lua_State *L) {
	DGSize size = DGCameraViewport();
	
	lua_pushnumber(L, size.width);
	lua_pushnumber(L, size.height);
	
	return 2;
}

static int camera_set_ortho(lua_State *L) {
	DGCameraSetOrthoView();
	
	return 0;
}

static int camera_set_perspective(lua_State *L) {
	DGCameraSetPerspectiveView();
	
	return 0;
}

static int camera_set_viewport(lua_State *L) {
	DGCameraSetViewport((int)luaL_checknumber(L, 1), (int)luaL_checknumber(L, 2));
	
	return 0;
}

int luaopen_camera(lua_State *L) {
    int ref;
    
	const struct luaL_Reg metacamera[] =
	{
		{"lookat", camera_lookat},
		{"getangle", camera_get_angle},
		{"getorigin", camera_get_origin},
		{"setorigin", camera_set_origin},
		{"setortho", camera_set_ortho},
		{"setperspective", camera_set_perspective},
		{"getviewport", camera_get_viewport},
		{"setviewport", camera_get_viewport},	
		{"__index", camera_get},
		{"__newindex", camera_set},	
		{NULL, NULL}
	};
	
	lua_newuserdata(L, sizeof(void *));
    
    lua_pushvalue(L, -1);
    ref = lua_ref(L,LUA_REGISTRYINDEX);
	
	luaL_newmetatable(L, "metacamera");
	luaL_register(L, NULL, metacamera);
	lua_setmetatable(L, -2);
	
	lua_newtable(L);
    lua_setfenv(L, -2);
	
	lua_setglobal(L, "camera");
	
	return ref;
}

////////////////////////////////////////////////////////////////////////////////
///// Camera Library -- END											       /////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///// Config Library -- BEGIN				    						   /////
////////////////////////////////////////////////////////////////////////////////

static int l_set(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0)
		DGConfig.antialiasing = lua_toboolean(L, 3);
	
	if (strcmp(key, "display_width") == 0)
		DGConfig.display_width = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "display_height") == 0)
		DGConfig.display_height = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "display_depth") == 0)
		DGConfig.display_depth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "debug_mode") == 0)
		DGConfig.debug_mode = lua_toboolean(L, 3);

    if (strcmp(key, "dust") == 0)
		DGConfig.dust = lua_toboolean(L, 3);
	
	if (strcmp(key, "effects") == 0)
		DGConfig.effects = lua_toboolean(L, 3);
	
	if (strcmp(key, "framerate") == 0)
		DGConfig.framerate = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "fullscreen") == 0)
		DGConfig.fullscreen = lua_toboolean(L, 3);
	
	if (strcmp(key, "log") == 0)
		DGConfig.log = lua_toboolean(L, 3);
	
	if (strcmp(key, "multithread") == 0)
		DGConfig.multithread = lua_toboolean(L, 3);
	
	if (strcmp(key, "path") == 0) {
        if (DGConfig.debug_mode)
            strcpy(DGConfig.res_path, luaL_checkstring(L, 3));
        else
            // Protect the current path
            strcat(DGConfig.res_path, luaL_checkstring(L, 3));
    }
    
	if (strcmp(key, "scare") == 0)
		DGConfig.scare = lua_toboolean(L, 3);
   
    if (strcmp(key, "script") == 0)
		strcpy(DGConfig.script, luaL_checkstring(L, 3));
	
	if (strcmp(key, "show_splash") == 0)
		DGConfig.show_splash = lua_toboolean(L, 3);
	
	if (strcmp(key, "show_spots") == 0)
		DGConfig.show_spots = lua_toboolean(L, 3);
	
	if (strcmp(key, "tex_compression") == 0)
		DGConfig.tex_compression = lua_toboolean(L, 3);
	
	if (strcmp(key, "tex_extension") == 0)
		// Careful with the length
		strcpy(DGConfig.tex_extension, luaL_checkstring(L, 3));
    
	if (strcmp(key, "throb") == 0)
		DGConfig.throb = lua_toboolean(L, 3);    
	
	if (strcmp(key, "vertical_sync") == 0)
		DGConfig.vertical_sync = lua_toboolean(L, 3);	
	
	return 0;
}

static int l_get(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0) {
		lua_pushboolean(L, DGConfig.antialiasing);
		return 1;
	}
	
	if (strcmp(key, "display_width") == 0) {
		lua_pushnumber(L, DGConfig.display_width);
		return 1;
	}
	
	if (strcmp(key, "display_height") == 0) {
		lua_pushnumber(L, DGConfig.display_height);
		return 1;
	}
	
	if (strcmp(key, "display_depth") == 0) {
		lua_pushnumber(L, DGConfig.display_depth);
		return 1;
	}
	
	if (strcmp(key, "debug_mode") == 0) {
		lua_pushboolean(L, DGConfig.debug_mode);
		return 1;
	}
    
    if (strcmp(key, "dust") == 0) {
		lua_pushboolean(L, DGConfig.dust);
		return 1;
	}
	
	if (strcmp(key, "effects") == 0) {
		lua_pushboolean(L, DGConfig.effects);
		return 1;
	}
	
	if (strcmp(key, "framerate") == 0) {
		lua_pushnumber(L, DGConfig.framerate);
		return 1;
	}
	
	if (strcmp(key, "fullscreen") == 0) {
		lua_pushboolean(L, DGConfig.fullscreen);
		return 1;
	}
	
	if (strcmp(key, "log") == 0) {
		lua_pushboolean(L, DGConfig.log);
		return 1;
	}
	
	if (strcmp(key, "multithread") == 0) {
		lua_pushboolean(L, DGConfig.multithread);
		return 1;
	}
	
	if (strcmp(key, "path") == 0) {
		lua_pushstring(L, DGConfig.user_path);
		return 1;
	}
	
    if (strcmp(key, "scare") == 0) {
		lua_pushboolean(L, DGConfig.scare);
		return 1;
	}    
    
	if (strcmp(key, "script") == 0) {
		lua_pushstring(L, DGConfig.script);
		return 1;
	}
	
	if (strcmp(key, "show_splash") == 0) {
		lua_pushboolean(L, DGConfig.show_splash);
		return 1;
	}
	
	if (strcmp(key, "show_spots") == 0) {
		lua_pushboolean(L, DGConfig.show_spots);
		return 1;
	}
	
	if (strcmp(key, "tex_compression") == 0) {
		lua_pushboolean(L, DGConfig.tex_compression);
		return 1;
	}

	if (strcmp(key, "tex_extension") == 0) {
		lua_pushstring(L, DGConfig.tex_extension);
		return 1;
	}
    
	if (strcmp(key, "throb") == 0) {
		lua_pushboolean(L, DGConfig.throb);
		return 1;
	}    
	
	if (strcmp(key, "vertical_sync") == 0) {
		lua_pushboolean(L, DGConfig.vertical_sync);
		return 1;
	}
	
	return 0;
}

int luaopen_config(lua_State *L) {
    int ref;
    
	const struct luaL_Reg metaconfig[] =
	{
		{"__index", l_get},
		{"__newindex", l_set},
		{NULL, NULL}
	};
	
	lua_newuserdata(L, sizeof(void *));
    
    lua_pushvalue(L, -1);
    ref = lua_ref(L,LUA_REGISTRYINDEX);
	
	luaL_newmetatable(L, "metaconfig");
	luaL_register(L, NULL, metaconfig);
	lua_setmetatable(L, -2);
	
	lua_newtable(L);
    lua_setfenv(L, -2);
	
	lua_setglobal(L, "config");
	
	return ref;
}

////////////////////////////////////////////////////////////////////////////////
///// Config Library -- END											       /////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///// System Library -- BEGIN				    						   /////
////////////////////////////////////////////////////////////////////////////////

static int system_init(lua_State *L) {
	DGSystemInitialize();
	DGConsoleInitialize();
	DGControlInitialize();
	
	if (DGConfig.show_splash)
		return lua_yield(L, 0);
	else
		return 0;
}

static int system_run(lua_State *L) {
	DGSystemRun();
	
	return 0;
}

static int system_update(lua_State *L) {
	DGControlUpdate(); // Necessary?
	DGSystemUpdate();
	
	return 0;
}

static const struct luaL_reg systemlib_f [] = {
	{"init", system_init},
    {"run", system_run},
	{"update", system_update},
	{NULL, NULL}
};

int luaopen_system(lua_State *L) {
	luaL_newmetatable(L, "dg.system");
	luaL_register(L, "system", systemlib_f);
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
///// System Library -- END											       /////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///// Timer Library -- BEGIN				    						   /////
////////////////////////////////////////////////////////////////////////////////

static int timer_start(lua_State *L) {
	int ref;
	
	if (!lua_isfunction(L, -1)) {
		DGLogError(DG_MOD_SCRIPT, "Function expected as second parameter in timer.start()");
		
		return 0;
	}
	
	ref = luaL_ref(L, LUA_REGISTRYINDEX);  // pop and return a reference to the table.
	
	dg_timer_start((unsigned int)luaL_checknumber(L, 1), ref);
	
	return 0;
}

static const struct luaL_reg timerlib_f [] = {
	{"start", timer_start},
	{NULL, NULL}
};

int luaopen_timer(lua_State *L) {
	luaL_newmetatable(L, "dg.timer");
	luaL_register(L, "timer", timerlib_f);
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
///// Timer Library -- END											       /////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///// Toolkit Functions -- BEGIN		    							   /////
////////////////////////////////////////////////////////////////////////////////

static int global_error(lua_State *L) {
	DGLogError(DG_MOD_NONE, luaL_checkstring(L, 1));
	
	return 0;
}

static int global_feedback(lua_State *L) {
	DGControlShowFeedback(luaL_checkstring(L, 1));
	
	/*if (lua_isstring(L, 2)) {
		// TODO: 0 must look for next free channel
		// TODO: Also, audios that don't loop should be unloaded automatically
		dg_audio_load(31, lua_tostring(L, 2), DG_NO);
		dg_audio_play(31);
	}*/
	
	return 0;
}

static int global_hotkey(lua_State *L) {
	DGControlRegisterHotkey((int)luaL_checknumber(L, 1), luaL_checkstring(L, 2));
	
	return 0;
}

static int global_log(lua_State *L) {
	DGLogTrace(DG_MOD_NONE, luaL_checkstring(L, 1));
	
	return 0;
}

static int global_print (lua_State *L) {
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, LUA_QL("tostring") " must return a string to "
							  LUA_QL("print"));
		if (i>1) fputs("\t", stdout);
		DGLogTrace(DG_MOD_NONE, "%s", s);
		lua_pop(L, 1);  /* pop result */
	}
	return 0;
}

static int global_register(lua_State *L) {
	int ref;
	
	if (!lua_isfunction(L, -1)) {
		DGLogError(DG_MOD_SCRIPT, "Function expected as second parameter in register()");
		
		return 0;
	}
	
	ref = luaL_ref(L, LUA_REGISTRYINDEX);  // pop and return a reference to the table.
	
	DGControlRegisterGlobalHandler((unsigned int)luaL_checknumber(L, 1), ref);
	
	return 0;
}

static int global_room(lua_State *L) {
    // Careful with these restrictions, please
    char aux[256], room[80];
    
    if (room_idx > 80) {
        DGLogError(DG_MOD_SCRIPT, "Maximum numbers of rooms reached... This is wrong. Fix ASAP.");
        
        return 0;
    }
    
    strcpy(room, luaL_checkstring(L, 1));
    
    sprintf(aux, "require \"%s\"", room);
    luaL_dostring(L, aux);

    sprintf(aux, "_G['create_%s']()", room);
    luaL_dostring(L, aux);

    sprintf(aux, "_G['link_%s']()", room);
    strcpy(rooms[room_idx], aux);
    //luaL_dostring(L, aux);
    
    room_idx++;
    
	return 0;
}

static int global_snap(lua_State *L) {
	DGControlTakeSnapshot();
	
	return 0;
}

static int global_sleep(lua_State *L) {
	DGControlSleep((int)luaL_checknumber(L, 1));
	
	return lua_yield(thread, 0);
}

static int global_switch(lua_State *L) {
	DGObject* target = (DGObject*)lua_unboxpointer(L, 1);
	DGControlSwitchTo(target);
	
	return 0;
}

static int global_warning(lua_State *L) {
	DGLogWarning(DG_MOD_NONE, luaL_checkstring(L, 1));
	
	return 0;
}

static const struct luaL_reg globallib [] = {
	{"snap", global_snap},
	{"error", global_error},
	{"feedback", global_feedback},
	{"hotkey", global_hotkey},
	{"log", global_log},
	{"print", global_print},
	{"register", global_register},
    {"room", global_room},
	{"sleep", global_sleep},
	{"switch", node_switch},
	{"warning", global_warning},
	{NULL, NULL}
};

int luaopen_global(lua_State *L) {
	lua_getglobal(L, "_G");
	luaL_register(L, NULL, globallib);
	lua_pop(L, 1);
	
	return 1;
}

static int on_error(lua_State *L) {
	DGLogInfo(DG_MOD_SCRIPT, "Critical Lua error, halting everything");
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
///// Toolkit Functions -- END										       /////
////////////////////////////////////////////////////////////////////////////////

void DGScriptInitialize() {
	int ref, size;
	L = lua_open();
    
	luaopen_config(L);
	
	if (luaL_loadfile(L, DGConfigGetPath(DG_PATH_USER, "dagon.cfg")) == 0) {	
		lua_newtable(L);
		lua_pushvalue(L,-1);
		ref = lua_ref(L, LUA_REGISTRYINDEX);
		lua_setfenv(L,-2);
		
		lua_pcall(L, 0, 0, 0);
		
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		size = lua_objlen(L, -1);
		
		lua_pushnil(L);
		while (lua_next(L, 1) != 0) {
			l_set(L);
			
			lua_pop(L, 1);
		}
	}

	DGLogInitialize();
	DGLogTrace(DG_MOD_SCRIPT, "Initializing script loader...");
	DGLogInfo(DG_MOD_SCRIPT, "Lua version: %s", LUA_VERSION);
	
	luaL_openlibs(L);
	luaopen_camera(L);
	luaopen_audio(L);
	luaopen_spot(L);
	luaopen_node(L);
	luaopen_room(L);
	luaopen_system(L);
	luaopen_timer(L);    
	luaopen_global(L);
    
    // Patchy
    luaL_dostring(L, "package.path = package.path .. \";script/?.lua\"");
    luaL_dostring(L, "package.path = package.path .. \";script/modules/?.lua\"");
    luaL_dostring(L, "package.path = package.path .. \";script/rooms/?.lua\"");
	
	lua_atpanic(L, on_error);
}

void DGScriptTerminate() {
	// This causes a crash on Windows (GC problem, objects needs a ref count)
	lua_close(L);
	
	DGControlTerminate();
	DGConsoleTerminate();
	DGConfigTerminate();
	DGLogTerminate();
	DGSystemTerminate();
}

void DGScriptCallback(int theHandler) {
	static DGBool in_process = DG_NO;
	int result;
	
	if (thread) {		
		luaL_dostring(thread, "self = Spot.current()");
		lua_rawgeti(thread, LUA_REGISTRYINDEX, theHandler);
		if (in_process) {
			result = lua_pcall(thread, 0, 0, 0);
		}
		else {
			in_process = DG_YES;
			result = lua_resume(thread, 0);	
		}
		switch (result) {
			case LUA_ERRRUN:
				DGLogError(DG_MOD_SCRIPT, "Runtime error");
				break;
			case LUA_ERRMEM:
				DGLogError(DG_MOD_SCRIPT, "Memory allocation error");
				break;
			case LUA_ERRERR:
				DGLogError(DG_MOD_SCRIPT, "Error while running handler");
				break;
			case LUA_YIELD:
				break;
			default: // This should be done when the result is 0
				in_process = DG_NO;
				luaL_dostring(thread, "self = nil");
				break;
		}
	}
}

void DGScriptDoLine(const char* aLine) {
	int result;
	
    
    
	if (thread) {
		luaL_loadbuffer(thread, aLine, strlen(aLine), "line");
        result = lua_resume(thread, 0);
		switch (result) {
			case LUA_ERRRUN:
				DGLogError(DG_MOD_SCRIPT, "Runtime error");
				break;
			case LUA_ERRMEM:
				DGLogError(DG_MOD_SCRIPT, "Memory allocation error");
				break;
			case LUA_ERRERR:
				DGLogError(DG_MOD_SCRIPT, "Error while running handler");
				break;
			case LUA_YIELD:
				lua_yield(thread, 0);
				break;
		}
	}
}

void DGScriptLoop() {
    int ref;
    
	if (DGControlIsRunning()) {
		if (DGConsoleIsEnabled()) {
			int error;
			char buff[256];
			
			if (DGConsoleOutput(buff)) {
				DGLogCommand(DG_MOD_SCRIPT, buff);
				error = luaL_loadbuffer(L, buff, strlen(buff), "line") ||
				lua_pcall(L, 0, 0, 0);
				if (error) {
					DGLogTrace(DG_MOD_SCRIPT, "%s", lua_tostring(L, -1));
					lua_pop(L, 1);  /* pop error message from the stack */
				}
			}
		}
		
		DGControlUpdate();
		DGConsoleUpdate(); // Avoid this if not in debug mode
		DGSystemUpdate();
        
        if ((DGCameraState() != DG_CAM_LOOKAT) && DGStateCurrent() != DG_STATE_TIMER){
            ref = dg_timer_check();
            if (ref) {
                DGScriptCallback(ref);
            }
        }
	}
	else {
		DGScriptTerminate();
	}
}

void DGScriptResume() {
	int result;
	
	if (thread) {
        result = lua_resume(thread, 0);
		switch (result) {
			case LUA_ERRRUN:
				DGLogError(DG_MOD_SCRIPT, "Runtime error");
				break;
			case LUA_ERRMEM:
				DGLogError(DG_MOD_SCRIPT, "Memory allocation error");
				break;
			case LUA_ERRERR:
				DGLogError(DG_MOD_SCRIPT, "Error while running handler");
				break;
			case LUA_YIELD:
				break;
			default: // This should be done when the result is 0
				luaL_dostring(thread, "self = nil");
				break;
		}
	}
}

void DGScriptHack() {
    int i;
    
    // FIXME: Temporary hack
    for (i = 0; i < room_idx; i++)
        luaL_dostring(L, rooms[i]);
}

#define LUA_ENUM(L, name, val) \
lua_pushnumber(L, val); \
lua_setfield(L, -2, #name);

void DGScriptRun() {
	FILE* file = NULL;
	char script[DG_MAX_PATHLEN];
	int error, i = 0;
	DGBool can_process;
	
    strcpy(script, DGConfigGetPath(DG_PATH_APP, DGConfig.script));
	strcat(script, ".lua");
	
	lua_getglobal(L, "_G");
	
	LUA_ENUM(L, GLOBAL_ENTER_NODE, DG_EVENT_ENTER_NODE);
	LUA_ENUM(L, GLOBAL_LEAVE_NODE, DG_EVENT_LEAVE_NODE);
	LUA_ENUM(L, GLOBAL_ENTER_ROOM, DG_EVENT_ENTER_ROOM);
	LUA_ENUM(L, GLOBAL_LEAVE_ROOM, DG_EVENT_LEAVE_ROOM);
	LUA_ENUM(L, PRE_RENDER, DG_EVENT_PRE_RENDER);
	LUA_ENUM(L, POST_RENDER, DG_EVENT_POST_RENDER);	
	LUA_ENUM(L, MOUSE_BUTTON, DG_EVENT_MOUSE_BUTTON);
	LUA_ENUM(L, MOUSE_MOVE, DG_EVENT_MOUSE_MOVE);
	
	LUA_ENUM(L, F1, DG_KEY_F1);
	LUA_ENUM(L, F2, DG_KEY_F2);
	LUA_ENUM(L, F3, DG_KEY_F3);
	LUA_ENUM(L, F4, DG_KEY_F4);
	LUA_ENUM(L, F5, DG_KEY_F5);
	LUA_ENUM(L, F6, DG_KEY_F6);
	LUA_ENUM(L, F7, DG_KEY_F7);
	LUA_ENUM(L, F8, DG_KEY_F8);
	LUA_ENUM(L, F9, DG_KEY_F9);
	LUA_ENUM(L, F10, DG_KEY_F10);
	LUA_ENUM(L, F11, DG_KEY_F11);
	LUA_ENUM(L, F12, DG_KEY_F12);
	
	LUA_ENUM(L, NORTH, DG_CAM_FRONT);
	LUA_ENUM(L, EAST, DG_CAM_RIGHT);
	LUA_ENUM(L, SOUTH, DG_CAM_BACK);
	LUA_ENUM(L, WEST, DG_CAM_LEFT);
	LUA_ENUM(L, UP, DG_CAM_TOP);
	LUA_ENUM(L, DOWN, DG_CAM_BOTTOM);
    
    /*LUA_ENUM(L, N, DG_CAM_FRONT);
	LUA_ENUM(L, E, DG_CAM_RIGHT);
	LUA_ENUM(L, S, DG_CAM_BACK);
	LUA_ENUM(L, W, DG_CAM_LEFT);
	LUA_ENUM(L, U, DG_CAM_TOP);
	LUA_ENUM(L, D, DG_CAM_BOTTOM);*/
    
	LUA_ENUM(L, CURRENT, DG_CAM_CURRENT);
	
	LUA_ENUM(L, AUDIO, i++);
	LUA_ENUM(L, CUSTOM, i++);
	LUA_ENUM(L, IMAGE, i++);
	LUA_ENUM(L, FEEDBACK, i++);
	LUA_ENUM(L, SWITCH, i++);
	LUA_ENUM(L, VIDEO, i++);
    
	DGLogTrace(DG_MOD_SCRIPT, "Executing script %s", DGConfig.script);
	
	file = fopen(script, "r");
	
	can_process = DG_YES;
	if (file) {
		thread = lua_newthread(L);
        luaL_loadfile(thread, script);
        error = lua_resume(thread, 0);
		if (error > 1) {	// Hacky; 1 is LUA_YIELD
			switch (error) {
				case LUA_ERRRUN:
					DGLogError(DG_MOD_SCRIPT, "Runtime error");
					break;
				case LUA_ERRMEM:
					DGLogError(DG_MOD_SCRIPT, "Memory allocation error");
					break;
				case LUA_ERRERR:
					DGLogError(DG_MOD_SCRIPT, "Problem with error handler");
					break;					
				default:
					DGLogError(DG_MOD_SCRIPT, "Undefined error");
					break;
			}
			
			// TODO: Must do traceback here
			DGLogTrace(DG_MOD_SCRIPT, "%s", lua_tostring(L, -1));
			lua_pop(L, 1);
			
			// Force manual mode
			if (!DGControlIsRunning())
				system_init(L);
			
			DGConsoleEnable();
		}
		fclose(file);
        
        DGScriptHack();
        
        DGSystemRun();
	}
	else {
		DGLogWarning(DG_MOD_SCRIPT, "Script not found");
		system_init(L);
		DGConsoleEnable();
        DGSystemRun();
	}
}

void DGScriptYield() {
    lua_yield(thread, 0); 
}

////////////////////////////////////////////////////////////////////////////////

// Dump Lua stack
/*static void stackDump (lua_State *L) {
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) {  // repeat for each level
        int t = lua_type(L, i);
        switch (t) {
				
			case LUA_TSTRING:  // strings
				printf("`%s'", lua_tostring(L, i));
				break;
				
			case LUA_TBOOLEAN:  // booleans
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
				
			case LUA_TNUMBER:  // numbers
				printf("%g", lua_tonumber(L, i));
				break;
				
			default:  // other values
				printf("%s", lua_typename(L, t));
				break;
				
        }
        printf("  ");  // put a separator
	}
	printf("\n");  // end the listing
}*/
