#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include "../Debug/Logger.h"

std::string GetFileContent(const char* fileName);

class Shader 
{
public:
	GLuint ID;
	Shader() {}
	Shader(const char* vertexFile, const char* fragmentFile);

	void Activate();
	void Delete();
private:
	void CompileErrors(unsigned int shader, const char* type);
};

#endif

