#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	vao.Bind();

	VBO vbo(vertices);
	EBO ebo(indices);

	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	vao.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	vao.LinkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera)
{
	shader.Activate();
	vao.Bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for(unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num = "";
		std::string type = textures[i].type;

		if (type == "diffuse") 
		{
			num = std::to_string(numDiffuse++);
		}
		else if(type == "specular")
		{
			num = std::to_string(numSpecular++);
		}

		textures[i].TexUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "cameraMatrix");

	glm::mat4 translationMatrix = glm::mat4(1.0f);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);

	// Transform the matrices to their correct form
	translationMatrix = glm::translate(translationMatrix, position);
	rotationMatrix = glm::mat4_cast(rotation);
	scaleMatrix = glm::scale(scaleMatrix, scale);

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "position"), 1, GL_FALSE, glm::value_ptr(translationMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(scaleMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::SetBasicLocation(glm::mat4 matrix, glm::vec3 position, glm::vec3 rotationEuler, glm::vec3 scale)
{
	SetMatrix(matrix);
	SetPosition(position);
	SetRotation(rotationEuler);
	SetScale(scale);
}

void Mesh::SetBasicLocation(glm::mat4 matrix, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
{
	SetMatrix(matrix);
	SetPosition(position);
	SetRotation(rotation);
	SetScale(scale);
}

void Mesh::SetMatrix(glm::mat4 matrix)
{
	this->matrix = matrix;
}

void Mesh::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void Mesh::SetRotation(glm::vec3 rotationEuler)
{
	this->rotation = glm::quat(1.0f, glm::radians(rotationEuler.x), glm::radians(rotationEuler.y), glm::radians(rotationEuler.z));;
}

void Mesh::SetRotation(glm::quat rotation)
{
	this->rotation = rotation;
}

void Mesh::SetScale(glm::vec3 scale)
{
	this->scale = scale;
}
