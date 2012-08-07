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

#ifndef DG_SLIDEPROXY_H
#define DG_SLIDEPROXY_H

////////////////////////////////////////////////////////////
// NOTE: This header file should never be included directly.
// It's auto-included by DGProxy.h
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGControl.h"
#include "DGNode.h"
#include "DGScript.h"
#include "DGSpot.h"
#include "DGTexture.h"

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGSlideProxy : public DGObjectProxy {
public:
    static const char className[];
    static Luna<DGSlideProxy>::RegType methods[];
    
    // Constructor
    DGSlideProxy(lua_State *L) {
        n = new DGNode;
        n->setName(luaL_checkstring(L, 1));
        
        n->setType(DGObjectSlide);
        n->setSlide(true);
        
        if (lua_gettop(L) == 2)
            n->setDescription(luaL_checkstring(L, 2));
        
        // Image
        std::vector<int> arrayOfCoordinates;
        int coordsImage[] = {64, 424};
        arrayOfCoordinates.assign(coordsImage, coordsImage + 2);
        
        DGSpot* spot = new DGSpot(arrayOfCoordinates, DGNorth, DGSpotClass);
        
        DGTexture* texture = new DGTexture;
        texture->setResource(DGConfig::getInstance().path(DGPathRes, luaL_checkstring(L, 1), DGObjectImage));
        spot->setTexture(texture);
        
        n->addSpot(spot);
        
        // Back
        
        if (lua_toboolean(L, 3) != true) {
            int coordsBack[] = {0, 1280, 2048, 1280, 2048, 1624, 0, 1624};
            arrayOfCoordinates.assign(coordsBack, coordsBack + 8);
            spot = new DGSpot(arrayOfCoordinates, DGNorth, DGSpotClass);
            
            DGAction action;
            action.type = DGActionSwitch;
            action.cursor = DGCursorBackward;
            action.target = NULL; // Current node
            
            spot->setAction(&action);
            spot->setColor(0);
            
            n->addSpot(spot);
        }
        
        // Register the node in the controller (generates the bundle)
        DGControl::getInstance().registerObject(n);
        
        if (DGScript::getInstance().isExecutingModule()) {
            // A module is being executed, so we know a room
            // object of the same name exists
            
            lua_getglobal(L, DGScript::getInstance().module());
            DGRoom* r = DGProxyToRoom(L, -1);
            
            // We add the node automatically
            r->addNode(n);
        }
        
        // Init the base class
        this->setObject(n);
    }
    
    // Destructor
    ~DGSlideProxy() { delete n; }
    
    // Add a spot to the node
    int addSpot(lua_State *L) {
        if (DGCheckProxy(L, 1) == DGObjectSpot) {
            DGSpot* spot = DGProxyToSpot(L, 1);
            spot->setOrigin(64, 424);
            n->addSpot(spot);
            
            // Now we get the metatable of the added spot and set it
            // as a return value
            lua_getfield(L, LUA_REGISTRYINDEX, DGSpotProxyName);
            lua_setmetatable(L, -2);
            
            return 1;
        }
        
        return 0;
    }
    
    // Process a custom action
    int onReturn(lua_State *L) {
        if (lua_isfunction(L, 1)) {
            n->setSlideReturn(luaL_ref(L, LUA_REGISTRYINDEX));
        }
        
        return 0;
    }
    
    DGNode* ptr() { return n; }
    
private:
    DGNode* n;
};

////////////////////////////////////////////////////////////
// Static definitions
////////////////////////////////////////////////////////////

const char DGSlideProxy::className[] = DGSlideProxyName;

Luna<DGSlideProxy>::RegType DGSlideProxy::methods[] = {
    DGObjectMethods(DGSlideProxy),
    method(DGSlideProxy, addSpot),
    method(DGSlideProxy, onReturn),
    {0,0}
};

#endif // DG_SLIDEPROXY_H
