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

#ifndef DG_GEOMETRY_H
#define	DG_GEOMETRY_H

typedef struct {
	int x;
	int y;
} DGPoint;

typedef struct {
	int width;
	int height;
} DGSize;

typedef struct {
	DGPoint origin;
	DGSize size;
} DGRect;

typedef struct {
	double x;
	double y;
    double z;
} DGVector;

#endif // DG_GEOMETRY_H
