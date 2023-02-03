#include "Program.h"

///////////////////////////////////
//
//
//////////////////////////////////

bool windowShouldClose = false;

const LONG CONTROLLER_WIDTH = 250;
const LONG CONTROLLER_HEIGHT = 600;

const TCHAR COMBO_BOX_OPTIONS[][3] = {
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

///////////////////////////////////
//
//
//////////////////////////////////

int gridXSize = 16;
int gridYSize = 16;
GLenum displayType = GL_FILL;

///////////////////////////////////
//
//
//////////////////////////////////

Program::Program(
	const char* title,
	bool resizable,
	GLFWimage images[]

) : camera(glm::vec3(0.0f, 0.0f, 1.5f), glm::vec3(0.0f, 1.0f, 0.0f))
{
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
	glClearColor(0.145f, 0.164f, 0.203f, 0.0f);

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
	vector<Vertex> v = vector<Vertex>(sample, sample + vCount);

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
	this->lightPos = glm::vec3(0.0f, 1.0f, 1.0f);
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

	HWND typeComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		((rect.right) / 2) - 50,
		rect.top + 120,
		80,
		80,
		hWnd,
		(HMENU)DISPLAY_TYPE_BOX,
		instance,
		NULL
	);

	HWND interpolationComboBox = CreateWindow(
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

	TCHAR A[255];
	memset(&A, 0, sizeof(A));

	int count = sizeof(COMBO_BOX_OPTIONS) / sizeof(COMBO_BOX_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)COMBO_BOX_OPTIONS[i]);

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

	SendMessage(gridXComboBox, CB_SETCURSEL, 4, 0);
	SendMessage(gridYComboBox, CB_SETCURSEL, 4, 0);
	SendMessage(typeComboBox, CB_SETCURSEL, 0, 0);
	SendMessage(interpolationComboBox, CB_SETCURSEL, 0, 0);
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
			//update noise

			break;
		}

		case GRID_X_BOX:
		{
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

				gridXSize = _wtoi(COMBO_BOX_OPTIONS[selectedItem]);
			}

			break;
		}

		case GRID_Y_BOX:
		{
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

				gridYSize = _wtoi(COMBO_BOX_OPTIONS[selectedItem]);
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
