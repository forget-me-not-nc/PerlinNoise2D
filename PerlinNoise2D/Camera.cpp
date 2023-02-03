#include "Camera.h"

///////////////////////////////////
//
//
//////////////////////////////////

bool isMouseLeftButtonPressed = false;

///////////////////////////////////
//
//
//////////////////////////////////

Camera::Camera(glm::vec3 cameraPos, glm::vec3 worldUp)
{
	this->viewMat = glm::mat4(1.0f);

	this->speed = 1.25f;
	this->sensitivity = 7.0f;

	this->pos = cameraPos;
	this->worldUp = worldUp;
	this->up = worldUp;
	this->right = glm::vec3(0.0f);

	this->yaw = -90.0f;
	this->pitch = 0.0f;

	this->updateVectors();
}

//////////////////////////////////

Camera::~Camera()
{
}

//////////////////////////////////

void Camera::updateVectors()
{
	this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->front.y = sin(glm::radians(this->pitch));
	this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	this->front = glm::normalize(this->front);
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}

//////////////////////////////////

void Camera::updateInput(const float& dt, const DIRECTIONS dir, const double& offsetX, const double& offsetY)
{
	this->move(dt, dir);
	this->updateMouseInput(dt, offsetX, offsetY);
}

//////////////////////////////////

void Camera::updateMouseInput(const float& dt, const double& offsetX, const double& offsetY)
{
	if (isMouseLeftButtonPressed)
	{
		this->pitch += static_cast<GLfloat>(offsetY) * this->sensitivity * dt;
		this->yaw += static_cast<GLfloat>(offsetX) * this->sensitivity * dt;

		if (this->pitch >= 80.0f) this->pitch = 80.0f;
		if (this->pitch <= -80.0f) this->pitch = -80.0f;

		if (this->yaw > 360.0f || this->yaw < -360.0f) this->yaw = 0.0f;
	}
}

//////////////////////////////////

void Camera::move(const float& dt, const DIRECTIONS dir)
{
	switch (dir)
	{
	case DIRECTIONS::FORWARD:
	{
		this->pos += this->front * this->speed * dt;

		break;
	}

	case DIRECTIONS::BACKWARD:
	{
		this->pos -= this->front * this->speed * dt;

		break;
	}

	case DIRECTIONS::LEFT:
	{
		this->pos -= this->right * this->speed * dt;

		break;
	}

	case DIRECTIONS::RIGHT:
	{
		this->pos += this->right * this->speed * dt;

		break;
	}

	default: break;
	}
}

//////////////////////////////////

const glm::mat4 Camera::getVievMatrix()
{
	this->updateVectors();

	this->viewMat = glm::lookAt(
		this->pos,
		this->pos + this->front,
		this->up
	);

	return this->viewMat;
}

//////////////////////////////////

const glm::vec3 Camera::getCameraPosition()
{
	return this->pos;
}

//////////////////////////////////
