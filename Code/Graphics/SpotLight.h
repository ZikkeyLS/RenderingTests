#pragma once
#include "Light.h"

class SpotLight : Light
{
public:
	SpotLight(float inner, float outer);
	float innerCone;
	float outerCone;
};