#pragma once

#include "PerlinNoise.h"

///////////////////////////////////
//
//
//////////////////////////////////

class PerlinNoiseOctaves : public PerlinNoise
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

public:

	//////////////////////////////////

	PerlinNoiseOctaves(
		unsigned int seed,
		int mult,
		WORLD_SIZE size,
		float harshness,
		INTERPOLATION_TYPES interpolationType,
		int octaves,
		float amplitude,
		float persistence,
		float lacunarity
	);

	~PerlinNoiseOctaves();


	///////////////////////////////////
	//
	//
	//////////////////////////////////

private:

	///////////////////////////////////

	int octaves;
	int amplitude;
	int persistence;
	int lacunarity;
	
	///////////////////////////////////

	virtual void initVertices();
};
