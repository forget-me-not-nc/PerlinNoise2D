#include "RandomNoise.h"

///////////////////////////////////
//
//
//////////////////////////////////

RandomNoise::RandomNoise(unsigned int seed, int multiplier, WORLD_SIZE size) 
	: Noise(seed, multiplier, size)
{
	this->initVerticies();
}

//////////////////////////////////

RandomNoise::~RandomNoise()
{
}

//////////////////////////////////

void RandomNoise::initVerticies()
{
	float step = 1.0f / (float)this->multiplier;

	float currX = 0.0f;
	float currZ = 0.0f;

	for (int i = 0; i < this->size; i++)
	{
		currZ = 0.0f;

		for (int j = 0; j < this->size; j++)
		{
			this->terrain[i * this->size + j] = glm::vec3(currX, this->getRandomFloat(this->NOISE_MAX), currZ);

			currZ += step;
		}

		currX += step;
	}
}
