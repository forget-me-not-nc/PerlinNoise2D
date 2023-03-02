#include "PerlinNoise.h"

///////////////////////////////////
//
//
//////////////////////////////////

PerlinNoise::PerlinNoise(unsigned int seed, int mult, WORLD_SIZE size, float harshness) 
	: Noise(seed, mult, size, harshness)
{
	permV.reserve(256 * 2);
	permV.resize(256);

	int index = 0;

	std::generate(permV.begin(), permV.end(), [&index] { return index++; });
	std::shuffle(permV.begin(), permV.end(), this->engine);

	permV.insert(permV.end(), permV.begin(), permV.end());

	this->initVertices();
}

//////////////////////////////////

vector<Vertex> PerlinNoise::getVertices()
{
	float time = (float)(glfwGetTime() / 50.0f);

	float currX = 0.0f;
	float currZ = 0.0f;

	for (int i = 0; i < this->size; i++)
	{
		currZ = 0.0f;

		//todo fomod(harshiness)

		for (int j = 0; j < this->size; j++)
		{
			this->data[i * this->size + j] = glm::vec3(currX, this->noise(currX * time, currZ * time, 0.99 * time), currZ);

			currZ += this->step;
		}

		currX += this->step;
	}

	return Noise::getVertices();
}

//////////////////////////////////

void PerlinNoise::initVertices()
{
	float currX = 0.0f;
	float currZ = 0.0f;

	for (int i = 0; i < this->size; i++)
	{
		currZ = 0.0f;

		for (int j = 0; j < this->size; j++)
		{
			this->data[i * this->size + j] = glm::vec3(currX, this->noise(currX, currZ, 0.99), currZ);

			currZ += this->step;
		}

		currX += this->step;
	}
}

//////////////////////////////////

double PerlinNoise::noise(double x, double y, double z)
{
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	int A = permV[X] + Y;
	int AA = permV[A] + Z;
	int AB = permV[A + 1] + Z;
	int B = permV[X + 1] + Y;
	int BA = permV[B] + Z;
	int BB = permV[B + 1] + Z;

	double res = lerp(
		-w, 
		lerp(
			v,
			lerp(
				u,
				grad(permV[AA], x, y, z),
				grad(permV[BA], x - 1, y, z)
			),
			lerp(
				u, 
				grad(permV[AB], x, y - 1, z), 
				grad(permV[BB], x - 1, y - 1, z)
			)
		),
		lerp(
			v, 
			lerp(
				u,
				grad(permV[AA + 1], x, y, z - 1),
				grad(permV[BA + 1], x - 1, y, z - 1)
			),
			lerp(
				u,
				grad(permV[AB + 1], x, y - 1, z - 1),
				grad(permV[BB + 1], x - 1, y - 1, z - 1)
			)
		)
	);
	
	return (res * this->harshness);
}

//////////////////////////////////

double PerlinNoise::fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

//////////////////////////////////

double PerlinNoise::lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

//////////////////////////////////

double PerlinNoise::grad(int hash, double x, double y, double z)
{
	int h = hash & 15;
	
	double u = h < 8 ? x : y;
	double v = h < 4 ? y :( h == 12 || h == 14) ? x : z;

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

//////////////////////////////////

PerlinNoise::~PerlinNoise()
{
}
