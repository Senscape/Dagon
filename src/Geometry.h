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

inline Point MakePoint(double x, double y);
inline Size MakeSize(double w, double h);
inline Rect MakeRect(double x, double y, double w, double h);

extern const Point ZeroPoint;
extern const Size ZeroSize;
extern const Rect ZeroRect;

inline double MaxX(Rect rect);
inline double MaxY(Rect rect);
inline double MidX(Rect rect);
inline double MidY(Rect rect);
inline double MinX(Rect rect);
inline double MinY(Rect rect);
inline double Width(Rect rect);
inline double Height(Rect rect);
}

#endif // DAGON_GEOMETRY_H_
