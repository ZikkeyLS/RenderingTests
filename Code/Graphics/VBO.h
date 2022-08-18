#ifndef  VBO_CLASS_H
#define VBO_CLASS_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

// VBO(vertex buffer object)

struct Vertex 
{
public:
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 TextureUV;
};

class VBO 
{
public:
	GLuint ID;
	VBO(std::vector<Vertex>& vertices);

	void Bind();
	void Unbind();
	void Delete();
};

#endif
