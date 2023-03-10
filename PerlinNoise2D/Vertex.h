#pragma once

#include <glm.hpp>
#include <vec3.hpp> 
#include <mat4x4.hpp> 
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/normal.hpp>

#include "Includes.h"

///////////////////////////////////
//
//
//////////////////////////////////

struct Vertex
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;

	///////////////////////////////////

	Vertex() 
	{
		this->pos = glm::vec3(0.0f);
		this->color = glm::vec3(0.0f);
		this->normal = glm::vec3(0.0f);
	}

	Vertex(glm::vec3 pos, glm::vec3 color, glm::vec3 normal)
	{
		this->pos = pos;
		this->color = color;
		this->normal = normal;
	}
};

///////////////////////////////////
//
//
//////////////////////////////////