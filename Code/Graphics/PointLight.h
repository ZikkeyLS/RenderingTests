#pragma once
#include "Light.h"

class PointLight : Light
{
public:
	PointLight(float intenA, float intenB);
	float intencityA;
	float intencityB;
};