#pragma once

#include <iostream>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <vec3.hpp> 
#include <mat4x4.hpp> 
#include <gtc/matrix_transform.hpp>

///////////////////////////////////
//
//
//////////////////////////////////

enum class DIRECTIONS
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	NONE
};

///////////////////////////////////
//
//
//////////////////////////////////

extern bool isMouseLeftButtonPressed;

///////////////////////////////////
//
//
//////////////////////////////////

class Camera
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

public:

	//////////////////////////////////

	Camera(glm::vec3 cameraPos, glm::vec3 worldUp);
	~Camera();

	void updateInput(const float& dt, const DIRECTIONS dir, const double& offsetX, const double& offsetY);
	void move(const float& dt, const DIRECTIONS dir);
	void setCameraPosition(glm::vec3 pos);

	const glm::mat4 getVievMatrix();
	const glm::vec3 getCameraPosition();

	///////////////////////////////////
	//
	//
	//////////////////////////////////

private:

	//////////////////////////////////

	glm::mat4 viewMat;

	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 worldUp;

	GLfloat pitch;
	GLfloat yaw;

	GLfloat sensitivity;
	GLfloat speed;

	//////////////////////////////////

	void updateVectors();
	void updateMouseInput(const float& dt, const double& offsetX, const double& offsetY);
};

///////////////////////////////////
//
//
//////////////////////////////////

