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

DGPoint DGMakePoint(double x, double y);
DGSize DGMakeSize(double w, double h);
DGRect DGMakeRect(double x, double y, double w, double h);

extern const DGPoint DGZeroPoint;
extern const DGSize DGZeroSize;
extern const DGRect DGZeroRect;

double DGMaxX(DGRect rect);
double DGMaxY(DGRect rect);
double DGMidX(DGRect rect);
double DGMidY(DGRect rect);
double DGMinX(DGRect rect);
double DGMinY(DGRect rect);
double DGWidth(DGRect rect);
double DGHeigth(DGRect rect);

void DGMovePoint(DGPoint& point, double offsetX, double offsetY);
void DGMoveRect(DGRect& rect, double offsetX, double offsetY);
void DGScaleRect(DGRect& rect, double factor);

// Add other geometry functions...

#endif // DG_GEOMETRY_H
