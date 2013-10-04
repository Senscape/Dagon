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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "Geometry.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

inline Point MakePoint(double x, double y) {
	Point point;
	point.x = x;
	point.y = y;
  
	return point;
}

inline Size MakeSize(double w, double h) {
	Size size;
	size.width = w;
	size.height = h;
  
	return size;
}

inline Rect MakeRect(double x, double y, double w, double h) {
	Rect rect;
	rect.origin.x = x;
	rect.origin.y = y;
	rect.size.width = w;
	rect.size.height = h;
  
	return rect;
}

const Point ZeroPoint = MakePoint(0.0, 0.0);
const Size ZeroSize = MakeSize(0.0, 0.0);
const Rect ZeroRect = MakeRect(0.0, 0.0, 0.0, 0.0);

inline double MaxX(Rect rect) {
	return rect.origin.x + rect.size.width;
}

inline double MaxY(Rect rect) {
	return rect.origin.y + rect.size.height;
}

inline double MidX(Rect rect) {
	return rect.origin.x + rect.size.width * 0.5;
}

inline double MidY(Rect rect) {
	return rect.origin.y + rect.size.height * 0.5;
}

inline double MinX(Rect rect) {
	return rect.origin.x;
}

inline double MinY(Rect rect) {
	return rect.origin.y;
}

inline double Width(Rect rect) {
	return rect.size.width;
}

inline double Height(Rect rect) {
	return rect.size.height;
}
  
}
