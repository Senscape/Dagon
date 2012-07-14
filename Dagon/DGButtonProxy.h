////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
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

#include "DGButton.h"
#include "DGScript.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGButtonProxy : public DGObjectProxy {
public:
    static const char className[];
    static Luna<DGButtonProxy>::RegType methods[];
    
    // Constructor
    DGButtonProxy(lua_State *L) {
        b = new DGButton();
        
        b->setPosition(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        b->setSize(luaL_checknumber(L, 3), luaL_checknumber(L, 4));
        
        // Init the base class
        this->setObject(b);
    }
    
    // Destructor
    ~DGButtonProxy() { delete b; }
    
    // Move the button
    int move(lua_State *L) {
        b->move(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
        return 0;
    }
    
    // Return the position
    int position(lua_State *L) {
        DGPoint position = b->position();
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
        DGAction action;
        
        int type = (int)luaL_checknumber(L, 1);
        
        switch (type) {
            case FEED:
                action.type = DGActionFeed;
                action.cursor = DGCursorLook;                
                strncpy(action.feed, luaL_checkstring(L, 2), DGMaxFeedLength);
                
                if (lua_isstring(L, 3)) {
                    strncpy(action.feedAudio, lua_tostring(L, 3), DGMaxFileLength);
                    action.hasFeedAudio = true;
                }
                else action.hasFeedAudio = false;
                
                b->setAction(&action);
                
                break;
            case FUNCTION:
                if (!lua_isfunction(L, 2)) {
                    DGLog::getInstance().error(DGModScript, "%s", DGMsg250006);
                    return 0;
                }
                
                action.type = DGActionFunction;
                action.cursor = DGCursorUse;
                action.luaHandler = luaL_ref(L, LUA_REGISTRYINDEX); // Pop and return a reference to the table
                b->setAction(&action);
                
                break;                
            case SWITCH:
                int type = DGCheckProxy(L, 2);
                if (type == DGObjectNode)
                    action.target = DGProxyToNode(L, 2);
                else if (type == DGObjectRoom)
                    action.target = DGProxyToRoom(L, 2);
                else {
                    DGLog::getInstance().error(DGModScript, "%s", DGMsg250005);
                    return 0;
                }
                
                action.type = DGActionSwitch;
                action.cursor = DGCursorForward;                
                b->setAction(&action);
                
                break;
        }
                
        return 0;
    }
    
    // Set a cursor for the action
    int setCursor(lua_State *L) {
        if (b->hasAction()) {
            DGAction* action = b->action();
            
            action->cursor = luaL_checknumber(L, 1);
        }
        
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
        DGSize size = b->size();
        lua_pushnumber(L, size.width);
        lua_pushnumber(L, size.height);        
        return 2;
    }
    
    // Return the text
    int text(lua_State *L) {
        lua_pushstring(L, b->text());     
        return 1;
    }
    
    DGButton* ptr() { return b; }
    
private:
    DGButton* b;
    
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGButtonProxy::className[] = DGButtonProxyName;

Luna<DGButtonProxy>::RegType DGButtonProxy::methods[] = {
    DGObjectMethods(DGButtonProxy),      
    method(DGButtonProxy, move),
    method(DGButtonProxy, position),
    method(DGButtonProxy, scale),
    method(DGButtonProxy, setAction),
    method(DGButtonProxy, setCursor),
    method(DGButtonProxy, setFont),     
    method(DGButtonProxy, setImage),    
    method(DGButtonProxy, setPosition),
    method(DGButtonProxy, setSize),
    method(DGButtonProxy, setText),    
    method(DGButtonProxy, setTextColor),     
    method(DGButtonProxy, size),
    method(DGButtonProxy, text),       
    {0,0}
};

#endif // DG_BUTTONPROXY_H
