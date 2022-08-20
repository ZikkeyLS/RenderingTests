#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <json/json.h>

#include "Mesh.h"

using json = nlohmann::json;

class ModelPart
{
public:
	ModelPart(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);

	std::vector<Vertex>& GetVertices();
	std::vector<GLuint>& GetIndices();
	std::vector<Texture>& GetTextures();

	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat Rotation;
	glm::vec3 Scale;
	glm::mat4 Matrix;
private:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
};

class Model 
{
public:
	Model(const char* modelPath);
	std::vector<ModelPart>& GetModelParts();
private:
	std::string path;
	std::string directory;
	std::vector<ModelPart> modelParts;
	std::vector<Texture> texturesLoaded;

	void LoadModel(const char* path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	ModelPart ProcessModelPart(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};

#endif