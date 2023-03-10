#include "RandomNoise.h"

///////////////////////////////////
//
//
//////////////////////////////////

RandomNoise::RandomNoise(unsigned int seed, int multiplier, WORLD_SIZE size, float harshness) 
	: Noise(seed, multiplier, size, harshness)
{
	this->initVertices();
}

//////////////////////////////////

RandomNoise::~RandomNoise()
{
}

//////////////////////////////////

vector<Vertex> RandomNoise::getVertices(GLenum polygonType)
{
	if (this->animated)
	{
		this->updateTime();

		float currX = 0.0f;
		float currZ = 0.0f;

		for (int i = 0; i < this->size; i++)
		{
			currZ = 0.0f;

			for (int j = 0; j < this->size; j++)
			{
				this->data[i * this->size + j] = glm::vec3(currX, this->getRandomFloat(0.0f, this->harshness + this->dt), currZ);

				currZ += this->step;
			}

			currX += this->step;
		}
	}

	return Noise::getVertices(polygonType);
}

//////////////////////////////////

void RandomNoise::initVertices()
{
	float currX = 0.0f;
	float currZ = 0.0f;

	for (int i = 0; i < this->size; i++)
	{
		currZ = 0.0f;

		for (int j = 0; j < this->size; j++)
		{
			this->data[i * this->size + j] = glm::vec3(currX, this->getRandomFloat(0.0f, this->harshness), currZ);

			currZ += this->step;
		}

		currX += this->step;
	}
}

//////////////////////////////////

float RandomNoise::getRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> distribution(min, max);

	return distribution(this->engine);
}

//////////////////////////////////
