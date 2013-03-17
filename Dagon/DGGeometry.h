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

#include <float.h>

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

/*
 *	DGPoint
 */
DGPoint DGMakePoint(double x, double y);

const DGPoint DGZeroPoint = DGMakePoint(0.0, 0.0);
const DGPoint DGFarAwayPoint = DGMakePoint(DBL_MAX, DBL_MAX);

static inline DGPoint DGAddPoints(DGPoint point1, DGPoint point2);
static inline DGPoint DGSubtractPoints(DGPoint point1, DGPoint point2);
static inline DGPoint DGOffsetPoint(DGPoint point, double x, double y);
static inline DGPoint DGReflectedPointAboutX(DGPoint point);
static inline DGPoint DGReflectedPointAboutY(DGPoint point);
static inline DGPoint DGReflectedPointAboutOrigin(DGPoint point);
static inline DGPoint DGCartesianToPolar(DGPoint point);
static inline DGPoint DGPolarToCartesian(DGPoint point);

/*
 *	DGSize
 */

DGSize DGSizeMake(double width, double height);

const DGSize DGSizeZero = DGSizeMake(0.0, 0.0);

/*
 *	DGRect
 */

DGRect DGRectMake(double x, double y, double width, double height);

const DGRect DGRectZero = DGRectMake(0.0, 0.0, 0.0, 0.0);

/*
 *	DGVector
 */

DGVector DGVectorMake(double x, double y, double z);

const DGVector DGVectorZero = DGVectorMake(0.0, 0.0, 0.0);

#endif // DG_GEOMETRY_H
