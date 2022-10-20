#pragma once

#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

class Light
{
public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec4 color;
	float ambient;
	float specular;
protected:
	int type;
};

#endif