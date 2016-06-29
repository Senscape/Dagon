////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
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
  
typedef Vector *VectorPointer;
typedef Vector *VectorArray;

// Makes a point with x and y coordinates.
Point MakePoint(double x, double y);
// Makes size with width and height parameters.
Size MakeSize(double w, double h);
// Makes a rectangle with origin in (x, y)-point with (w, h)-size.
Rect MakeRect(double x, double y, double w, double h);
// Makes a vector.
Vector MakeVector(double x, double y, double z);
  
extern const Point ZeroPoint;
extern const Size ZeroSize;
extern const Rect ZeroRect;
extern const Vector ZeroVector;

// Returns a max rectangle's x-coordinate.
double MaxX(Rect rect);
// Returns a max rectangle's y-coordinate.
double MaxY(Rect rect);
// Returns a middle rectangle's x-coordinate.
double MidX(Rect rect);
// Returns a middle rectangle's y-coordinate.
double MidY(Rect rect);
// Returns a min rectangle's x-coordinate.
double MinX(Rect rect);
// Returns a min rectangle's y-coordinate.
double MinY(Rect rect);
// Returns a rectangle's width.
double Width(Rect rect);
// Returns a rectangle's height.
double Height(Rect rect);
  
void MovePoint(Point& point, double offsetX, double offsetY);
void MoveRect(Rect& rect, double offsetX, double offsetY);
void ScaleRect(Rect& rect, double factor);
}

#endif // DAGON_GEOMETRY_H_
