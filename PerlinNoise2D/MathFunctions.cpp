#include "MathFunctions.h"

///////////////////////////////////
//
//
//////////////////////////////////

double fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

//////////////////////////////////

double lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

//////////////////////////////////

double cosine(double a, double b, double t)
{
	double t2 = (1.0 - cos(t * M_PI)) / 2.0;

	return a * (1 - t2) + b * t2;
}

//////////////////////////////////

double cubic(double a0, double a1, double a2, double a3, double t)
{
	double t2 = t * t;
	double v0 = a3 - a2 - a0 + a1;
	double v1 = a0 - a1 - a0;
	double v2 = a2 - a0;
	double v3 = a1;

	return(v0 * t * t2 + v1 * t2 + v2 * t + v3);
}

//////////////////////////////////

double hermite(double a0, double a1, double a2, double a3, double t)
{
	double t2 = t * t;
	double v0 = (-0.5 * a0) + (1.5 * a1) - (1.5 * a2) + (0.5 * a3);
	double v1 = a0 - (2.5 * a1) + (2 * a2) - (0.5 * a3);
	double v2 = (-0.5 * a0) + (0.5 * a2);
	double v3 = a1;

	return(v0 * t * t2 + v1 * t2 + v2 * t + v3);
}