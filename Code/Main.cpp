#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Graphics/Mesh.h" 
#include "Graphics/Model.h"
#include "Graphics/Skybox.h"

const unsigned int width = 800;
const unsigned int height = 800;

unsigned int samples = 8;

int main() 
{
	Logger* mainLogger = new Logger("Main");

	mainLogger->SetMain();
	mainLogger->Log("Main logger was successfully set up!");

	glfwInit();

	// OpenGL version checks
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, samples);
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

	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("Shaders/default.vert", "Shaders/default.frag");
	Shader skyboxShader("Shaders/skybox.vert", "Shaders/skybox.frag");

	//Test Model
	Model model = Model("Assets/Models/Scroll/scene.gltf");

	std::vector<Mesh> meshes;

	std::vector<ModelPart> parts = model.GetModelParts();

	for(int i = 0; i < parts.size(); i++)
	{
		ModelPart basePart = parts[i];
		Mesh mesh = Mesh(basePart.GetVertices(), basePart.GetIndices(), basePart.GetTextures());
		// mesh.SetBasicLocation(basePart.Matrix, basePart.Position, basePart.Rotation, basePart.Scale);
		mesh.scale = basePart.Scale / 10.0f;
		mesh.SetRotation(glm::vec3(0, 0, 0));

		meshes.push_back(mesh);
		
		std::string scale = std::to_string(basePart.Scale.x) + " " + std::to_string(basePart.Scale.y) + " " + std::to_string(basePart.Scale.z);

		mainLogger->Log(scale);
	}

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// light pos doesn't work with directional lights)
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);

	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

	double prevTime = glfwGetTime();
	unsigned int fpsCounter = 0;
	double deltaSecond = 0;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_FRAMEBUFFER_SRGB);

	Camera camera(width, height, glm::vec3(0.0f, 2.0f, 0.0f));

	// Test Skybox
	Skybox skybox = Skybox(&camera, &skyboxShader, "Assets/Images/Skybox");
	   
	// glfwSwapInterval(0);

	while(!glfwWindowShouldClose(window))
	{
		double critTime = glfwGetTime();

		double deltaTime = critTime - prevTime;
		deltaSecond += deltaTime;

		fpsCounter++;

		std::string fps = std::to_string((1.0 / deltaTime) * fpsCounter);
		std::string ms = std::to_string((deltaTime / fpsCounter) * 1000);
		std::string title = "BEngine - " + fps + "FPS / " + ms + "ms";
		glfwSetWindowTitle(window, title.c_str());

		prevTime = critTime;

		if(deltaSecond >= 1)
		{
			fpsCounter = 0;
		}

		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window, deltaTime);

		camera.UpdateMatrixData(70.0f, 0.1f, 1000.0f);

		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shaderProgram, camera);
		}

		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();

		skybox.Update();

		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);

		// Process events on window
		glfwPollEvents();
	}

	shaderProgram.Delete();
	skyboxShader.Delete();

	mainLogger->Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
