#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Graphics/Mesh.h" 
#include "Graphics/Model.h"

const unsigned int width = 800;
const unsigned int height = 800;

int main() 
{
	Logger* mainLogger = new Logger("Main");

	mainLogger->SetMain();
	mainLogger->Log("Main logger was successfully set up!");

	glfwInit();

	// OpenGL version checks
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "BEngine", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

	// Texture data
	Texture textures[]
	{
		Texture("Assets/Models/Mask/textures/mask_blinn1_BaseColor.png", "diffuse", 0),
		// Texture("Assets/Models/Mask/textures/mask_blinn1_Roughness.png", "specular", 0)
	};

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("Shaders/default.vert", "Shaders/default.frag");
	// Store mesh data in vectors for the mesh

	Model model = Model("Assets/Models/Mask/mask.fbx");

	ModelPart basePart = model.GetModelParts()[0];

	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	// Create floor mesh
	Mesh floor(basePart.GetVertices(), basePart.GetIndices(), tex);

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	double prevTime = glfwGetTime();

	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 2.0f, 0.0f));

	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		double critTime = glfwGetTime();

		double deltaTime = critTime - prevTime;

		prevTime = critTime;

		camera.Inputs(window, deltaTime);

		camera.UpdateMatrixData(45.0f, 0.1f, 100.0f);

		floor.Draw(shaderProgram, camera);

		glfwSwapBuffers(window);

		// Process events on window
		glfwPollEvents();
	}

	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
