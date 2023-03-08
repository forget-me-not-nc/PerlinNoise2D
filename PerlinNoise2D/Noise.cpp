#include "Noise.h"

///////////////////////////////////
//
//
//////////////////////////////////

Noise::Noise(unsigned int seed, int multiplier, WORLD_SIZE size, float harshness)
{
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
	this->animated = false;
	this->data = new glm::vec3[this->size * this->size];
	this->curTime = 0.0f;
	this->dt = 0.0f;
	this->prevTime = 0.0f;
}

void Noise::updateTime()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->prevTime;
	this->prevTime = this->curTime;
}

///////////////////////////////////

Noise::~Noise()
{
	delete[] data;
}

//////////////////////////////////

vector<Vertex> Noise::getVertices(GLenum polygonType)
{
	switch (polygonType)
	{
		case GL_TRIANGLES:
		{
			return triangulateMesh();

			break;
		}

		case GL_QUADS:
		{
			return arrangeMeshAsQuads();

			break;
		}

		default: 
		{
			return vector<Vertex>();

			break; 
		}
	}
}

//////////////////////////////////

glm::vec3 Noise::getLightPos()
{
	this->lightPos.y += (this->harshness / 3.5f);

	return this->lightPos;
}

//////////////////////////////////

void Noise::setAnimation(bool animation)
{
	this->animated = animation;
}

///////////////////////////////////

void Noise::setPaintTerrainLayers(bool paint)
{
	this->paintTerrainLayers = paint;
}

///////////////////////////////////

std::vector<Vertex> Noise::triangulateMesh()
{
	std::vector<Vertex> v;

	int numVertices = (this->size - 1) * (this->size - 1) * 6;
	v.resize(numVertices);

	int sizeMinusOne = this->size - 1;
	int sizeSquared = this->size * this->size;

	glm::vec3 currV;
	glm::vec3 nextV;
	glm::vec3 diagV;

	int indexStep = 0;

	for (int i = 0; i < sizeMinusOne; i++)
	{
		for (int j = 0; j < sizeMinusOne; j++)
		{
			int leftUpIndex = i * this->size + j;
			int leftDownIndex = leftUpIndex + this->size;
			int rightUpIndex = leftUpIndex + 1;
			int rightDownIndex = leftDownIndex + 1;

			currV = data[leftUpIndex];
			nextV = data[rightUpIndex];
			diagV = data[rightDownIndex];

			v[indexStep + 0] = Vertex(currV, DEFAULT_V_COLOR, glm::normalize(glm::cross(nextV - currV, diagV - currV)));
			v[indexStep + 1] = Vertex(nextV, DEFAULT_V_COLOR, glm::normalize(glm::cross(diagV - nextV, currV - nextV)));
			v[indexStep + 2] = Vertex(diagV, DEFAULT_V_COLOR, glm::normalize(glm::cross(currV - diagV, nextV - diagV)));

			currV = data[rightDownIndex];
			nextV = data[leftDownIndex];
			diagV = data[leftUpIndex];

			v[indexStep + 3] = Vertex(currV, DEFAULT_V_COLOR, glm::normalize(glm::cross(nextV - currV, diagV - currV)));
			v[indexStep + 4] = Vertex(nextV, DEFAULT_V_COLOR, glm::normalize(glm::cross(diagV - nextV, currV - nextV)));
			v[indexStep + 5] = Vertex(diagV, DEFAULT_V_COLOR, glm::normalize(glm::cross(currV - diagV, nextV - diagV)));

			if (this->paintTerrainLayers)
			{
				assignColorBasedOnLayer(v[indexStep + 0]);
				assignColorBasedOnLayer(v[indexStep + 1]);
				assignColorBasedOnLayer(v[indexStep + 2]);
				assignColorBasedOnLayer(v[indexStep + 3]);
				assignColorBasedOnLayer(v[indexStep + 4]);
				assignColorBasedOnLayer(v[indexStep + 5]);
			}

			indexStep += 6;
		}
	}

	return v;
}

///////////////////////////////////

std::vector<Vertex> Noise::arrangeMeshAsQuads()
{
	std::vector<Vertex> v;

	int numVertices = this->size * this->size * 4;
	v.resize(numVertices);

	int index = 0;

	for (int i = 0; i < this->size - 1; i++)
	{
		for (int j = 0; j < this->size - 1; j++)
		{
			glm::vec3 currV = data[i * size + j];
			glm::vec3 nextV = data[i * size + j + 1];
			glm::vec3 prevV = data[(i + 1) * size + j];
			glm::vec3 diagV = data[(i + 1) * size + j + 1];

			glm::vec3 normal = glm::normalize(glm::cross(nextV - currV, diagV - currV));

			v[index++] = Vertex(currV, DEFAULT_V_COLOR, normal);
			if (paintTerrainLayers) assignColorBasedOnLayer(v[index - 1]);

			v[index++] = Vertex(nextV, DEFAULT_V_COLOR, normal);
			if (paintTerrainLayers) assignColorBasedOnLayer(v[index - 1]);

			v[index++] = Vertex(diagV, DEFAULT_V_COLOR, normal);
			if (paintTerrainLayers) assignColorBasedOnLayer(v[index - 1]);

			v[index++] = Vertex(prevV, DEFAULT_V_COLOR, normal);
			if (paintTerrainLayers) assignColorBasedOnLayer(v[index - 1]);
		}
	}

	return v;
}

///////////////////////////////////

void Noise::assignColorBasedOnLayer(Vertex& v)
{
	if (v.pos.y <= SEE_LAYER)
	{
		v.color = seeLayer.color;
	}
	else if (v.pos.y > SEE_LAYER && v.pos.y <= PLANES_LAYER)
	{
		v.color = planeLayer.color;
	}
	else if (v.pos.y > PLANES_LAYER)
	{
		v.color = hillsLayer.color;
	}
}

///////////////////////////////////

