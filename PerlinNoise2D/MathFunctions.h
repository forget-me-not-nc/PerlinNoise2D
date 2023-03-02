#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

///////////////////////////////////
//
//
//////////////////////////////////

enum class INTERPOLATION_TYPES
{
	NONE = 0,
	LINEAR = 1,
	COSINE = 2
};

///////////////////////////////////
//
//
//////////////////////////////////

extern double fade(double t);
extern double lerp(double t, double a, double b);
extern double cosine(double t, double a, double b);
extern double cubic(double a0, double a1, double a2, double a3, double t);
extern double hermite(double a0, double a1, double a2, double a3, double t);


///////////////////////////////////
//
//
//////////////////////////////////