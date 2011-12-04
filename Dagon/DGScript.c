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

static DG_OBJECT* checkobject(lua_State *L, unsigned type, int index) {
	luaL_checktype(L, index, LUA_TUSERDATA);
	lua_getmetatable(L, index);
	
	if (!lua_equal(L, lua_upvalueindex(1), -1)) {
		switch (type) {
			case DG_OBJ_AUDIO:
				luaL_typerror(L, index, AUDIO_OBJ);  /* die */
				break;
			case DG_OBJ_ROOM:
				luaL_typerror(L, index, ROOM);  /* die */
				break;
			case DG_OBJ_NODE:
				luaL_typerror(L, index, NODE);  /* die */
				break;
			case DG_OBJ_SPOT:
				luaL_typerror(L, index, SPOT);  /* die */
				break;             
		}
	}
	
	lua_pop(L, 1);
	
	return (DG_OBJECT*)lua_unboxpointer(L, index);
}

static DG_OBJECT* pushobject(lua_State *L, DG_OBJECT* obj) {
	dg_object_ref(obj);
	
	lua_boxpointer(L, obj);
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
	
	return obj;
}

static int protectobject(lua_State *L) {
	dg_log_error(DG_MOD_SCRIPT, "Operation not permitted");
	
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
	DG_OBJECT* audio;
	DG_BOOL loop;
	
	if (lua_isboolean(L, 2))
		loop = lua_toboolean(L, 2);
	else
		loop = DG_NO;
	
	audio = dg_audio_new(luaL_checkstring(L, 1), loop);
	pushobject(L, audio);
	
	dg_audio_load(audio);
	
	return 1;
}

static int audio_destroy(lua_State *L) {
	// WARNING: Very, very careful with this! Could break the arrays...
	
	DG_OBJECT* obj = (DG_OBJECT*)lua_unboxpointer(L, 1);
	//if (obj) dg_audio_release(obj);
	if (obj) dg_audio_unload(obj);
	
	return 0;
}

static int audio_fade_in(lua_State *L) {
	DG_OBJECT* audio = checkobject(L, DG_OBJ_AUDIO, 1);
	
	dg_audio_fade_in(audio, (int)lua_tonumber(L, 2));
	
	return 0;
}

static int audio_fade_out(lua_State *L) {
	DG_OBJECT* audio = checkobject(L, DG_OBJ_AUDIO, 1);
	
	dg_audio_fade_out(audio, (int)lua_tonumber(L, 2));
	
	return 0;
}

static int audio_is_playing(lua_State *L) {
	DG_OBJECT* audio = checkobject(L, DG_OBJ_AUDIO, 1);
	DG_BOOL playing;
	
	playing = dg_audio_is_playing(audio);
	
	lua_pushboolean(L, playing);
	lua_pushvalue(L, -1);
	
	return 1;
}

static int audio_play(lua_State *L) {
	DG_OBJECT* audio = checkobject(L, DG_OBJ_AUDIO, 1);
	
	dg_audio_play(audio);
	
	return 0;
}

static int audio_pause(lua_State *L) {
	DG_OBJECT* audio = checkobject(L, DG_OBJ_AUDIO, 1);
	
	dg_audio_pause(audio);
	
	return 0;
}

static int audio_stop(lua_State *L) {
	DG_OBJECT* audio = checkobject(L, DG_OBJ_AUDIO, 1);
	
	dg_audio_stop(audio);
	
	return 0;
}

static int audio_volume(lua_State *L) {
	DG_OBJECT* audio = checkobject(L, DG_OBJ_AUDIO, 1);
	
	dg_audio_set_volume(audio, (float)lua_tonumber(L, 2));
	
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
	DG_OBJECT* spot;
	int i, size, face;
	short* coords;
	
	if (!lua_istable(L, 2)) {
		luaL_error(L, "Table with coordinates expected to create spot");
		
		return 0;
	}
	
	size = lua_objlen(L, 2);
	
	coords = (short*)dg_alloc(size * sizeof(short));
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
			dg_log_error(DG_MOD_SCRIPT, "Invalid face specified");
			return 0;
			break;
	}
	
	spot = dg_spot_new(coords, size, face, 0);
	pushobject(L, spot);
	
	dg_free(coords);
	
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
	DG_OBJECT* spot = dg_world_current_spot();
	pushobject(L, spot);
	
	return 1;
}

static int spot_desc(lua_State *L) {
	char aux[DG_MAX_LOGLEN];
	DG_OBJECT* self = lua_unboxpointer(L, 1);
	
	if (strcmp(self->name, "") == 0)
		sprintf(aux, "spot");
	else
		sprintf(aux, "%s (spot)", self->name);
	
	lua_pushstring(L, aux);
	
	return 1;
}


static int spot_attach(lua_State *L) {
	DG_ACTION action;
	DG_OBJECT* spot = checkobject(L, DG_OBJ_SPOT, 1);
    DG_OBJECT* audio;
	DG_OBJECT* video;
	
	DG_BOOL autoplay = DG_NO;
	DG_BOOL loop = DG_NO;
	DG_BOOL sync = DG_NO;
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
            
            audio = dg_audio_new(luaL_checkstring(L, 3), loop);
            dg_audio_set_volume(audio, volume);
			dg_spot_set_audio(spot, audio);
            
			break;
		case CUSTOM:
			if (!lua_isfunction(L, -1)) {
				dg_log_error(DG_MOD_SCRIPT, "Function expected as second parameter in attach()");
				
				return 0;
			}
			
			ref = luaL_ref(L, LUA_REGISTRYINDEX);  // pop and return a reference to the table.
			
			action.type = DG_ACT_CUSTOM;
			action.custom_handler = ref;
			
			dg_spot_set_action(checkobject(L, DG_OBJ_SPOT, 1), &action);
			if (!dg_spot_has_color(spot))
				dg_spot_set_color(spot, 0);
			
			break;
		case IMAGE:
			dg_spot_set_image(spot, luaL_checkstring(L, 3));
			break;
		case FEEDBACK:
			action.type = DG_ACT_FEEDBACK;
			
			strcpy(action.feedback, luaL_checkstring(L, 3));
			
			if (lua_isstring(L, 4))
				strcpy(action.feedback_audio, lua_tostring(L, 4));
			
			dg_spot_set_action(checkobject(L, DG_OBJ_SPOT, 1), &action);
			if (!dg_spot_has_color(spot))
				dg_spot_set_color(spot, 0);
			
			break;
		case SWITCH:
			action.type = DG_ACT_SWITCH;
			action.target = (DG_OBJECT*)lua_unboxpointer(L, 3);
			
			dg_spot_set_action(checkobject(L, DG_OBJ_SPOT, 1), &action);
			if (!dg_spot_has_color(spot))
				dg_spot_set_color(spot, 0);
			
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

			video = dg_video_new(luaL_checkstring(L, 3), autoplay, loop, sync);
			dg_spot_set_video(spot, video);
			
			if (autoplay)
				dg_spot_play(spot);
			
			break;
		default:
			dg_log_error(DG_MOD_SCRIPT, "Invalid type in attach() function");
			break;
	}
	
	return 0;
}

static int spot_enable(lua_State *L) {
	dg_spot_enable(checkobject(L, DG_OBJ_SPOT, 1));
	
	return 0;
}

static int spot_disable(lua_State *L) {
	dg_spot_disable(checkobject(L, DG_OBJ_SPOT, 1));
	
	return 0;
}

static int spot_is_enabled(lua_State *L) {
	DG_BOOL ret = dg_spot_is_enabled(checkobject(L, DG_OBJ_SPOT, 1));
	
	lua_pushboolean(L, ret);
	
	return 1;
}

static int spot_play(lua_State *L) {
	DG_OBJECT* spot = checkobject(L, DG_OBJ_SPOT, 1);
	//DG_OBJECT* video = dg_spot_video(spot);
	
	dg_spot_play(spot);
    
    if (dg_spot_has_audio(spot)) {
        DG_OBJECT* audio = dg_spot_audio(spot);
        
        if (!dg_audio_is_playing(audio)) {
            dg_audio_play(audio);
        }
    }
    
    // This can't happen here, should check if we're in the node first
	/*if (dg_video_is_synced(video))
		return lua_yield(thread, 1);*/
	
	return 0;
}

static int spot_stop(lua_State *L) {
    
    DG_OBJECT* spot = checkobject(L, DG_OBJ_SPOT, 1);
	dg_spot_stop(spot);
    
    // Hack: too many tasks performed here
    if (dg_spot_has_video(spot)) {
        DG_OBJECT* video = dg_spot_video(spot);
        dg_video_stop(video);
    }
	
	return 0;
}

static int spot_is_playing(lua_State *L) {
	DG_BOOL ret = dg_spot_is_playing(checkobject(L, DG_OBJ_SPOT, 1));
	
	lua_pushboolean(L, ret);
	
	return 1;
}

static int spot_toggle(lua_State *L) {
	dg_spot_toggle(checkobject(L, DG_OBJ_SPOT, 1));
	
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
	DG_OBJECT* node = dg_node_new(luaL_checkstring(L, 1));
	pushobject(L, node);
	
	// Temporary
	dg_world_add_node(node);
	
	return 1;
}

static int node_destroy(lua_State *L) {
	// WARNING: Very, very careful with this! Could break the arrays...
	
	DG_OBJECT* obj = (DG_OBJECT*)lua_unboxpointer(L, 1);
	if (obj) dg_node_release(obj);
	
	return 0;
}

static int node_current(lua_State *L) {
	DG_OBJECT* node = dg_world_current_node();
	pushobject(L, node);
	
	return 1;
}

static int node_desc(lua_State *L) {
	char aux[DG_MAX_LOGLEN];
	DG_OBJECT* self = lua_unboxpointer(L, 1);
	
	if (strcmp(self->name, "") == 0)
		sprintf(aux, "node");
	else
		sprintf(aux, "%s (node)", self->name);
	
	lua_pushstring(L, aux);
	
	return 1;
}

static int node_equal(lua_State *L) {
	DG_OBJECT* node1 = lua_unboxpointer(L, 1);
    DG_OBJECT* node2 = lua_unboxpointer(L, 2);
	
	if (strcmp(node1->name, node2->name) == 0)
        return 1;
	
	return 0;
}

static int node_add_spot(lua_State *L) {
	DG_OBJECT* target;
	DG_OBJECT* self = checkobject(L, DG_OBJ_NODE, 1);
	
	if (lua_isnumber(L, 2)) { // Using the convenience syntax
		// Doesn't work yet!!
		lua_remove(L, 1); // Remove the first element so that the admitted values are shifted
		spot_new(L);
	}

	target = (DG_OBJECT*)lua_unboxpointer(L, -1);

	dg_node_add_spot(self, target);
	
	return 1;
}

static int node_link(lua_State *L) {
	int size;
	DG_OBJECT* self = checkobject(L, DG_OBJ_NODE, 1);
	
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
			if (strcmp(key, "N") == 0) dg_node_add_custom_link(self, DG_NORTH, ref);
			else if (strcmp(key, "W") == 0) dg_node_add_custom_link(self, DG_WEST, ref);
			else if (strcmp(key, "S") == 0) dg_node_add_custom_link(self, DG_SOUTH, ref);
			else if (strcmp(key, "E") == 0) dg_node_add_custom_link(self, DG_EAST, ref);
			else if (strcmp(key, "NW") == 0) dg_node_add_custom_link(self, DG_NORTHWEST, ref);
			else if (strcmp(key, "SW") == 0) dg_node_add_custom_link(self, DG_SOUTHWEST, ref);
			else if (strcmp(key, "SE") == 0) dg_node_add_custom_link(self, DG_SOUTHEAST, ref);
			else if (strcmp(key, "NE") == 0) dg_node_add_custom_link(self, DG_NORTHEAST, ref);
			
			// Runtime error if we pop the 1st value here. Why?
		}
		else {
			DG_OBJECT* target = (DG_OBJECT*)lua_unboxpointer(L, -1);
			
			// Ugly nesting of strcmps() but there's probably no better way of doing this...
			if (strcmp(key, "N") == 0) dg_node_add_link(self, DG_NORTH, target);
			else if (strcmp(key, "W") == 0) dg_node_add_link(self, DG_WEST, target);
			else if (strcmp(key, "S") == 0) dg_node_add_link(self, DG_SOUTH, target);
			else if (strcmp(key, "E") == 0) dg_node_add_link(self, DG_EAST, target);
			else if (strcmp(key, "NW") == 0) dg_node_add_link(self, DG_NORTHWEST, target);
			else if (strcmp(key, "SW") == 0) dg_node_add_link(self, DG_SOUTHWEST, target);
			else if (strcmp(key, "SE") == 0) dg_node_add_link(self, DG_SOUTHEAST, target);
			else if (strcmp(key, "NE") == 0) dg_node_add_link(self, DG_NORTHEAST, target);
			
			lua_pop(L, 1);
		}
	}
	
	return 0;
}

static int node_switch(lua_State *L) {
	dg_control_switch(lua_unboxpointer(L, 1));
	
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
	DG_OBJECT* room = dg_room_new(luaL_checkstring(L, 1));
	pushobject(L, room);
	dg_world_add_room(room);
	
	return 1;
}

static int room_destroy(lua_State *L) {
	// WARNING: Very, very careful with this! Could break the arrays...
	
	DG_OBJECT* obj = (DG_OBJECT*)lua_unboxpointer(L, 1);
	if (obj) dg_room_release(obj);
	
	return 0;
}

static int room_current(lua_State *L) {
	DG_OBJECT* room = dg_world_current_room();
	pushobject(L, room);
	
	return 1;
}

static int room_desc(lua_State *L) {
	char aux[DG_MAX_LOGLEN];
	DG_OBJECT* self = lua_unboxpointer(L, 1);
	
	if (strcmp(self->name, "") == 0)
		sprintf(aux, "room");
	else
		sprintf(aux, "%s (room)", self->name);
	
	lua_pushstring(L, aux);
	
	return 1;
}


static int room_add_node(lua_State *L) {
	DG_OBJECT* self = checkobject(L, DG_OBJ_ROOM, 1);
	DG_OBJECT* target = (DG_OBJECT*)lua_unboxpointer(L, -1);
	
	dg_room_add_node(self, target);
	
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
		dg_camera_set_breathe_fx(lua_toboolean(L, 3));
	
	if (strcmp(key, "fov") == 0)
		dg_camera_set_fov((float)luaL_checknumber(L, 3));
	
	if (strcmp(key, "neutral_zone") == 0)
		dg_camera_set_neutral_zone((int)luaL_checknumber(L, 3));
	
	if (strcmp(key, "pan_speed") == 0)
		dg_camera_set_pan_speed((int)luaL_checknumber(L, 3));
	
	if (strcmp(key, "vertical_limit") == 0)
		dg_camera_set_vertical_limit((int)luaL_checknumber(L, 3));
	
	if (strcmp(key, "walk") == 0)
		dg_camera_set_walk_fx(lua_toboolean(L, 3));
	
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
		lua_pushboolean(L, dg_camera_breathe_fx());
		return 1;
	}
	
	if (strcmp(key, "fov") == 0) {
		lua_pushnumber(L, dg_camera_fov());
		return 1;
	}
	
	if (strcmp(key, "neutral_zone") == 0) {
		lua_pushnumber(L, dg_camera_neutral_zone());
		return 1;
	}
	
	if (strcmp(key, "pan_speed") == 0) {
		lua_pushnumber(L, dg_camera_pan_speed());
		return 1;
	}
	
	if (strcmp(key, "vertical_limit") == 0) {
		lua_pushnumber(L, dg_camera_vertical_limit());
		return 1;
	}
	
	if (strcmp(key, "walk") == 0) {
		lua_pushboolean(L, dg_camera_walk_fx());
		return 1;
	}
	
	return 0;
}

static int camera_lookat(lua_State *L) {
	int angle_h = (int)luaL_checknumber(L, 1);
	DG_BOOL instant = lua_toboolean(L, 3);
	
	if (lua_isnumber(L, 2))
		dg_camera_lookat(angle_h, (int)lua_tonumber(L, 2), instant);
	else
		dg_camera_lookat(angle_h, DG_CAM_CURRENT, instant);
	
	if (instant)
		return 0;
	else
		return lua_yield(L, 0);
}

static int camera_get_angle(lua_State *L) {
	int angleh = dg_camera_angle_h();
	int anglev = dg_camera_angle_v();
	
	lua_pushnumber(L, angleh);
	lua_pushnumber(L, anglev);
	
	return 2;
}

static int camera_get_origin(lua_State *L) {
	DG_POINT point = dg_camera_origin();
	
	lua_pushnumber(L, point.x);
	lua_pushnumber(L, point.y);
	
	return 2;
}

static int camera_set_origin(lua_State *L) {
	dg_camera_set_origin((int)luaL_checknumber(L, 1), (int)luaL_checknumber(L, 2));
	
	return 0;
}

static int camera_get_viewport(lua_State *L) {
	DG_SIZE size = dg_camera_viewport();
	
	lua_pushnumber(L, size.width);
	lua_pushnumber(L, size.height);
	
	return 2;
}

static int camera_set_ortho(lua_State *L) {
	dg_camera_set_ortho();
	
	return 0;
}

static int camera_set_perspective(lua_State *L) {
	dg_camera_set_perspective();
	
	return 0;
}

static int camera_set_viewport(lua_State *L) {
	dg_camera_set_viewport((int)luaL_checknumber(L, 1), (int)luaL_checknumber(L, 2));
	
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
		config.antialiasing = lua_toboolean(L, 3);
	
	if (strcmp(key, "display_width") == 0)
		config.display_width = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "display_height") == 0)
		config.display_height = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "display_depth") == 0)
		config.display_depth = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "debug_mode") == 0)
		config.debug_mode = lua_toboolean(L, 3);

    if (strcmp(key, "dust") == 0)
		config.dust = lua_toboolean(L, 3);
	
	if (strcmp(key, "effects") == 0)
		config.effects = lua_toboolean(L, 3);
	
	if (strcmp(key, "framerate") == 0)
		config.framerate = (int)luaL_checknumber(L, 3);
	
	if (strcmp(key, "fullscreen") == 0)
		config.fullscreen = lua_toboolean(L, 3);
	
	if (strcmp(key, "log") == 0)
		config.log = lua_toboolean(L, 3);
	
	if (strcmp(key, "multithread") == 0)
		config.multithread = lua_toboolean(L, 3);
	
	if (strcmp(key, "path") == 0) {
        if (config.debug_mode)
            strcpy(config.res_path, luaL_checkstring(L, 3));
        else
            // Protect the current path
            strcat(config.res_path, luaL_checkstring(L, 3));
    }
    
	if (strcmp(key, "scare") == 0)
		config.scare = lua_toboolean(L, 3);
   
    if (strcmp(key, "script") == 0)
		strcpy(config.script, luaL_checkstring(L, 3));
	
	if (strcmp(key, "show_splash") == 0)
		config.show_splash = lua_toboolean(L, 3);
	
	if (strcmp(key, "show_spots") == 0)
		config.show_spots = lua_toboolean(L, 3);
	
	if (strcmp(key, "tex_compression") == 0)
		config.tex_compression = lua_toboolean(L, 3);
	
	if (strcmp(key, "tex_extension") == 0)
		// Careful with the length
		strcpy(config.tex_extension, luaL_checkstring(L, 3));
    
	if (strcmp(key, "throb") == 0)
		config.throb = lua_toboolean(L, 3);    
	
	if (strcmp(key, "vertical_sync") == 0)
		config.vertical_sync = lua_toboolean(L, 3);	
	
	return 0;
}

static int l_get(lua_State *L) {
	const char *key = luaL_checkstring(L, 2);
	
	if (strcmp(key, "antialiasing") == 0) {
		lua_pushboolean(L, config.antialiasing);
		return 1;
	}
	
	if (strcmp(key, "display_width") == 0) {
		lua_pushnumber(L, config.display_width);
		return 1;
	}
	
	if (strcmp(key, "display_height") == 0) {
		lua_pushnumber(L, config.display_height);
		return 1;
	}
	
	if (strcmp(key, "display_depth") == 0) {
		lua_pushnumber(L, config.display_depth);
		return 1;
	}
	
	if (strcmp(key, "debug_mode") == 0) {
		lua_pushboolean(L, config.debug_mode);
		return 1;
	}
    
    if (strcmp(key, "dust") == 0) {
		lua_pushboolean(L, config.dust);
		return 1;
	}
	
	if (strcmp(key, "effects") == 0) {
		lua_pushboolean(L, config.effects);
		return 1;
	}
	
	if (strcmp(key, "framerate") == 0) {
		lua_pushnumber(L, config.framerate);
		return 1;
	}
	
	if (strcmp(key, "fullscreen") == 0) {
		lua_pushboolean(L, config.fullscreen);
		return 1;
	}
	
	if (strcmp(key, "log") == 0) {
		lua_pushboolean(L, config.log);
		return 1;
	}
	
	if (strcmp(key, "multithread") == 0) {
		lua_pushboolean(L, config.multithread);
		return 1;
	}
	
	if (strcmp(key, "path") == 0) {
		lua_pushstring(L, config.user_path);
		return 1;
	}
	
    if (strcmp(key, "scare") == 0) {
		lua_pushboolean(L, config.scare);
		return 1;
	}    
    
	if (strcmp(key, "script") == 0) {
		lua_pushstring(L, config.script);
		return 1;
	}
	
	if (strcmp(key, "show_splash") == 0) {
		lua_pushboolean(L, config.show_splash);
		return 1;
	}
	
	if (strcmp(key, "show_spots") == 0) {
		lua_pushboolean(L, config.show_spots);
		return 1;
	}
	
	if (strcmp(key, "tex_compression") == 0) {
		lua_pushboolean(L, config.tex_compression);
		return 1;
	}

	if (strcmp(key, "tex_extension") == 0) {
		lua_pushstring(L, config.tex_extension);
		return 1;
	}
    
	if (strcmp(key, "throb") == 0) {
		lua_pushboolean(L, config.throb);
		return 1;
	}    
	
	if (strcmp(key, "vertical_sync") == 0) {
		lua_pushboolean(L, config.vertical_sync);
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
	dg_system_init();
	dg_console_init();
	dg_control_init();
	
	if (config.show_splash)
		return lua_yield(L, 0);
	else
		return 0;
}

static int system_run(lua_State *L) {
	dg_system_run();
	
	return 0;
}

static int system_update(lua_State *L) {
	dg_control_update(); // Necessary?
	dg_system_update();
	
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
		dg_log_error(DG_MOD_SCRIPT, "Function expected as second parameter in timer.start()");
		
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
	dg_log_error(DG_MOD_NONE, luaL_checkstring(L, 1));
	
	return 0;
}

static int global_feedback(lua_State *L) {
	dg_control_feedback(luaL_checkstring(L, 1));
	
	/*if (lua_isstring(L, 2)) {
		// TODO: 0 must look for next free channel
		// TODO: Also, audios that don't loop should be unloaded automatically
		dg_audio_load(31, lua_tostring(L, 2), DG_NO);
		dg_audio_play(31);
	}*/
	
	return 0;
}

static int global_hotkey(lua_State *L) {
	dg_control_register_hotkey((int)luaL_checknumber(L, 1), luaL_checkstring(L, 2));
	
	return 0;
}

static int global_log(lua_State *L) {
	dg_log_trace(DG_MOD_NONE, luaL_checkstring(L, 1));
	
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
		dg_log_trace(DG_MOD_NONE, "%s", s);
		lua_pop(L, 1);  /* pop result */
	}
	return 0;
}

static int global_register(lua_State *L) {
	int ref;
	
	if (!lua_isfunction(L, -1)) {
		dg_log_error(DG_MOD_SCRIPT, "Function expected as second parameter in register()");
		
		return 0;
	}
	
	ref = luaL_ref(L, LUA_REGISTRYINDEX);  // pop and return a reference to the table.
	
	dg_control_register_global_handler((unsigned int)luaL_checknumber(L, 1), ref);
	
	return 0;
}

static int global_room(lua_State *L) {
    // Careful with these restrictions, please
    char aux[256], room[80];
    
    if (room_idx > 80) {
        dg_log_error(DG_MOD_SCRIPT, "Maximum numbers of rooms reached... This is wrong. Fix ASAP.");
        
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
	dg_control_snap();
	
	return 0;
}

static int global_sleep(lua_State *L) {
	dg_control_sleep((int)luaL_checknumber(L, 1));
	
	return lua_yield(thread, 0);
}

static int global_switch(lua_State *L) {
	DG_OBJECT* target = (DG_OBJECT*)lua_unboxpointer(L, 1);
	dg_control_switch(target);
	
	return 0;
}

static int global_warning(lua_State *L) {
	dg_log_warning(DG_MOD_NONE, luaL_checkstring(L, 1));
	
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
	dg_log_info(DG_MOD_SCRIPT, "Critical Lua error, halting everything");
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
///// Toolkit Functions -- END										       /////
////////////////////////////////////////////////////////////////////////////////

void dg_script_init() {
	int ref, size;
	L = lua_open();
    
	luaopen_config(L);
	
	if (luaL_loadfile(L, dg_config_get_path(DG_USER, "dagon.cfg")) == 0) {	
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

	dg_log_init();
	dg_log_trace(DG_MOD_SCRIPT, "Initializing script loader...");
	dg_log_info(DG_MOD_SCRIPT, "Lua version: %s", LUA_VERSION);
	
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

void dg_script_release() {
	// This causes a crash on Windows (GC problem, objects needs a ref count)
	lua_close(L);
	
	dg_control_release();
	dg_console_release();
	dg_config_finalize();
	dg_log_release();
	dg_system_release();
}

void dg_script_callback(int ref) {
	static DG_BOOL in_process = DG_NO;
	int result;
	
	if (thread) {		
		luaL_dostring(thread, "self = Spot.current()");
		lua_rawgeti(thread, LUA_REGISTRYINDEX, ref);
		if (in_process) {
			result = lua_pcall(thread, 0, 0, 0);
		}
		else {
			in_process = DG_YES;
			result = lua_resume(thread, 0);	
		}
		switch (result) {
			case LUA_ERRRUN:
				dg_log_error(DG_MOD_SCRIPT, "Runtime error");
				break;
			case LUA_ERRMEM:
				dg_log_error(DG_MOD_SCRIPT, "Memory allocation error");
				break;
			case LUA_ERRERR:
				dg_log_error(DG_MOD_SCRIPT, "Error while running handler");
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

void dg_script_doline(const char* line) {
	int result;
	
    
    
	if (thread) {
		luaL_loadbuffer(thread, line, strlen(line), "line");
        result = lua_resume(thread, 0);
		switch (result) {
			case LUA_ERRRUN:
				dg_log_error(DG_MOD_SCRIPT, "Runtime error");
				break;
			case LUA_ERRMEM:
				dg_log_error(DG_MOD_SCRIPT, "Memory allocation error");
				break;
			case LUA_ERRERR:
				dg_log_error(DG_MOD_SCRIPT, "Error while running handler");
				break;
			case LUA_YIELD:
				lua_yield(thread, 0);
				break;
		}
	}
}

void dg_script_loop() {
    int ref;
    
	if (dg_control_is_running()) {
		if (dg_console_is_enabled()) {
			int error;
			char buff[256];
			
			if (dg_console_output(buff)) {
				dg_log_command(DG_MOD_SCRIPT, buff);
				error = luaL_loadbuffer(L, buff, strlen(buff), "line") ||
				lua_pcall(L, 0, 0, 0);
				if (error) {
					dg_log_trace(DG_MOD_SCRIPT, "%s", lua_tostring(L, -1));
					lua_pop(L, 1);  /* pop error message from the stack */
				}
			}
		}
		
		dg_control_update();
		dg_console_update(); // Avoid this if not in debug mode
		dg_system_update();
        
        if ((dg_camera_state() != DG_CAM_LOOKAT) && dg_state_current() != DG_ST_TIMER){
            ref = dg_timer_check();
            if (ref) {
                dg_script_callback(ref);
            }
        }
	}
	else {
		dg_script_release();
	}
}

void dg_script_resume() {
	int result;
	
	if (thread) {
        result = lua_resume(thread, 0);
		switch (result) {
			case LUA_ERRRUN:
				dg_log_error(DG_MOD_SCRIPT, "Runtime error");
				break;
			case LUA_ERRMEM:
				dg_log_error(DG_MOD_SCRIPT, "Memory allocation error");
				break;
			case LUA_ERRERR:
				dg_log_error(DG_MOD_SCRIPT, "Error while running handler");
				break;
			case LUA_YIELD:
				break;
			default: // This should be done when the result is 0
				luaL_dostring(thread, "self = nil");
				break;
		}
	}
}

void dg_script_hack() {
    int i;
    
    // FIXME: Temporary hack
    for (i = 0; i < room_idx; i++)
        luaL_dostring(L, rooms[i]);
}

#define LUA_ENUM(L, name, val) \
lua_pushnumber(L, val); \
lua_setfield(L, -2, #name);

void dg_script_run() {
	FILE* file = NULL;
	char script[DG_MAX_PATHLEN];
	int error, i = 0;
	DG_BOOL can_process;
	
    strcpy(script, dg_config_get_path(DG_APP, config.script));
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
    
	dg_log_trace(DG_MOD_SCRIPT, "Executing script %s", config.script);
	
	file = fopen(script, "r");
	
	can_process = DG_YES;
	if (file) {
		thread = lua_newthread(L);
        luaL_loadfile(thread, script);
        error = lua_resume(thread, 0);
		if (error > 1) {	// Hacky; 1 is LUA_YIELD
			switch (error) {
				case LUA_ERRRUN:
					dg_log_error(DG_MOD_SCRIPT, "Runtime error");
					break;
				case LUA_ERRMEM:
					dg_log_error(DG_MOD_SCRIPT, "Memory allocation error");
					break;
				case LUA_ERRERR:
					dg_log_error(DG_MOD_SCRIPT, "Problem with error handler");
					break;					
				default:
					dg_log_error(DG_MOD_SCRIPT, "Undefined error");
					break;
			}
			
			// TODO: Must do traceback here
			dg_log_trace(DG_MOD_SCRIPT, "%s", lua_tostring(L, -1));
			lua_pop(L, 1);
			
			// Force manual mode
			if (!dg_control_is_running())
				system_init(L);
			
			dg_console_enable();
		}
		fclose(file);
        
        dg_script_hack();
        
        dg_system_run();
	}
	else {
		dg_log_warning(DG_MOD_SCRIPT, "Script not found");
		system_init(L);
		dg_console_enable();
        dg_system_run();
	}
}

void dg_script_yield() {
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
