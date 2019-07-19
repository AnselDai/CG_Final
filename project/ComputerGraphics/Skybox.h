#pragma once

#ifndef SKYBOX_H
#define SKYBOX_H

#include <iostream>
#include <vector>
#include <stb_image.h>
#include <string>
#include "Shader.h"
#include "Camera.h"
#include "MyClass.h"
#include "MyClass.h"

using namespace std;

extern float skyboxVertices[];

class Skybox {
public:
	Skybox(vector<string> faces, Point_3D min, Point_3D max) {
		this->faces = faces;
		setupTexture();
		setupPlaneVertices(min, max);
		setupVertices();
	}
	void Show();
	~Skybox() {
		glDeleteVertexArrays(1, &skyboxVAO);
		glDeleteBuffers(1, &skyboxVBO);
	}
private:
	unsigned int textureID;
	unsigned int skyboxVAO, skyboxVBO;
	vector<string> faces;
	void setupTexture();
	void setupPlaneVertices(Point_3D min, Point_3D max);
	void setupVertices();
};

#endif // !SKYBOX_H
