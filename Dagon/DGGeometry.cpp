////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012-2013 Senscape s.r.l.
// Copyright (c) 2013 Alexander Alekseev
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

#include "DGGeometry.h"

DGPoint DGMakePoint(double x, double y)
{
	DGPoint point;
	point.x = x;
	point.y = y;

	return point;
}

DGSize DGMakeSize(double w, double h)
{
	DGSize size;
	size.width = w;
	size.height = h;

	return size;
}

DGRect DGMakeRect(double x, double y, double w, double h)
{
	DGRect rect;
	rect.origin.x = x;
	rect.origin.y = y;
	rect.size.width = w;
	rect.size.height = h;

	return rect;
}

DGVector DGMakeVector(double x, double y, double z)
{
	DGVector v;
	v.x = x;
	v.y = y;
	v.z = z;

	return v;
}

const DGPoint DGZeroPoint = DGMakePoint(0.0, 0.0);
const DGSize DGZeroSize = DGMakeSize(0.0, 0.0);
const DGRect DGZeroRect = DGMakeRect(0.0, 0.0, 0.0, 0.0);
const DGVector DGZeroVector = DGMakeVector(0.0, 0.0, 0.0);

double DGMaxX(DGRect rect)
{
	return rect.origin.x + rect.size.width;
}

double DGMaxY(DGRect rect)
{
	return rect.origin.y + rect.size.height;
}

double DGMidX(DGRect rect)
{
	return rect.origin.x + rect.size.width * 0.5;
}

double DGMidY(DGRect rect)
{
	return rect.origin.y + rect.size.height * 0.5;
}

double DGMinX(DGRect rect)
{
	return rect.origin.x;
}

double DGMinY(DGRect rect)
{
	return rect.origin.y;
}

double DGWidth(DGRect rect)
{
	return rect.size.width;
}

double DGHeigth(DGRect rect)
{
	return rect.size.height;
}

void DGMovePoint(DGPoint& point, double offsetX, double offsetY)
{
	point.x += offsetX;
	point.y += offsetY;
}

void DGMoveRect(DGRect& rect, double offsetX, double offsetY)
{
	DGMovePoint(rect.origin, offsetX, offsetY);
}

void DGScaleRect(DGRect& rect, double factor)
{
	rect.size.width *= factor;
	rect.size.height *= factor;
}
