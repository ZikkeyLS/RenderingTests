#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Graphics/Mesh.h" 
#include "Graphics/Model.h"

const unsigned int width = 800;
const unsigned int height = 800;

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

unsigned int samples = 5;

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

		Logger::GetMain()->Log(scale);
	}


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
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

	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		"Assets/Images/Skybox/right.jpg",
		"Assets/Images/Skybox/left.jpg",
		"Assets/Images/Skybox/top.jpg",
		"Assets/Images/Skybox/bottom.jpg",
		"Assets/Images/Skybox/front.jpg",
		"Assets/Images/Skybox/back.jpg"
	};

	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	   
	// glfwSwapInterval(0);

	Camera camera(width, height, glm::vec3(0.0f, 2.0f, 0.0f));

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

		glEnable(GL_BLEND);

		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shaderProgram, camera);
		}

		glDisable(GL_BLEND);

		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();
		glm::mat4 rotation = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		rotation = glm::mat4_cast(glm::quat(1.0f, glm::radians(40.0f), glm::radians(0.0f), glm::radians(0.0f)));

		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.1f, 1000.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);

		// Process events on window
		glfwPollEvents();
	}

	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
