#ifndef  TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "Shader.h"

class Texture
{
public:
	GLuint ID;
	const char* type;
	GLuint unit;
	std::string path;

	Texture(const char* image, const char* texType, GLuint slot);

	void TexUnit(Shader& shader, const char* uniform, GLuint unit);
	void Bind();
	void Unbind();
	void Delete();
};

#endif
