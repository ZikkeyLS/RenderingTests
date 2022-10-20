#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 currentPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Unit from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;

// Gets the position of the camera from the main function
uniform vec3 camPos;

#define MAX_LIGHTS 1024
uniform int numLights;
uniform struct Light {
   int type;

   vec3 position;
   vec3 direction;

   vec4 color;
   float ambient;
   float specularLight;

   float valueA;
   float valueB;
} lights[MAX_LIGHTS];

// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;

vec4 CalculatePointLight(Light light)
{
	vec3 lightVec = light.position - currentPos;

	float dist = length(lightVec);
	float a = light.valueA; // 3.0
	float b = light.valueB; // 0.7
	float intencity = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, light.direction), 0.0f);

	// specular lighting
	float specular = 0.0f;

	if(diffuse != 0.0f)
	{
		// specularlight default 0.5
		vec3 viewDirection = normalize(camPos - currentPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayVec = normalize(viewDirection + light.direction);

		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * light.specular;
	}

	return (texture(diffuse0, texCoord) * (diffuse * intencity + light.ambient) + texture(specular0, texCoord).r * specular * intencity) * lightColor;
}

vec4 CalculateDirectionalLight(Light light)
{
	// ambient lighting 0.5f

	// diffuse lighting
	vec3 normal = normalize(Normal);
	// vec3 lightDirection = normalize(vec3(0.0f, 0.0f, 1.0f));
	float diffuse = max(dot(normal, light.direction), 0.0f);

	// specular lighting
	float specular = 0.0f;

	if(diffuse != 0.0f)
	{
		// float specularLight = 0.20f;
		vec3 viewDirection = normalize(camPos - currentPos);
		vec3 reflectionDirection = reflect(-light.direction, normal);

		vec3 halfwayVec = normalize(viewDirection + light.direction);

		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * light.specularLight;
	}

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 CalculateSpotLight(Light light)
{
	// controls how big the area that is lit up is (outerCone = valueA, innerCone = valueB)

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.position - currentPos);
	float diffuse = max(dot(normal, light.direction), 0.0f);

	// specular lighting
	float specular = 0.0f;

	if(diffuse != 0.0f)
	{
		// float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - currentPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayVec = normalize(viewDirection + lightDirection);

		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * light.specularLight;
	}

	// calculates the intensity of the currentPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, 0.0f, -1.0f), -light.direction);
	float inten = clamp((angle - light.valueA) / (light.valueB - light.valueA), 0.0f, 1.0f);

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float LogisticDepth(float depth, float steepness = 0.5f, float offset = 30.0f)
{
	float zVal = LinearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
	if(texture(diffuse0, texCoord).a < 0.1)
		discard;

	// outputs final color
	float depth = LogisticDepth(gl_FragCoord.z);

	vec4 globalLight = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	for(int i=-1; ++i < MAX_LIGHTS; )
	{
		if(lights[i].type == 0) // point
		{
			globalLight = globalLight + CalculatePointLight(lights[i]);
		}
		else if(lights[i].type == 1) // directional
		{
			globalLight = globalLight + CalculateDirectionalLight(lights[i]);
		}
		else if(lights[i].type == 2) // spot
		{
			globalLight = globalLight + CalculateSpotLight(lights[i]);
		}
	}

	FragColor = globalLight * (1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}
