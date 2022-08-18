#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

class Mesh 
{
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);

	void Draw(Shader& shader, Camera& camera);

	void SetBasicLocation(glm::mat4 matrix, glm::vec3 position, glm::vec3 rotationEuler, glm::vec3 scale);
	void SetBasicLocation(glm::mat4 matrix, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	void SetMatrix(glm::mat4 matrix);
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 rotationEuler);
	void SetRotation(glm::quat rotation);
	void SetScale(glm::vec3 scale);
private:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	glm::mat4 matrix = glm::mat4(1.0f);

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	VAO vao;
};

#endif
