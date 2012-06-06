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

#ifndef DG_CURSOR_H
#define DG_CURSOR_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////



enum DGCursorTypes {
    DGCursorNormal = 1,
    DGCursorDragging,
    DGCursorLeft,
    DGCursorRight,
    DGCursorUp,
    DGCursorDown,
    DGCursorUpperLeft,
    DGCursorUpperRight,
    DGCursorBottomLeft,
    DGCursorBottomRight,
    DGCursorForward,
    DGCursorBackward,
    DGCursorUse,
    DGCursorLook,
    DGCursorCustom
};

class DGTexture;

typedef struct {
    int type;
    DGTexture* image;
} DGCursorData;

////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////

class DGCursor {
    std::vector<DGCursorData> _arrayOfCursors;
    int _currentType;
    
public:
    DGCursor();
    ~DGCursor();
    
    DGTexture* current();
    int currentType();
    bool isLoaded(int type);
    void load(int type, const char* imageFromFile);
    void set(int type);
};

#endif // DG_CURSOR_H
