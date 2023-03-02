#include "Noise.h"

///////////////////////////////////
//
//
//////////////////////////////////

Noise::Noise(unsigned int seed, int multiplier, WORLD_SIZE size, float harshness)
{
	this->vertexColor = glm::vec3(0.113f, 0.647f, 0.172f);

	this->engine = std::default_random_engine(seed);

	srand(seed);

	this->trueSize = (int)size;
	this->size = ((int)size * multiplier) / ((int)size / 16);
	this->multiplier = multiplier;
	this->step = (1.0f / (float)(this->multiplier)) * ((this->trueSize / 16.0f));
	this->lightPos = glm::vec3(
		static_cast<int>(trueSize) / 2.0f,
		static_cast<int>(trueSize) / 3.5f,
		static_cast<int>(trueSize) / 2.0f
	);
	this->harshness = harshness;
	this->data = new glm::vec3[this->size * this->size];
}

///////////////////////////////////

Noise::~Noise()
{
	delete[] data;
}

//////////////////////////////////

vector<Vertex> Noise::getVertices()
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

		currV = data[indexCurr];
		nextV = data[indexNext];

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

	if(this->multiplier > 8) calculateNormals(v);

	return v;
}

//////////////////////////////////

glm::vec3 Noise::getLightPos()
{
	this->lightPos.y += this->harshness / 3.5f;

	return this->lightPos;
}

//////////////////////////////////

void Noise::calculateNormals(vector<Vertex>& vertices)
{
	for (unsigned int i = 2; i < vertices.size(); i += 3)
	{
		glm::vec3 v1 = vertices[i - 2].pos;
		glm::vec3 v2 = vertices[i - 1].pos;
		glm::vec3 v3 = vertices[i].pos;

		glm::vec3 polyNormal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

		if (glm::dot(polyNormal, glm::normalize(lightPos - v1)) < 0.0f) {
			polyNormal *= -1.0f;
		}

		vertices[i].normal = polyNormal;
		vertices[i - 1].normal = polyNormal;
		vertices[i - 2].normal = polyNormal;
	}
}

//////////////////////////////////

