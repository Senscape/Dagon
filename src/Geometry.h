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
#define DAGON_GEOMETRY_H_

#include <cfloat>

#define kEpsilon FLT_EPSILON

namespace dagon {

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
  
typedef Point *PointPointer;
typedef Point *PointArray;
  
typedef Size *SizePointer;
typedef Size *SizeArray;
  
typedef Rect *RectPointer;
typedef Rect *RectArray;

Point MakePoint(double x, double y);
Size MakeSize(double w, double h);
Rect MakeRect(double x, double y, double w, double h);

extern const Point ZeroPoint;
extern const Size ZeroSize;
extern const Rect ZeroRect;

double MaxX(Rect rect);
double MaxY(Rect rect);
double MidX(Rect rect);
double MidY(Rect rect);
double MinX(Rect rect);
double MinY(Rect rect);
double Width(Rect rect);
double Height(Rect rect);
  
void MovePoint(Point& point, double offsetX, double offsetY);
void MoveRect(Rect& rect, double offsetX, double offsetY);
void ScaleRect(Rect& rect, double factor);
}

#endif // DAGON_GEOMETRY_H_
