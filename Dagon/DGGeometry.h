////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2012 Senscape s.r.l.
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
	double x;
	double y;
} DGPoint;

typedef DGPoint *DGPointPointer;
typedef DGPoint *DGPointArray;

typedef struct {
	double width;
	double height;
} DGSize;

typedef DGSize *DGSizePointer;
typedef DGSize *DGSizeArray;

typedef struct {
	DGPoint origin;
	DGSize size;
} DGRect;

typedef DGRect *DGRectPointer;
typedef DGRect *DGRectArray;

typedef struct {
	double x;
	double y;
    double z;
} DGVector;

typedef DGVector *DGVectorPointer;
typedef DGVector *DGVectorArray;

// Makes a point with x and y coordinates.
DGPoint DGMakePoint(double x, double y);
// Makes size with width and height parameters.
DGSize DGMakeSize(double w, double h);
// Makes a rectangle with origin in (x, y)-point with (w, h)-size.
DGRect DGMakeRect(double x, double y, double w, double h);
// Makes a vector.
DGVector DGMakeVector(double x, double y, double z);

extern const DGPoint DGZeroPoint;
extern const DGSize DGZeroSize;
extern const DGRect DGZeroRect;
extern const DGVector DGZeroVector;

// Returns a max rectangle's x-coordinate.
double DGMaxX(DGRect rect);
// Returns a max rectangle's y-coordinate.
double DGMaxY(DGRect rect);
// Returns a middle rectangle's x-coordinate.
double DGMidX(DGRect rect);
// Returns a middle rectangle's y-coordinate.
double DGMidY(DGRect rect);
// Returns a min rectangle's x-coordinate.
double DGMinX(DGRect rect);
// Returns a min rectangle's y-coordinate.
double DGMinY(DGRect rect);
// Returns a rectangle's width.
double DGWidth(DGRect rect);
// Returns a rectangle's height.
double DGHeigth(DGRect rect);

void DGMovePoint(DGPoint& point, double offsetX, double offsetY);
void DGMoveRect(DGRect& rect, double offsetX, double offsetY);
void DGScaleRect(DGRect& rect, double factor);

// Add other geometry functions...

#endif // DG_GEOMETRY_H
