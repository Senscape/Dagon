#include "DGGeometry.h"

inline DGPoint DGMakePoint(double x, double y)
{
	DGPoint point;
	point.x = x;
	point.y = y;

	return point;
}

inline DGSize DGMakeSize(double w, double h)
{
	DGSize size;
	size.width = w;
	size.height = h;

	return size;
}

inline DGRect DGMakeRect(double x, double y, double w, double h)
{
	DGRect rect;
	rect.origin.x = x;
	rect.origin.y = y;
	rect.size.width = w;
	rect.size.height = h;

	return rect;
}

const DGPoint DGZeroPoint = DGMakePoint(0.0, 0.0);
const DGSize DGZeroSize = DGMakeSize(0.0, 0.0);
const DGRect DGZeroRect = DGMakeRect(0.0, 0.0, 0.0, 0.0);

inline double DGMaxX(DGRect rect)
{
	return rect.origin.x + rect.size.width;
}

inline double DGMaxY(DGRect rect)
{
	return rect.origin.y + rect.size.height;
}

inline double DGMidX(DGRect rect)
{
	return rect.origin.x + rect.size.width * 0.5;
}

inline double DGMidY(DGRect rect)
{
	return rect.origin.y + rect.size.height * 0.5;
}

inline double DGMinX(DGRect rect)
{
	return rect.origin.x;
}

inline double DGMinY(DGRect rect)
{
	return rect.origin.y;
}

inline double DGWidth(DGRect rect)
{
	return rect.size.width;
}

inline double DGHeigth(DGRect rect)
{
	return rect.size.height;
}
