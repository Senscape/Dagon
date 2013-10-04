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

Point MakePoint(double x, double y) {
	Point point;
	point.x = x;
	point.y = y;
  
	return point;
}

Size MakeSize(double w, double h) {
	Size size;
	size.width = w;
	size.height = h;
  
	return size;
}

Rect MakeRect(double x, double y, double w, double h) {
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

double MaxX(Rect rect) {
	return rect.origin.x + rect.size.width;
}

double MaxY(Rect rect) {
	return rect.origin.y + rect.size.height;
}

double MidX(Rect rect) {
	return rect.origin.x + rect.size.width * 0.5;
}

double MidY(Rect rect) {
	return rect.origin.y + rect.size.height * 0.5;
}

double MinX(Rect rect) {
	return rect.origin.x;
}

double MinY(Rect rect) {
	return rect.origin.y;
}

double Width(Rect rect) {
	return rect.size.width;
}

double Height(Rect rect) {
	return rect.size.height;
}
  
void MovePoint(Point& point, double offsetX, double offsetY) {
  point.x += offsetX;
  point.y += offsetY;
}
  
void MoveRect(Rect& rect, double offsetX, double offsetY) {
  MovePoint(rect.origin, offsetX, offsetY);
}
  
void ScaleRect(Rect& rect, double factor) {
  rect.size.width *= factor;
  rect.size.height *= factor;
}
  
}
