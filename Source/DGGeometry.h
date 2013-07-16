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

#ifndef DG_GEOMETRY_H
#define	DG_GEOMETRY_H

typedef struct {
	double x;
	double y;
} DGPoint;

typedef struct {
	double width;
	double height;
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

// Add functions to make point, size, etc.

#endif // DG_GEOMETRY_H
