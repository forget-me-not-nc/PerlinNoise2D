#include "Program.h"

///////////////////////////////////
//
//
//////////////////////////////////

void createTextLabels(RECT rect, HWND hParent, HINSTANCE hInstance);
void createControlls(HWND hWnd, HINSTANCE instance);
void setWindowState(HWND hWnd, INT menus[], int size, bool flag);

///////////////////////////////////
//
//
//////////////////////////////////

bool windowShouldClose = false;
bool noiseShouldChange = false;
bool inputsAreActive = true;
bool inputsAreCreated = false;

bool animate = false;

const LONG CONTROLLER_WIDTH = 300;
const LONG CONTROLLER_HEIGHT = 806;

const LONG BOX_WIDTH = 100;
const LONG BOX_Y_OFFSET = 30;
const LONG BOX_X_OFFSET = 130;
const LONG LABEL_X_OFFSET = 10;
const LONG LABEL_Y_OFFSET = 31;
const LONG LABEL_WIDTH = 120;

const TCHAR DISPLAY_TYPE_OPTIONS[][6] = {
	TEXT("Fill"),
	TEXT("Dots"),
	TEXT("Lines")
};

const TCHAR INTERPOLATION_TYPE_OPTIONS[][10] = {
	TEXT("None"),
	TEXT("Linear"),
	TEXT("Cosine")
};

const TCHAR POLYGON_TYPE_OPTIONS[][11] = {
	TEXT("TRIANGLES"),
	TEXT("QUADS")
};

const TCHAR MULTIPLIER_OPTIONS[][4] = {
	TEXT("1"),
	TEXT("8"),
	TEXT("16"),
	TEXT("32"),
	TEXT("64"),
};

const TCHAR WORLD_SIZE_OPTIONS[][8] = {
	TEXT("16x16"),
	TEXT("32x32"),
	TEXT("64x64"),
};

const TCHAR NOISE_TYPE_OPTIONS[][14] = {
	TEXT("Random"),
	TEXT("Perlin"),
	TEXT("PerlinOctaves")
};

///////////////////////////////////
//
//
//////////////////////////////////

INT animationDisableMenus[] = {
	POLYGON_TYPE_BOX,
	//INTERPOLATION_TYPE_BOX,
	WORLD_SIZE_SELECTOR,
	MULTIPLIER_SELECTOR,
	//NOISE_HARSH_INPUT,
	//SEED_INPUT,
	OCTAVES_INPUT,
	//AMPLITUDE_INPUT,
	//PERSISTENCE_INPUT,
	//LACUNARITY_INPUT
};

INT onlyPerlinOctavesMenus[] = {
	OCTAVES_INPUT,
	AMPLITUDE_INPUT,
	PERSISTENCE_INPUT,
	LACUNARITY_INPUT
};

///////////////////////////////////
//
//
//////////////////////////////////

WORLD_SIZE worldSize = WORLD_SIZE::S_16x16;
NOISE_TYPE noiseType = NOISE_TYPE::RANDOM;
INTERPOLATION_TYPES interpolationType = INTERPOLATION_TYPES::LINEAR;

GLenum displayType = GL_FILL;
GLenum polygonType = GL_TRIANGLES;

int multiplier = 1;
unsigned int seed = 0;
int octaves = 8;

float harshness = 1;
float amplitude = 1.0f;
float persistence = 0.5f;
float lacunarity = 2.0f;

char floatInputBuff[64];
char intInputBuff[64];

glm::vec3 cameraPos = glm::vec3(8.0f, 5.0f, 16.0f);
glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

///////////////////////////////////
//
//
//////////////////////////////////

Program::Program(
	const char* title,
	bool resizable,
	GLFWimage images[]

) : camera(cameraPos, worldUp)
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
		setupCameraPos();
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

	if (animate)
	{
		initMashes();
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
	glClearColor(0.13333f,  0.15686f,  0.19216f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glCullFace(GL_CCW);

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
	vector<Vertex> v = this->noise->getVertices(polygonType);

	if (terrain) delete terrain;

	this->terrain = new Mesh(v);
	this->terrain->setPolygonType(polygonType);
}

//////////////////////////////////

void Program::initShaders()
{
	this->core = new ShaderLoader("vertShader.glsl", "fragmentShader.glsl");
}

//////////////////////////////////

void Program::initLights()
{
	this->lightPos = this->noise->getLightPos();
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

	if (animate) this->setupAnimationSetting();

	switch (noiseType)
	{
		case NOISE_TYPE::PERLIN:
		{
			this->noise = new PerlinNoise(seed, multiplier, worldSize, harshness, interpolationType);

			break;
		}

		case NOISE_TYPE::PERLIN_OCTAVES:
		{
			this->noise = new PerlinNoiseOctaves(
				seed, 
				multiplier,
				worldSize, 
				harshness, 
				interpolationType,
				octaves, 
				amplitude,
				persistence,
				lacunarity
			);

			break;
		}

		default:
		{
			this->noise = new RandomNoise(seed, multiplier, worldSize, harshness);

			break;
		}
	}

	noiseShouldChange = false;
	this->noise->setAnimation(animate);
}

//////////////////////////////////

void Program::setupCameraPos()
{
	this->camera.setCameraPosition(
		glm::vec3(
			(float)worldSize / 2.0f,
			5.0f,
			(float)worldSize)
	);
}

//////////////////////////////////

void Program::setupAnimationSetting()
{
	worldSize = WORLD_SIZE::S_16x16;
	interpolationType = INTERPOLATION_TYPES::LINEAR;
	polygonType = GL_QUADS;
	multiplier = 8;
	octaves = 2;
}

//////////////////////////////////

void createControlls(HWND hWnd, HINSTANCE instance)
{
	RECT rect = { 0 };

	rect.bottom = CONTROLLER_HEIGHT;
	rect.right = CONTROLLER_WIDTH;

	///////////button////////////////////

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

	///////////animation checkbox////////////////////

	HWND animationCheckBox = CreateWindow(
		L"BUTTON",
		NULL,
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		rect.right - BOX_X_OFFSET - 70,
		rect.bottom - 120,
		20,
		20,
		hWnd,
		(HMENU)ANIMATION_CHECKBOX,
		instance,
		NULL
	);

	///////////world controlls////////////////////

	HWND worldSizeComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 1,
		BOX_WIDTH,
		120,
		hWnd,
		(HMENU)WORLD_SIZE_SELECTOR,
		instance,
		NULL
	);

	HWND multiplierComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 2,
		BOX_WIDTH,
		170,
		hWnd,
		(HMENU)MULTIPLIER_SELECTOR,
		instance,
		NULL
	);

	HWND typeComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 3,
		BOX_WIDTH,
		80,
		hWnd,
		(HMENU)DISPLAY_TYPE_BOX,
		instance,
		NULL
	);

	HWND polygonComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 4,
		BOX_WIDTH,
		60,
		hWnd,
		(HMENU)POLYGON_TYPE_BOX,
		instance,
		NULL
	);

	///////////noise controlls////////////////////

	HWND noiseTypeComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 7,
		BOX_WIDTH,
		80,
		hWnd,
		(HMENU)NOISE_TYPE_SELECTOR,
		instance,
		NULL
	);

	HWND interpolationComboBox = CreateWindow(
		WC_COMBOBOX,
		NULL,
		CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 8,
		BOX_WIDTH,
		80,
		hWnd,
		(HMENU)INTERPOLATION_TYPE_BOX,
		instance,
		NULL
	);

	///////////init controlls////////////////////

	TCHAR A[255];
	memset(&A, 0, sizeof(A));

	int count = sizeof(DISPLAY_TYPE_OPTIONS) / sizeof(DISPLAY_TYPE_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)DISPLAY_TYPE_OPTIONS[i]);

		SendMessage(typeComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
	}

	count = sizeof(POLYGON_TYPE_OPTIONS) / sizeof(POLYGON_TYPE_OPTIONS[0]);

	for (int i = 0; i < count; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)POLYGON_TYPE_OPTIONS[i]);

		SendMessage(polygonComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
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

	SendMessage(typeComboBox, CB_SETCURSEL, 0, 0);
	SendMessage(polygonComboBox, CB_SETCURSEL, 0, 0);
	SendMessage(interpolationComboBox, CB_SETCURSEL, 1, 0);
	SendMessage(multiplierComboBox, CB_SETCURSEL, 0, 0);
	SendMessage(noiseTypeComboBox , CB_SETCURSEL, 0, 0);
	SendMessage(worldSizeComboBox, CB_SETCURSEL, 0, 0);

	///////////inputs////////////////////

	HWND noiseHarshInput = CreateWindow(
		WC_EDIT,
		NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 9,
		BOX_WIDTH,
		20,
		hWnd,
		(HMENU)NOISE_HARSH_INPUT,
		instance,
		NULL
	);

	HWND seedInput = CreateWindow(
		WC_EDIT,
		NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 5,
		BOX_WIDTH,
		20,
		hWnd,
		(HMENU)SEED_INPUT,
		instance,
		NULL
	);

	HWND octavesInput = CreateWindow(
		WC_EDIT,
		NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 10,
		BOX_WIDTH,
		20,
		hWnd,
		(HMENU)OCTAVES_INPUT,
		instance,
		NULL
	);

	HWND amplitudeInput = CreateWindow(
		WC_EDIT,
		NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 11,
		BOX_WIDTH,
		20,
		hWnd,
		(HMENU)AMPLITUDE_INPUT,
		instance,
		NULL
	);

	HWND persistenceInput = CreateWindow(
		WC_EDIT,
		NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 12,
		BOX_WIDTH,
		20,
		hWnd,
		(HMENU)PERSISTENCE_INPUT,
		instance,
		NULL
	);

	HWND lacunarityInput = CreateWindow(
		WC_EDIT,
		NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE,
		rect.right - BOX_X_OFFSET,
		rect.top + BOX_Y_OFFSET * 13,
		BOX_WIDTH,
		20,
		hWnd,
		(HMENU)LACUNARITY_INPUT,
		instance,
		NULL
	);

	///////////init inputs////////////////////

	SetWindowTextA(noiseHarshInput, std::to_string(harshness).c_str());
	SetWindowTextA(seedInput, std::to_string(seed).c_str());
	SetWindowTextA(octavesInput, std::to_string(octaves).c_str());
	SetWindowTextA(amplitudeInput, std::to_string(amplitude).c_str());
	SetWindowTextA(persistenceInput, std::to_string(persistence).c_str());
	SetWindowTextA(lacunarityInput, std::to_string(lacunarity).c_str());

	///////////init labels////////////////////

	createTextLabels(rect, hWnd, instance);

	inputsAreCreated = true;
}

//////////////////////////////////

void createTextLabels(RECT rect, HWND hParent, HINSTANCE hInstance)
{
	HWND animationCheckLable = CreateWindow(L"STATIC", L" Enable Animation ", WS_CHILD | WS_VISIBLE, rect.right - BOX_X_OFFSET - 50, rect.bottom - 119, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND sizeLable = CreateWindow(L"STATIC", L" World size: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 1 - 0, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND multLable = CreateWindow(L"STATIC", L" Multiplier: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 2 - 1, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND displayLable = CreateWindow(L"STATIC", L" Polygon display: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 3 - 2, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND polygonTypeLable = CreateWindow(L"STATIC", L" Polygon type: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 4 - 3, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND seedLable = CreateWindow(L"STATIC", L" Seed: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 5 - 4, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND noiseTypeLable = CreateWindow(L"STATIC", L" Noise type: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 7 - 6, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND interpolTypeLable = CreateWindow(L"STATIC", L" Interpolation type: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 8 - 7, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND harshLable = CreateWindow(L"STATIC", L" Harshiness: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 9 - 8, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND octavesLable = CreateWindow(L"STATIC", L" Octaves: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 10 - 9, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND amplitudeLable = CreateWindow(L"STATIC", L" Amplitude: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 11 - 10, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND persistenceLable = CreateWindow(L"STATIC", L" Persistence: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 12 - 11, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
	HWND lacunarityLable = CreateWindow(L"STATIC", L" Lacunarity: ", WS_CHILD | WS_VISIBLE, rect.left + LABEL_X_OFFSET, rect.top + LABEL_Y_OFFSET * 13 - 12, LABEL_WIDTH, 20, hParent, (HMENU)STATIC_LABEL, hInstance, NULL);
}

//////////////////////////////////

void setWindowState(HWND hWnd, INT menus[], int size, bool flag)
{
	for (int i = 0; i < size; i++)
	{
		EnableWindow(GetDlgItem(hWnd, menus[i]), flag);
	}
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
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(238, 238, 238)));

			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(34, 40, 49));
			SetBkColor(hdcStatic, RGB(238, 238, 238));

			return (LRESULT)GetStockObject(NULL_BRUSH);

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

				case ANIMATION_CHECKBOX:
				{
					if(HIWORD(wParam) == BN_CLICKED)
					{
						if (SendDlgItemMessage(hWnd, ANIMATION_CHECKBOX, BM_GETCHECK, 0, 0))
						{
							animate = true;

							setWindowState(hWnd, animationDisableMenus, sizeof(animationDisableMenus) / sizeof(INT), false);

							noiseShouldChange = true;
						}
						else
						{
							animate = false;

							setWindowState(hWnd, animationDisableMenus, sizeof(animationDisableMenus) / sizeof(INT), true);

							inputsAreActive = true;

							polygonType = GL_TRIANGLES;
						}
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

				case POLYGON_TYPE_BOX:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						if (selectedItem == 0) polygonType = GL_TRIANGLES;
						if (selectedItem == 1) polygonType = GL_QUADS;
					}

					break;
				}

				case WORLD_SIZE_SELECTOR:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						if (selectedItem == 0) worldSize = WORLD_SIZE::S_16x16;
						if (selectedItem == 1) worldSize = WORLD_SIZE::S_32x32;
						if (selectedItem == 2) worldSize = WORLD_SIZE::S_64x64;
					}
			
					break;
				}

				case INTERPOLATION_TYPE_BOX:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						int selectedItem = SendMessage((HWND)lParam, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						if (selectedItem == 0) interpolationType = INTERPOLATION_TYPES::NONE;
						if (selectedItem == 1) interpolationType = INTERPOLATION_TYPES::LINEAR;
						if (selectedItem == 2) interpolationType = INTERPOLATION_TYPES::COSINE;
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
						if (selectedItem == 2) noiseType = NOISE_TYPE::PERLIN_OCTAVES;
					}

					break;
				}

				case NOISE_HARSH_INPUT:
				{
					if (HIWORD(wParam) == EN_CHANGE)
					{
						memset(floatInputBuff, 0, sizeof(floatInputBuff));

						GetWindowTextA((HWND)lParam, floatInputBuff, sizeof(floatInputBuff));

						try
						{
							harshness = (float)atof(floatInputBuff);
						}
						catch (const std::exception&) {}
					}

					break;
				}

				case SEED_INPUT:
				{
					if (HIWORD(wParam) == EN_CHANGE)
					{
						memset(intInputBuff, 0, sizeof(intInputBuff));

						GetWindowTextA((HWND)lParam, intInputBuff, sizeof(intInputBuff));

						try
						{
							seed = atoi(intInputBuff);
						}
						catch (const std::exception&) {}
					}

					break;
				}

				case OCTAVES_INPUT:
				{
					if (HIWORD(wParam) == EN_CHANGE)
					{
						memset(intInputBuff, 0, sizeof(intInputBuff));

						GetWindowTextA((HWND)lParam, intInputBuff, sizeof(intInputBuff));

						try
						{
							octaves = atoi(intInputBuff);
						}
						catch (const std::exception&) {}
					}

					break;
				}

				case AMPLITUDE_INPUT:
				{
					if (HIWORD(wParam) == EN_CHANGE)
					{
						memset(floatInputBuff , 0, sizeof(floatInputBuff));

						GetWindowTextA((HWND)lParam, floatInputBuff, sizeof(floatInputBuff));

						try
						{
							amplitude = (float)atof(floatInputBuff);
						}
						catch (const std::exception&) {}
					}

					break;
				}

				case LACUNARITY_INPUT:
				{
					if (HIWORD(wParam) == EN_CHANGE)
					{
						memset(floatInputBuff, 0, sizeof(floatInputBuff));

						GetWindowTextA((HWND)lParam, floatInputBuff, sizeof(floatInputBuff));

						try
						{
							lacunarity = (float)atof(floatInputBuff);
						}
						catch (const std::exception&) {}
					}

					break;
				}

				case PERSISTENCE_INPUT:
				{
					if (HIWORD(wParam) == EN_CHANGE)
					{
						memset(floatInputBuff, 0, sizeof(floatInputBuff));

						GetWindowTextA((HWND)lParam, floatInputBuff, sizeof(floatInputBuff));

						try
						{
							persistence = (float)atof(floatInputBuff);
						}
						catch (const std::exception&) {}
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

	if (noiseType != NOISE_TYPE::PERLIN_OCTAVES && inputsAreActive && inputsAreCreated)
	{
		setWindowState(hWnd, onlyPerlinOctavesMenus, sizeof(onlyPerlinOctavesMenus) / sizeof(INT), false);
		
		inputsAreActive = false;
	}
	else if (noiseType == NOISE_TYPE::PERLIN_OCTAVES && !inputsAreActive && inputsAreCreated)
	{
		setWindowState(hWnd, onlyPerlinOctavesMenus, sizeof(onlyPerlinOctavesMenus) / sizeof(INT), true);
		
		if (animate)
		{
			INT t[]{ OCTAVES_INPUT };
			setWindowState(hWnd, t, 1, false);
		}
		
		inputsAreActive = true;
	}

	return DefWindowProcW(hWnd, message, wParam, lParam);
}

///////////callbacks////////////////////

void frameBufferResize(GLFWwindow* window, int bWidth, int bHeight)
{
	glViewport(0, 0, bWidth, bHeight);
}

//////////////////////////////////
