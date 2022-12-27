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
// Gets the position of the camera from the main function
uniform vec3 camPos;

// Gets the Texture Unit from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform float directionalLightAmbient;
uniform float directionalLightSpecular;
uniform vec4 directionalLightColor;
uniform vec3 directionalLightDirection;

#define MAX_POINT_LIGHTS 1024
uniform int pointLightsCount;
uniform struct PointLightType {
   int type;

   vec3 position;
   vec3 direction;

   vec4 color;
   float ambient;
   float specularLight;

   float valueA;
   float valueB;
} pointLights[MAX_POINT_LIGHTS];

vec4 PointLight()
{
	vec3 lightVec = vec3(0.5f, 0.5f, 0.5f) - currentPos;

	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float intencity = 1.0f / (a * dist * dist + b * dist + 1.0f);


	// ambient lighting
	float ambient = 0.2f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;

	if(diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - currentPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayVec = normalize(viewDirection + lightDirection);

		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	}

	return (texture(diffuse0, texCoord) * (diffuse * intencity + directionalLightAmbient) + texture(specular0, texCoord).r * specular * intencity) * directionalLightColor;
}


vec4 DirectionalLight()
{
	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(directionalLightDirection);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;

	if(diffuse != 0.0f)
	{
		vec3 viewDirection = normalize(camPos - currentPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayVec = normalize(viewDirection + lightDirection);

		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * directionalLightSpecular;
	}

	return (texture(diffuse0, texCoord) * (diffuse + directionalLightAmbient) + texture(specular0, texCoord).r * specular) * directionalLightColor;
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

    vec4 calculatedLight = vec4(0f, 0f, 0f, 0f);

	//calculatedLight += DirectionalLight();

	for(int i = 0; i < pointLightsCount; i++)
	{
		calculatedLight += PointLight();
	}

	// outputs final color
	float depth = LogisticDepth(gl_FragCoord.z);
	FragColor = calculatedLight * (1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}
