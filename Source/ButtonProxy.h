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

#ifndef DG_BUTTONPROXY_H
#define DG_BUTTONPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Button.h"
#include "DGScript.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class ButtonProxy : public ObjectProxy {
public:
    static const char className[];
    static Luna<ButtonProxy>::RegType methods[];
    
    // Constructor
    ButtonProxy(lua_State *L) {
        b = new Button();
        
        b->setPosition(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        b->setSize(luaL_checknumber(L, 3), luaL_checknumber(L, 4));
        
        // Init the base class
        this->setObject(b);
    }
    
    // Destructor
    ~ButtonProxy() { delete b; }
    
    // Move the button
    int move(lua_State *L) {
        b->move(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Return the position
    int position(lua_State *L) {
        Point position = b->position();
        lua_pushnumber(L, position.x);
        lua_pushnumber(L, position.y);        
        return 2;
    }
    
    // Scale the button
    int scale(lua_State *L) {
        b->scale(luaL_checknumber(L, 1));
        return 0;
    }
    
    // Set an action
    int setAction(lua_State *L) {
        Action action;
        
        int type = (int)luaL_checknumber(L, 1);
        
        switch (type) {
            case FEED:
                action.type = kActionFeed;
                action.cursor = kCursorLook;                
                action.feed = luaL_checkstring(L, 2);
            
                if (lua_isstring(L, 3)) {
                  action.feedAudio = lua_tostring(L, 3);
                  action.hasFeedAudio = true;
                }
                else action.hasFeedAudio = false;
                
                b->setAction(action);
                
                break;
            case FUNCTION:
                if (!lua_isfunction(L, 2)) {
                    Log::instance().error(kModScript, "%s", kString14009);
                    return 0;
                }
                
                action.type = kActionFunction;
                action.cursor = kCursorUse;
                action.luaHandler = luaL_ref(L, LUA_REGISTRYINDEX); // Pop and return a reference to the table
                b->setAction(action);
                
                break;                
            case SWITCH:
                int type = DGCheckProxy(L, 2);
                if (type == kObjectNode)
                    action.target = DGProxyToNode(L, 2);
                else if (type == kObjectRoom)
                    action.target = DGProxyToRoom(L, 2);
                else {
                    Log::instance().error(kModScript, "%s", kString14008);
                    return 0;
                }
                
                action.type = kActionSwitch;
                action.cursor = kCursorForward;                
                b->setAction(action);
                
                break;
        }
                
        return 0;
    }
    
    // Set a cursor for the action
    int setCursor(lua_State *L) {
      b->updateCursor(luaL_checknumber(L, 1));
        
        return 0;
    }
    
    // Set the font for text
    int setFont(lua_State *L) {
        b->setFont(luaL_checkstring(L, 1), luaL_checknumber(L, 2));
        return 0;
    }  
    
    // Set the background image
    int setImage(lua_State *L) {
        b->setTexture(luaL_checkstring(L, 1));
        return 0;
    }    
    
    // Set a new position
    int setPosition(lua_State *L) {
        b->setPosition(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Set a new size
    int setSize(lua_State *L) {
        b->setSize(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Set a text
    int setText(lua_State *L) {
        b->setText(luaL_checkstring(L, 1));
        return 0;
    }
    
    // Set the color of text
    int setTextColor(lua_State *L) {
		unsigned int color = lua_tonumber(L, 1);
        b->setTextColor(color);
        return 0;
    }
    
    // Return the size
    int size(lua_State *L) {
        Size size = b->size();
        lua_pushnumber(L, size.width);
        lua_pushnumber(L, size.height);        
        return 2;
    }
    
    // Return the text
    int text(lua_State *L) {
        lua_pushstring(L, b->text().c_str());
        return 1;
    }
    
    Button* ptr() { return b; }
    
private:
    Button* b;
    
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char ButtonProxy::className[] = ButtonProxyName;

Luna<ButtonProxy>::RegType ButtonProxy::methods[] = {
    ObjectMethods(ButtonProxy),      
    method(ButtonProxy, move),
    method(ButtonProxy, position),
    method(ButtonProxy, scale),
    method(ButtonProxy, setAction),
    method(ButtonProxy, setCursor),
    method(ButtonProxy, setFont),     
    method(ButtonProxy, setImage),    
    method(ButtonProxy, setPosition),
    method(ButtonProxy, setSize),
    method(ButtonProxy, setText),    
    method(ButtonProxy, setTextColor),     
    method(ButtonProxy, size),
    method(ButtonProxy, text),       
    {0,0}
};

#endif // DG_BUTTONPROXY_H
