#include "PerlinNoiseOctaves.h"

///////////////////////////////////
//
//
//////////////////////////////////

PerlinNoiseOctaves::PerlinNoiseOctaves(
	unsigned int seed,
	int mult,
	WORLD_SIZE size,
	float harshness, 
	INTERPOLATION_TYPES interpolationType,
	int octaves,
	float amplitude,
	float persistence,
	float lacunarity)
	: PerlinNoise(seed, mult, size, harshness, interpolationType)
{

}

///////////////////////////////////
//
//
//////////////////////////////////

PerlinNoiseOctaves::~PerlinNoiseOctaves()
{
}

///////////////////////////////////

void PerlinNoiseOctaves::initVertices()
{
}
