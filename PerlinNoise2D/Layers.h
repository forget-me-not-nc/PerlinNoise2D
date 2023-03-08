#pragma once

#include <glm.hpp>
#include <vec3.hpp> 

///////////////////////////////////
//
//
//////////////////////////////////

const float SEE_LAYER = -0.1f;
const float PLANES_LAYER = 1.2f;
const float HILLS_LAYER = 10000.0f;

///////////////////////////////////
//
//
//////////////////////////////////

struct Layer
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

	float level;
	glm::vec3 color;

	//////////////////////////////////

	Layer(float level, glm::vec3 color)
	{
		this->color = color;
		this->level = level;
	};

	~Layer() {};
};

///////////////////////////////////
//
//
//////////////////////////////////

const Layer seeLayer(SEE_LAYER, glm::vec3(0.00000f, 0.25882f, 0.70980f));
const Layer planeLayer(PLANES_LAYER, glm::vec3(0.00000f, 0.70980f, 0.21176f));
const Layer hillsLayer(HILLS_LAYER, glm::vec3(0.31765f, 0.34118f, 0.32549f));