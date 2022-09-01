#pragma once
#include "Shader.h"
#include "Camera.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/quaternion.hpp>


class Skybox 
{
public:
	Skybox(Camera* camera, Shader* shader, std::string skyboxFolder);

	void Update();
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO, cubemapTexture;
private:
	Camera* camera;
	Shader* skyboxShader;
	std::string facesCubemap[6];
};
