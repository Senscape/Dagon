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

#ifndef DAGON_GEOMETRY_H_
#define	DAGON_GEOMETRY_H_

typedef struct {
	double x;
	double y;
} Point;

typedef struct {
	double width;
	double height;
} Size;

typedef struct {
	Point origin;
	Size size;
} Rect;

typedef struct {
	double x;
	double y;
  double z;
} Vector;

// TODO: Add functions to make point, size, etc.

#endif // DAGON_GEOMETRY_H_
