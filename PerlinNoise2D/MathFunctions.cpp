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

float fastDivide(float x, float y)
{
	const float magic = 1.0f / (1 << 20);

	const int sign = (x < 0) ^ (y < 0) ? -1 : 1;

	x = std::abs(x);
	y = std::abs(y);

	const float reciprocal = magic * y;

	return sign * x * reciprocal;
}

//////////////////////////////////

int fastFloor(double x)
{
	int xi = (int)x;

	return x < xi ? xi - 1 : xi;
}

//////////////////////////////////
