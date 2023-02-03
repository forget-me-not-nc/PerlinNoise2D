#include "ShaderLoader.h"

///////////////////////////////////
//
//
//////////////////////////////////

ShaderLoader::ShaderLoader(string vertFile, string fragFile)
{
	///////////load shader////////////////////

	GLuint vShader = 0;
	GLuint fShader = 0;

	vShader = loadShader(vertFile, GL_VERTEX_SHADER);
	fShader = loadShader(fragFile, GL_FRAGMENT_SHADER);

	this->linkProgram(vShader, fShader);

	///////////clear////////////////////

	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

//////////////////////////////////

string ShaderLoader::readShaderSrc(string file)
{
	///////////read shader////////////////////

	string line = "";
	string data = "";

	std::ifstream readFrom;

	readFrom.open(file);

	if (readFrom.is_open())
	{
		while (std::getline(readFrom, line))
		{
			data += line + '\n';
		}
	}
	else
	{
		printLog("ShaderLoader.cpp::readShaderSrc() --> Failed to read shader!");
	}

	readFrom.close();

	return data;
}

//////////////////////////////////

void ShaderLoader::linkProgram(GLuint& vertShader, GLuint& fragmentShader)
{
	///////////link shaders and program////////////////////

	memset(this->log, 0, sizeof(this->log));
	this->currStatus = GL_TRUE;

	this->programId = glCreateProgram();

	glAttachShader(this->programId, vertShader);
	glAttachShader(this->programId, fragmentShader);
	glLinkProgram(this->programId);

	glGetProgramiv(this->programId, GL_LINK_STATUS, &this->currStatus);

	if (this->currStatus == GL_FALSE)
	{
		glGetProgramInfoLog(this->programId, 512, NULL, this->log);

		printLog("ShaderLoader.cpp::linkProgram() --> Failed to link shaders");
		printLog(this->log);
	}

	///////////clear////////////////////

	glUseProgram(0);
}

//////////////////////////////////

GLuint ShaderLoader::loadShader(string file, GLenum shaderType)
{
	///////////read glsl////////////////////

	memset(this->log, 0, sizeof(this->log));
	this->currStatus = GL_TRUE;

	GLuint shader = glCreateShader(shaderType);
	string data = this->readShaderSrc(file);
	const GLchar* shaderData = data.c_str();

	glShaderSource(shader, 1, &shaderData, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &this->currStatus);

	if (this->currStatus == GL_FALSE)
	{
		glGetShaderInfoLog(shader, sizeof(this->log), NULL, this->log);

		printLog("ShaderLoader.cpp::loadShader() --> Failed to compile shader");
		printLog(file);
		printLog(this->log);
	}

	return shader;
}

//////////////////////////////////

void ShaderLoader::printLog(string message)
{
	cout << "---------------" << endl;
	cout << message << endl;
	cout << "---------------" << endl;
}

//////////////////////////////////

ShaderLoader::~ShaderLoader()
{
	glDeleteProgram(this->programId);
}

//////////////////////////////////

void ShaderLoader::use()
{
	glUseProgram(this->programId);
}

//////////////////////////////////

void ShaderLoader::unuse()
{
	glUseProgram(0);
}

//////////////////////////////////

void ShaderLoader::setVec3(glm::vec3 vec, const GLchar* uniName)
{
	this->use();

	glUniform3fv(glGetUniformLocation(this->programId, uniName), 1, glm::value_ptr(vec));

	this->unuse();
}

//////////////////////////////////

void ShaderLoader::setMat4(glm::mat4 mat, const GLchar* uniName, GLboolean transpose)
{
	this->use();

	glUniformMatrix4fv(glGetUniformLocation(this->programId, uniName), 1, transpose, glm::value_ptr(mat));

	this->unuse();
}
