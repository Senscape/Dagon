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

inline DGPoint DGMakePoint(double x, double y);
inline DGSize DGMakeSize(double w, double h);
inline DGRect DGMakeRect(double x, double y, double w, double h);

extern const DGPoint DGZeroPoint;
extern const DGSize DGZeroSize;
extern const DGRect DGZeroRect;

inline double DGMaxX(DGRect rect);
inline double DGMaxY(DGRect rect);
inline double DGMidX(DGRect rect);
inline double DGMidY(DGRect rect);
inline double DGMinX(DGRect rect);
inline double DGMinY(DGRect rect);
inline double DGWidth(DGRect rect);
inline double DGHeigth(DGRect rect);

// Add other geometry functions...

#endif // DG_GEOMETRY_H
