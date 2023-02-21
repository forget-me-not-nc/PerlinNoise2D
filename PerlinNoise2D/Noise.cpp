#include "Noise.h"

///////////////////////////////////
//
//
//////////////////////////////////

Noise::Noise(unsigned int seed, int multiplier, WORLD_SIZE size)
{
	this->vertexColor = glm::vec3(0.113f, 0.647f, 0.172f);

	srand(seed);

	this->size = (int)size * multiplier;
	this->multiplier = multiplier;

	this->terrain = new glm::vec3[this->size * this->size];
}

///////////////////////////////////

Noise::~Noise()
{
	delete[] terrain;
}

//////////////////////////////////

vector<Vertex> Noise::getVertecies()
{
	vector<Vertex> v;
	v.reserve(this->size * this->size);

	glm::vec3 currV;
	glm::vec3 nextV;
	
	int i = 0;
	int j = -1;

	bool forward = true;
	bool first = true;
	bool end = false;

	while (true)
	{
		if (i == this->size - 1) break;

		if (forward && !end)
		{
			j++;
		}
		else if(!forward && !end)
		{
			j--;
		}

		int indexCurr = i * this->size + j;
		int indexNext = (i + 1) * this->size + j;

		currV = terrain[indexCurr];
		nextV = terrain[indexNext];

		v.push_back(
			Vertex(currV, this->vertexColor, glm::vec3(0.0f, 1.0f, 0.0f))
		);

		v.push_back(
			Vertex(nextV, this->vertexColor, glm::vec3(0.0f, 1.0f, 0.0f))
		);

		if (j == this->size - 1)
		{
			if (!end)
			{
				end = true;

				i++;

				continue;
			}

			forward = false;

			end = false;
		}

		if (!first)
		{
			if (j == 0)
			{
				if (!end)
				{
					end = true;

					i++;

					continue;
				}

				forward = true;

				end = false;
			}
		}
		else
		{
			first = false;
		}
	}

	return v;
}

//////////////////////////////////

float Noise::getRandomFloat(float max)
{
	return (float(rand()) / float((RAND_MAX)) * max);
}

//////////////////////////////////
