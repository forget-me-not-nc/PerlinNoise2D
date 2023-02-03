#pragma once

#include "Vertex.h"

///////////////////////////////////
//
//
//////////////////////////////////


class ShaderLoader
{
	///////////////////////////////////
	//
	//
	//////////////////////////////////

public:

	ShaderLoader(string vertFile, string fragFile);
	~ShaderLoader();

	//////////////////////////////////

	void use();
	void unuse();
	void setVec3(glm::vec3 vec, const GLchar* uniName);
	void setMat4(glm::mat4 mat, const GLchar* uniName, GLboolean transpose = GL_FALSE);

	///////////////////////////////////
	//
	//
	//////////////////////////////////

private:

	GLuint programId;
	GLint currStatus;
	char log[512];

	//////////////////////////////////

	string readShaderSrc(string file);
	GLuint loadShader(string file, GLenum shaderType);
	void linkProgram(GLuint& vertShader, GLuint& fragmentShader);
	void printLog(string message);

	//////////////////////////////////

};
