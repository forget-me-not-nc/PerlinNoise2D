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
	this->octaves = octaves;
	this->amplitude = amplitude;
	this->persistence = persistence;
	this->lacunarity = lacunarity;

	this->initVertices();
}

///////////////////////////////////

PerlinNoiseOctaves::~PerlinNoiseOctaves()
{

}

///////////////////////////////////

double PerlinNoiseOctaves::noiseFBM(double x, double y, double z)
{
	double value = 0;
	double tAmplitude = this->amplitude;
	double tHarshness = this->harshness;
	double tPersistence = this->persistence;
	double tLacunarity = this->lacunarity;

	for (int i = 0; i < this->octaves; i++)
	{
		value += tAmplitude * this->noise(x * tHarshness, y * tHarshness, z * tHarshness);
		tAmplitude *= tPersistence;
		tHarshness *= tLacunarity;
	}

	return value;
}

///////////////////////////////////

void PerlinNoiseOctaves::initVertices()
{
	float currX = 0.0f;
	float currZ = 0.0f;

	for (int i = 0; i < this->size; i++)
	{
		currZ = 0.0f;

		for (int j = 0; j < this->size; j++)
		{
			this->data[i * this->size + j] = glm::vec3(
				currX,
				this->noiseFBM(currX, currZ, 0.99),
				currZ
			);

			currZ += this->step;
		}

		currX += this->step;
	}
}

///////////////////////////////////
