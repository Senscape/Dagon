#include "DGGeometry.h"

#include <math.h>

DGPoint DGMakePoint(double x, double y)
{
	DGPoint point = { x, y };

	return point;
}

static inline DGPoint DGAddPoints(DGPoint point1, DGPoint point2)
{
	return DGMakePoint(point1.x + point2.x, point1.y + point2.y);
}

static inline DGPoint DGSubtractPoints(DGPoint point1, DGPoint point2)
{
	return DGMakePoint(point1.x - point2.x, point1.y - point2.y);
}

static inline DGPoint DGOffsetPoint(DGPoint point, double x, double y)
{
	return DGAddPoints(point, DGMakePoint(x, y));
}

static inline DGPoint DGReflectedPointAboutX(DGPoint point)
{
	return DGMakePoint(-point.x, point.y);
}

static inline DGPoint DGReflectedPointAboutY(DGPoint point)
{
	return DGMakePoint(point.x, -point.y);
}

static inline DGPoint DGReflectedPointAboutOrigin(DGPoint point)
{
	return DGMakePoint(-point.x, -point.y);
}

static inline DGPoint DGCartesianToPolar(DGPoint point)
{
	return DGMakePoint(sqrt(point.x * point.x + point.y * point.y), atan2(point.y, point.x));
}

static inline DGPoint DGPolarToCartesian(DGPoint point)
{
	return DGMakePoint(point.x * cos(point.y), point.x * sin(point.y));
}

DGSize DGSizeMake(double width, double height)
{
	DGSize size = { width, height };

	return size;
}

DGRect DGRectMake(double x, double y, double width, double height)
{
	DGRect rect = { x, y, width, height };

	return rect;
}

DGVector DGVectorMake(double x, double y, double z)
{
	DGVector vector = { x, y, z };

	return vector;
}
