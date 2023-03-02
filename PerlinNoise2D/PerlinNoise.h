#pragma once
#include "Noise.h"

///////////////////////////////////
//
//
//////////////////////////////////

class PerlinNoise : public Noise
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

public:

	//////////////////////////////////
	
	PerlinNoise(unsigned int seed, int mult, WORLD_SIZE size, float harshness);
	~PerlinNoise();

	///////////////////////////////////
	//
	//
	//////////////////////////////////

private:
	
	//////////////////////////////////

	vector<int> permV;

	//////////////////////////////////

	virtual vector<Vertex> getVertices();
	virtual void initVertices();
	double noise(double x, double y, double z);
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
};

