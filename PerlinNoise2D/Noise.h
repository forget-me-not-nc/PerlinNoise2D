#pragma once

#include <cstdlib>
#include <ctime>
#include <gtc/noise.hpp>

#include "Vertex.h"
#include "Layers.h"

///////////////////////////////////
//
//
//////////////////////////////////

enum class WORLD_SIZE
{
	S_16x16 = 16,
	S_32x32 = 32,
	S_64x64 = 64,
};

enum class NOISE_TYPE
{
	RANDOM,
	PERLIN,
	PERLIN_OCTAVES
};

const glm::vec3	DEFAULT_V_COLOR = glm::vec3(0.0f, 0.67843f, 0.70980f);

///////////////////////////////////
//
//
//////////////////////////////////

class Noise
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

public:

	//////////////////////////////////

	~Noise();

	//////////////////////////////////
	
	virtual vector<Vertex> getVertices(GLenum polygonType);
	virtual void initVertices() = 0;
	virtual glm::vec3 getLightPos();

	void setAnimation(bool animation);
	void setPaintTerrainLayers(bool paint);

	///////////////////////////////////
	//
	//
	//////////////////////////////////

protected:

	//////////////////////////////////

	Noise(unsigned int seed, int multiplier, WORLD_SIZE size, float harshness);

	//////////////////////////////////

	void updateTime();

	//////////////////////////////////

	glm::vec3* data;
	glm::vec3 lightPos;

	///////////////////////////////////

	int multiplier;
	int size;
	int trueSize;
	float step;
	float harshness;

	bool animated;
	bool paintTerrainLayers;

	std::default_random_engine engine;

	float curTime;
	float dt;
	float prevTime;

private: 

	///////////////////////////////////
	//
	//
	//////////////////////////////////

	std::vector<Vertex> triangulateMesh();
	std::vector<Vertex> arrangeMeshAsQuads();
	
	void assignColorBasedOnLayer(Vertex& v);
};

///////////////////////////////////
//
//
//////////////////////////////////