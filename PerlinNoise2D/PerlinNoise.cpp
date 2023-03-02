#include "PerlinNoise.h"

///////////////////////////////////
//
//
//////////////////////////////////

PerlinNoise::PerlinNoise(unsigned int seed, int mult, WORLD_SIZE size, float harshness, INTERPOLATION_TYPES interpolationType) 
	: Noise(seed, mult, size, harshness)
{
	permV.reserve(256 * 2);
	permV.resize(256);

	int index = 0;

	std::generate(permV.begin(), permV.end(), [&index] { return index++; });
	std::shuffle(permV.begin(), permV.end(), this->engine);

	permV.insert(permV.end(), permV.begin(), permV.end());

	this->interpolationType = interpolationType;

	this->initVertices();
}

//////////////////////////////////

vector<Vertex> PerlinNoise::getVertices(GLenum polygonType)
{
	//float time = (float)(glfwGetTime() / 150.0f);

	//float currX = 0.0f;
	//float currZ = 0.0f;

	//for (int i = 0; i < this->size; i++)
	//{
	//	currZ = 0.0f;

	//	for (int j = 0; j < this->size; j++)
	//	{
	//		this->data[i * this->size + j] = glm::vec3(
	//			currX,
	//			this->noise(currX * time, currZ * time, 0.99f),
	//			currZ
	//		);

	//		currZ += this->step;
	//	}

	//	currX += this->step;
	//}

	return Noise::getVertices(polygonType);
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
			this->data[i * this->size + j] = glm::vec3(
				currX,
				this->noise(currX * this->harshness, currZ * this->harshness, 0.99 * this->harshness),
				currZ
			);

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

	double gAA = grad(permV[AA], x, y, z);
	double gBA = grad(permV[BA], x - 1, y, z);
	double gAB = grad(permV[AB], x, y - 1, z);
	double gBB = grad(permV[BB], x - 1, y - 1, z);
	double gAA1 = grad(permV[AA + 1], x, y, z - 1);
	double gBA1 = grad(permV[BA + 1], x - 1, y, z - 1);
	double gAB1 = grad(permV[AB + 1], x, y - 1, z - 1);
	double gBB1 = grad(permV[BB + 1], x - 1, y - 1, z - 1);

	switch (interpolationType)
	{
		case INTERPOLATION_TYPES::LINEAR:
		{
			double l0 = lerp(
				v,
				lerp(u, gAA, gBA),
				lerp(u, gAB, gBB)
			);

			double l1 = lerp(
				v,
				lerp(u, gAA1, gBA1),
				lerp(u, gAB1, gBB1)
			);

			double l2 = lerp(w, l0, l1);

			return l2;

			break;
		}

		case INTERPOLATION_TYPES::COSINE:
		{
			double l0 = cosine(
				cosine(gAA, gBA, u),
				cosine(gAB, gBB, u),
				v
			);

			double l1 = cosine(
				cosine(gAA1, gBA1, u),
				cosine(gAB1, gBB1, u),
				v
			);

			double l2 = cosine(l0, l1, w);

			return l2;

			break;
		}

		default:
		{
			return (u + v + w);

			break;
		}
	}
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

//////////////////////////////////


