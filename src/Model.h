#pragma once
#include "Shader.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <fstream>
#include <sstream>
#include "MyClass.h"

#ifndef MODEL_H
#define MODEL_H

using namespace std;

class Model {
public:
	/* 模型数据 */
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;
	Point_3D max;
	Point_3D min;
	/* 函数 */
	Model() { }
	Model(string const& path) {
		this->max = Point_3D(-100.0f, -100.0f, -100.0f);
		this->min = Point_3D(100.0f, 100.0f, 100.0f);
		loadModel(path);
	}
	void Draw(Shader& shader) {
		for (unsigned int i = 0; i < meshes.size(); i++) {
			//shader.setVec4("mat.aAmbient", meshes[i].mat.Ka);
			//shader.setVec4("mat.aDiffuse", meshes[i].mat.Kd);
			//shader.setVec4("mat.aSpecular", meshes[i].mat.Ks);
			meshes[i].Draw(shader);
		}
	}
private:
	/* 函数 */
	void loadModel(string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	unsigned int TextureFromFile(const char* path, const string& directory, bool gama = false);
};

#endif