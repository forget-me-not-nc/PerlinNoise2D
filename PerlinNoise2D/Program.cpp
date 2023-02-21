#include "Program.h"

///////////////////////////////////
//
//
//////////////////////////////////

bool windowShouldClose = false;
bool noiseShouldChange = false;

const LONG CONTROLLER_WIDTH = 250;
const LONG CONTROLLER_HEIGHT = 600;

const TCHAR GRID_AND_MULT_BOX_OPTIONS[][3] = {
	TEXT("1"),
	TEXT("2"),
	TEXT("4"),
	TEXT("8"),
	TEXT("16"),
	TEXT("32")
};

const TCHAR DISPLAY_TYPE_OPTIONS[][6] = {
	TEXT("Fill"),
	TEXT("Dots"),
	TEXT("Lines")
};

const TCHAR INTERPOLATION_TYPE_OPTIONS[][10] = {
	TEXT("None"),
	TEXT("Cosine")
};

const TCHAR MULTIPLIER_OPTIONS[][3] = {
	TEXT("1"),
	TEXT("2"),
	TEXT("4"),
	TEXT("8"),
	TEXT("16")
};

const TCHAR WORLD_SIZE_OPTIONS[][8] = {
	TEXT("32x32"),
	TEXT("64x64"),
	TEXT("128x128"),
};

const TCHAR NOISE_TYPE_OPTIONS[][7] = {
	TEXT("Random"),
	TEXT("Perlin")
};

///////////////////////////////////
//
//
//////////////////////////////////

int gridXSize = 16;
int gridYSize = 16;
WORLD_SIZE worldSize = WORLD_SIZE::S_32x32;
int multiplier = 2;
unsigned int seed = 0;
GLenum displayType = GL_FILL;
NOISE_TYPE noiseType = NOISE_TYPE::RANDOM;

///////////////////////////////////
//
//
//////////////////////////////////

Program::Program(
	const char* title,
	bool resizable,
	GLFWimage images[]

) : camera(glm::vec3(0.0f, 0.5f, 1.5f), glm::vec3(0.0f, 1.0f, 0.0f))
{
	this->initNoise();

	this->initGLFW();
	this->initWindow(title, resizable, images);
	this->initGLEW();
	this->customInit();
	this->initMatricies();
	this->initShaders();
	this->initMashes();
	this->initLights();
	this->initUniforms();

	this->createDialogWindow();
}

//////////////////////////////////

Program::~Program()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();

	delete this->core;
	delete this->terrain;
	delete this->noise;
}

//////////////////////////////////

int Program::getWindowShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

//////////////////////////////////

void Program::setWindowShouldClose()
{
	glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

//////////////////////////////////

void Program::update()
{
	this->updateDeltaTime();
	this->updateInput();

	if (windowShouldClose) this->setWindowShouldClose();
}

//////////////////////////////////

void Program::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///////////update noise if needed////////////////////

	if (noiseShouldChange)
	{
		initNoise();
		initMashes();
		initLights();
	}

	///////////draw////////////////////

	glPolygonMode(GL_FRONT_AND_BACK, displayType);

	if (displayType == GL_POINT)
	{
		glEnable(GL_POINT_SMOOTH);
		glPointSize(5.0f);

	}
	else if (displayType == GL_LINE)
	{
		glLineWidth(2.0f);
	}

	this->updateUniforms();

	this->terrain->render(this->core);

	glfwSwapBuffers(window);
	glFlush();

	///////////reset////////////////////

	glBindVertexArray(0);
	glUseProgram(0);

	if (displayType == GL_POINT)
	{
		glDisable(GL_POINT_SMOOTH);
		glPointSize(1.0f);

	}
	else if (displayType == GL_LINES)
	{
		glLineWidth(1.0f);
	}
}

//////////////////////////////////

void Program::initWindow(
	const char* title,
	bool resizable,
	GLFWimage images[]
)
{
	///////////init params////////////////////

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	this->window = glfwCreateWindow(WINDOW_START_WIDTH, WINDOW_START_HEIGHT, title, NULL, NULL);

	if (this->window == nullptr)
	{
		cout << "Program.cpp::initWindow() --> Failed to create window" << endl;

		glfwTerminate();
	}

	glfwSetWindowIcon(this->window, 1, images);

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBuffWidth, &frameBuffHeight);
	glfwSetWindowSizeLimits(window, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT, WINDOW_MAX_WIDTH, WINDOW_MAX_HEIGHT);
	glfwSetFramebufferSizeCallback(window, frameBufferResize);
	glfwSetWindowSize(window, WINDOW_MAX_WIDTH, WINDOW_MAX_HEIGHT);
}

//////////////////////////////////

void Program::initGLFW()
{
	if (glfwInit() == GLFW_FALSE)
	{
		cout << "Program.cpp::initGLFW() --> Failed to init GLFW" << endl;

		glfwTerminate();
	}
}

//////////////////////////////////

void Program::initGLEW()
{
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		cout << "Program.cpp::initGLEW() --> Failed to init GLEW" << endl;

		glfwTerminate();
	}
}

//////////////////////////////////

void Program::customInit()
{
	glClearColor(0.062f, 0.070f, 0.078f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	this->fov = 90.0f;
	this->nearPlane = 0.1f;
	this->farPlane = 100.0f;

	this->dt = 0.0f;
	this->curTime = 0.0f;
	this->prevTime = 0.0f;

	this->mouseX = 0.0;
	this->mouseY = 0.0;
	this->prevMouseX = 0.0;
	this->prevMouseY = 0.0;
	this->mouseOffsetX = 0.0;
	this->mouseOffsetY = 0.0;
	this->isFirstMove = true;
}

//////////////////////////////////

void Program::initMatricies()
{
	this->viewMat = glm::mat4(1.0f);

	this->projMat = glm::mat4(1.0f);
	this->projMat = glm::perspective(
		glm::radians(this->fov),
		static_cast<float>(this->frameBuffWidth) / this->frameBuffHeight,
		this->nearPlane,
		this->farPlane);
}

//////////////////////////////////

void Program::initMashes()
{
	vector<Vertex> v = this->noise->getVertecies();

	if (terrain) delete terrain;

	this->terrain = new Mesh(v);
}

//////////////////////////////////

void Program::initShaders()
{
	this->core = new ShaderLoader("vertShader.glsl", "fragmentShader.glsl");
}

//////////////////////////////////

void Program::initLights()
{
	this->lightPos = glm::vec3(
		static_cast<int>(worldSize) / 2.0f, 
		static_cast<int>(worldSize) / 3.5f,
		static_cast<int>(worldSize) / 2.0f
	);
}

//////////////////////////////////

void Program::initUniforms()
{
	this->core->setMat4(this->viewMat, "viewMat");
	this->core->setMat4(this->projMat, "projMat");

	this->core->setVec3(this->lightPos, "lightPos");
}

//////////////////////////////////

void Program::updateUniforms()
{
	this->viewMat = this->camera.getVievMatrix();

	glfwGetFramebufferSize(this->window, &this->frameBuffWidth, &this->frameBuffHeight);

	if (this->frameBuffHeight != 0 && this->frameBuffWidth != 0)
	{
		this->projMat = glm::perspective(
			glm::radians(this->fov),
			static_cast<float>(this->frameBuffWidth) / this->frameBuffHeight,
			this->nearPlane,
			this->farPlane);
	}

	this->core->setMat4(this->projMat, "projMat");
	this->core->setMat4(this->viewMat, "viewMat");
	this->core->setVec3(this->camera.getCameraPosition(), "cameraPos");
	this->core->setVec3(this->lightPos, "lightPos");
}

//////////////////////////////////

void Program::keyboardInput()
{
	//program
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		this->setWindowShouldClose();
	}

	//camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		this->camera.move(this->dt, DIRECTIONS::FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		this->camera.move(this->dt, DIRECTIONS::LEFT);

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		this->camera.move(this->dt, DIRECTIONS::BACKWARD);

	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		this->camera.move(this->dt, DIRECTIONS::RIGHT);
	}

	//mesh
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		this->terrain->rotate(glm::vec3(0.0f, -1.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		this->terrain->rotate(glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		this->terrain->scale(glm::vec3(0.05f, 0.05f, 0.05f));
	}
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		this->terrain->scale(glm::vec3(-0.05f, -0.05f, -0.05f));
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		this->terrain->move(glm::vec3(0.0f, 0.0f, 0.2f));
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		this->terrain->move(glm::vec3(0.2f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		this->terrain->move(glm::vec3(0.0f, 0.0f, -0.2f));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		this->terrain->move(glm::vec3(-0.2f, 0.0f, 0.0f));
	}

}

//////////////////////////////////

void Program::mouseInput()
{
	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		isMouseLeftButtonPressed = true;
	}
	else
	{
		isMouseLeftButtonPressed = false;
	}

	glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);

	if (this->isFirstMove)
	{
		this->prevMouseX = this->mouseX;
		this->prevMouseY = this->mouseY;

		this->isFirstMove = false;
	}

	this->mouseOffsetX = this->mouseX - this->prevMouseX;
	this->mouseOffsetY = this->prevMouseY - this->mouseY;

	this->prevMouseX = this->mouseX;
	this->prevMouseY = this->mouseY;
}

//////////////////////////////////

void Program::updateInput()
{
	glfwPollEvents();

	this->keyboardInput();
	this->mouseInput();
	this->camera.updateInput(dt, DIRECTIONS::NONE, mouseOffsetX, mouseOffsetY);
}

//////////////////////////////////

void Program::updateDeltaTime()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->prevTime;
	this->prevTime = this->curTime;
}

//////////////////////////////////

void Program::createDialogWindow()
{
	WNDCLASSW window = { 0 };

	window.hInstance = NULL;
	window.lpszClassName = L"ControllerWindow";
	window.lpfnWndProc = windowProcedure;
	window.hIcon = reinterpret_cast<HICON>(LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));

	RegisterClassW(&window);

	CreateWindowW(
		L"ControllerWindow",
		L"Controllers",
		WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		200,
		200,
		CONTROLLER_WIDTH,
		CONTROLLER_HEIGHT,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

//////////////////////////////////

void Program::initNoise()
{
	if (noise) delete noise;

	switch (noiseType)
	{
		case NOISE_TYPE::PERLIN:
		{

			break;
		}

		default:
		{
			this->noise = new RandomNoise(seed, multiplier, worldSize);

			break;
		}
	}

	noiseShouldChange = false;
}

//////////////////////////////////

void createControlls(HWND hWnd, HINSTANCE instance)
{
	RECT rect = { 0 };

	rect.bottom = CONTROLLER_HEIGHT;
	rect.right = CONTROLLER_WIDTH;

	HWND displayButton = CreateWindow(
		L"BUTTON",
		L"Display",
		BS_FLAT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		((rect.right) / 2) - 50,
		rect.bottom - 90,
		100,
		30,
		hWnd,
		(HMENU)DISPLAY_BUTTON,
		instance,
		NULL
	);

	HWND gridXComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		((rect.right) / 2) - 50,
		rect.top + 30,
		80,
		130,
		hWnd,
		(HMENU)GRID_X_BOX,
		instance,
		NULL
	);

	HWND gridYComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		((rect.right) / 2) - 50,
		rect.top + 60,
		80,
		130,
		hWnd,
		(HMENU)GRID_Y_BOX,
		instance,
		NULL
	);

	HWND interpolationComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		((rect.right) / 2) - 50,
		rect.top + 90,
		80,
		80,
		hWnd,
		(HMENU)DISPLAY_TYPE_BOX,
		instance,
		NULL
	);

	HWND typeComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		((rect.right) / 2) - 50,
		rect.top + 150,
		80,
		80,
		hWnd,
		(HMENU)DISPLAY_TYPE_BOX,
		instance,
		NULL
	);

	HWND multiplierComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		((rect.right) / 2) - 50,
		rect.top + 180,
		80,
		140,
		hWnd,
		(HMENU)MULTIPLIER_SELECTOR,
		instance,
		NULL
	);

	HWND noiseTypeComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		((rect.right) / 2) - 50,
		rect.top + 210,
		80,
		80,
		hWnd,
		(HMENU)NOISE_TYPE_SELECTOR,
		instance,
		NULL
	);

	HWND worldSizeComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		((rect.right) / 2) - 50,
		rect.top + 240,
		80,
		100,
		hWnd,
		(HMENU)WORLD_SIZE_SELECTOR,
		instance,
		NULL
	);

	TCHAR A[255];
	memset(&A, 0, sizeof(A));

	int count = sizeof(GRID_AND_MULT_BOX_OPTIONS) / sizeof(GRID_AND_MULT_BOX_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)GRID_AND_MULT_BOX_OPTIONS[i]);

		SendMessage(gridXComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
		SendMessage(gridYComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
	}

	count = sizeof(DISPLAY_TYPE_OPTIONS) / sizeof(DISPLAY_TYPE_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)DISPLAY_TYPE_OPTIONS[i]);

		SendMessage(typeComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
	}

	count = sizeof(INTERPOLATION_TYPE_OPTIONS) / sizeof(INTERPOLATION_TYPE_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)INTERPOLATION_TYPE_OPTIONS[i]);

		SendMessage(interpolationComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
	}

	count = sizeof(MULTIPLIER_OPTIONS) / sizeof(MULTIPLIER_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)MULTIPLIER_OPTIONS[i]);

		SendMessage(multiplierComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
	}

	count = sizeof(WORLD_SIZE_OPTIONS) / sizeof(WORLD_SIZE_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)WORLD_SIZE_OPTIONS[i]);

		SendMessage(worldSizeComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
	}

	count = sizeof(NOISE_TYPE_OPTIONS) / sizeof(NOISE_TYPE_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)NOISE_TYPE_OPTIONS[i]);

		SendMessage(noiseTypeComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
	}

	SendMessage(gridXComboBox, CB_SETCURSEL, 4, 0);
	SendMessage(gridYComboBox, CB_SETCURSEL, 4, 0);
	SendMessage(typeComboBox, CB_SETCURSEL, 0, 0);
	SendMessage(interpolationComboBox, CB_SETCURSEL, 0, 0);
	SendMessage(multiplierComboBox, CB_SETCURSEL, 0, 0);
	SendMessage(noiseTypeComboBox , CB_SETCURSEL, 0, 0);
	SendMessage(worldSizeComboBox, CB_SETCURSEL, 0, 0);
}

//////////////////////////////////

LRESULT Program::windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));

	switch (message)
	{
		case WM_CREATE:
		{
			createControlls(hWnd, instance);

			break;
		}

		case WM_ERASEBKGND:
		{
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(234, 234, 234)));

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case DISPLAY_BUTTON:
				{
					noiseShouldChange = true;

					break;
				}

				case GRID_X_BOX:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						gridXSize = _wtoi(GRID_AND_MULT_BOX_OPTIONS[selectedItem]);
					}

					break;
				}

				case GRID_Y_BOX:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						gridYSize = _wtoi(GRID_AND_MULT_BOX_OPTIONS[selectedItem]);
					}

					break;
				}

				case DISPLAY_TYPE_BOX:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						if (selectedItem == 0) displayType = GL_FILL;
						if (selectedItem == 1) displayType = GL_POINT;
						if (selectedItem == 2) displayType = GL_LINE;
					}

					break;
				}

				case WORLD_SIZE_SELECTOR:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						if (selectedItem == 0) worldSize = WORLD_SIZE::S_32x32;
						if (selectedItem == 1) worldSize = WORLD_SIZE::S_64x64;
						if (selectedItem == 2) worldSize = WORLD_SIZE::S_128x128;
					}
			
					break;
				}

				case MULTIPLIER_SELECTOR:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						multiplier = _wtoi(MULTIPLIER_OPTIONS[selectedItem]);
					}

					break;
				}

				case NOISE_TYPE_SELECTOR:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						if (selectedItem == 0) noiseType = NOISE_TYPE::RANDOM;
						if (selectedItem == 1) noiseType = NOISE_TYPE::PERLIN;
					}

					break;
				}


				default: break;
			}

			break;
		}

		case WM_DESTROY:
		{
			DestroyWindow(hWnd);

			windowShouldClose = true;

			break;
		}

		default: break;
	}

	return DefWindowProcW(hWnd, message, wParam, lParam);
}

///////////callbacks////////////////////

void frameBufferResize(GLFWwindow* window, int bWidth, int bHeight)
{
	glViewport(0, 0, bWidth, bHeight);
}

//////////////////////////////////
