#pragma once
#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "Shader.h"

using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Material {
	glm::vec4 Ka;
	glm::vec4 Kd;
	glm::vec4 Ks;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	/* �������� */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	Material mat;
	unsigned int uniformBlockIndex;
	/* ���� */
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material mat) {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->mat = mat;
		setupMesh();
	}
	void Draw(Shader& shader);
private:
	/* ��Ⱦ���� */
	unsigned int VAO, VBO, EBO;
	/* ���� */
	void setupMesh();
};

#endif