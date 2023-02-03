#pragma once

#include <Windows.h>
#include <CommCtrl.h>

#include "Mesh.h"
#include "Camera.h"
#include "Settings.h"

///////////////////////////////////
//
//
//////////////////////////////////

#define DISPLAY_BUTTON 1000
#define GRID_X_BOX 1001
#define GRID_Y_BOX 1002
#define DISPLAY_TYPE_BOX 1003
#define INTERPOLATION_TYPE_BOX 1004

///////////////////////////////////
//
//
//////////////////////////////////

void frameBufferResize(GLFWwindow* window, int bWidth, int bHeight);
void createControlls(HWND hWnd, HINSTANCE instance);

///////////////////////////////////
//
//
//////////////////////////////////

const Vertex sample[] =
{
	glm::vec3(0.0f, 0.5f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.5f, -0.5f, 0.5f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(-0.5f, -0.5f, 0.5f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),

	glm::vec3(0.0f, 0.5f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-0.5f, -0.5f, 0.5f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),

	glm::vec3(0.0f, 0.5f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(0.0f, 0.0f, -1.0f),

	glm::vec3(0.0f, 0.5f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.5f, -0.5f, 0.5f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f)
};

const int vCount = sizeof(sample) / sizeof(Vertex);

///////////////////////////////////
//
//
//////////////////////////////////

class Program
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

public:

	//////////////////////////////////

	Program(
		const char* title,
		bool resizable,
		GLFWimage images[]
	);
	~Program();

	int getWindowShouldClose();

	void setWindowShouldClose();
	void update();
	void render();

	///////////////////////////////////
	//
	//
	//////////////////////////////////

private:

	//////////////////////////////////

	const int WINDOW_MIN_WIDTH = 640;
	const int WINDOW_MIN_HEIGHT = 480;
	const int WINDOW_START_WIDTH = 800;
	const int WINDOW_START_HEIGHT = 600;
	const int WINDOW_MAX_WIDTH = 1024;
	const int WINDOW_MAX_HEIGHT = 768;

	int frameBuffWidth;
	int frameBuffHeight;

	GLFWwindow* window;

	glm::mat4 viewMat;
	glm::mat4 projMat;

	glm::vec3 lightPos;

	float fov;
	float nearPlane;
	float farPlane;

	ShaderLoader* core;
	Mesh* terrain;
	Camera camera;

	float dt;
	float curTime;
	float prevTime;

	double prevMouseX;
	double prevMouseY;
	double mouseX;
	double mouseY;
	double mouseOffsetX;
	double mouseOffsetY;
	bool isFirstMove;

	//////////////////////////////////

	void initWindow(
		const char* title,
		bool resizable,
		GLFWimage images[]
	);

	void initGLFW();
	void initGLEW();
	void customInit();
	void initMatricies();
	void initMashes();
	void initShaders();
	void initLights();
	void initUniforms();
	void updateUniforms();

	void keyboardInput();
	void mouseInput();
	void updateInput();
	void updateDeltaTime();

	void createDialogWindow();

	static LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//////////////////////////////////

};

