#include "Mesh.h"

///////////////////////////////////
//
//
//////////////////////////////////

void Mesh::render(ShaderLoader* shader)
{
	this->updateUniform(shader);

	shader->use();

	///////////draw////////////////////

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, this->verticies.size());
}

//////////////////////////////////

void Mesh::updateVertecies(vector<Vertex>& data)
{
	this->setVertexData(data);
}

//////////////////////////////////

void Mesh::setScale(glm::vec3 scale)
{
	this->_scale = scale;
}

//////////////////////////////////

void Mesh::setPos(glm::vec3 pos)
{
	this->_position = pos;
}

//////////////////////////////////

void Mesh::setRotation(glm::vec3 rotation)
{
	this->_rotation = rotation;
}

//////////////////////////////////

void Mesh::scale(glm::vec3 scale)
{
	this->_scale += scale;
}

//////////////////////////////////

void Mesh::move(glm::vec3 pos)
{
	this->_position += pos;
}

//////////////////////////////////

void Mesh::rotate(glm::vec3 rotation)
{
	this->_rotation += rotation;
}

//////////////////////////////////

Mesh::Mesh(vector<Vertex>& data)
{
	this->setVertexData(data);
	this->initVAO();
	this->initModelMat();
}

//////////////////////////////////

Mesh::~Mesh()
{
	verticies.clear();

	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
}

//////////////////////////////////

void Mesh::setVertexData(vector<Vertex>& data)
{
	this->verticies = data;
}

//////////////////////////////////

void Mesh::initVAO()
{
	///////////working with VAO & VBO////////////////////

	glCreateVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->verticies.size() * sizeof(Vertex), this->verticies.data(), GL_DYNAMIC_DRAW);

	///////////assembly input////////////////////

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, pos));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

//////////////////////////////////

void Mesh::initModelMat()
{
	this->_rotation = glm::vec3(0.0f);
	this->_position = glm::vec3(0.0f);
	this->_scale = glm::vec3(1.0f);

	this->modelMat = glm::mat4(1.0f);

	this->modelMat = glm::translate(this->modelMat, this->_position);
	this->modelMat = glm::rotate(this->modelMat, glm::radians(this->_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	this->modelMat = glm::rotate(this->modelMat, glm::radians(this->_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	this->modelMat = glm::rotate(this->modelMat, glm::radians(this->_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	this->modelMat = glm::scale(this->modelMat, this->_scale);
}

//////////////////////////////////

void Mesh::updateUniform(ShaderLoader* shader)
{
	this->modelMat = glm::translate(this->modelMat, this->_position);
	this->modelMat = glm::rotate(this->modelMat, glm::radians(this->_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	this->modelMat = glm::rotate(this->modelMat, glm::radians(this->_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	this->modelMat = glm::rotate(this->modelMat, glm::radians(this->_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	this->modelMat = glm::scale(this->modelMat, this->_scale);

	this->_rotation = glm::vec3(0.0f);
	this->_position = glm::vec3(0.0f);
	this->_scale = glm::vec3(1.0f);

	shader->setMat4(this->modelMat, "modelMat");
}

//////////////////////////////////

