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
extern float fastDivide(float v1, float v2);
extern int fastFloor(double x);

///////////////////////////////////
//
//
//////////////////////////////////