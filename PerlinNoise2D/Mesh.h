#pragma once

#include "ShaderLoader.h"

///////////////////////////////////
//
//
//////////////////////////////////

class Mesh
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

public:

	//////////////////////////////////

	void render(ShaderLoader* shader);
	void updateVertecies(vector<Vertex>& data);

	void setScale(glm::vec3 scale);
	void setPos(glm::vec3 pos);
	void setRotation(glm::vec3 rotation);

	void scale(glm::vec3 scale);
	void move(glm::vec3 pos);
	void rotate(glm::vec3 rotation);

	Mesh(vector<Vertex>& data);
	~Mesh();

	///////////////////////////////////
	//
	//
	//////////////////////////////////

private:

	//////////////////////////////////

	vector<Vertex> verticies;

	GLuint VAO;
	GLuint VBO;

	glm::vec3 _scale;
	glm::vec3 _rotation;
	glm::vec3 _position;

	glm::mat4 modelMat;

	//////////////////////////////////

	void setVertexData(vector<Vertex>& data);
	void initVAO();
	void initModelMat();
	void updateUniform(ShaderLoader* shader);
};

///////////////////////////////////
//
//
//////////////////////////////////
