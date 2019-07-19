#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GlobalVariables.h"
#include "MyFunction.h"
#include "Model.h"
#include <stb_image.h>
#include "Skybox.h"
#include <string>
//#include <ft2build.h>
//#define FT_FREETYPE_H

using namespace std;

int main() {
	GLFWwindow* window;
	bool state;
	state = init_GLFW_window(window);
	if (!state) return -1;
	camera = Camera(width, height);
	Shader shader("ShaderCode/Vertex/model_loading.vs", "ShaderCode/Fragment/model_loading.fs");
	Shader lampShader("ShaderCode/Vertex/lamp.vs", "ShaderCode/Fragment/lamp.fs");
	Shader planeShader("ShaderCode/Vertex/plane.vs", "ShaderCode/Fragment/plane.fs");
	Shader depthShader("ShaderCode/Vertex/Shadow_Depth.vs", "ShaderCode/Fragment/Shadow_Depth.fs");
	Shader debugDepthQuad("ShaderCode/Vertex/Quad.vs", "ShaderCode/Fragment/Quad.fs");
	// 加载模型
	int num_model = 6;
	vector<Model> models;
	for (unsigned int i = 1; i <= num_model; i++) {
		cout << "part" << i << endl;
		models.push_back(Model(("./Resource/model/final/final" + to_string(i) + ".obj").c_str()));
		cout << "finish" << endl;
	}
	Point_3D min = models[0].min, max = models[0].max;
	for (unsigned int i = 1; i < num_model; i++) {
		if (models[i].min.x < min.x) min.x = models[i].min.x;
		if (models[i].min.y < min.y) min.y = models[i].min.y;
		if (models[i].min.z < min.z) min.z = models[i].min.z;
		if (models[i].max.x > max.x) max.x = models[i].max.x;
		if (models[i].max.y > max.y) max.y = models[i].max.y;
		if (models[i].max.z > max.z) max.z = models[i].max.z;
	}
	cout << "min: " << min.x << ", " << min.y << ", " << min.z << endl;
	cout << "max: " << max.x << ", " << max.y << ", " << max.z << endl;
	
	//vector<Model> snow;
	//int num_snow = 3;
	//for (int i = 0; i < num_snow; i++) {
	//	cout << "snow part " << i + 1 << endl;
	//	snow.push_back(Model(("Resource/model/snow/finalWinter" + to_string(i + 1) + ".obj").c_str()));
	//	cout << "finish" << endl;
	//}

	// -----------------天空盒----------------
	cout << "set up skybox" << endl;
	Shader skyboxShader("./ShaderCode/Vertex/skybox.vs", "./ShaderCode/Fragment/skybox.fs");
	vector<std::string> faces
	{
	  "./Resource/texture/skybox3/right.jpg",
	  "./Resource/texture/skybox3/left.jpg",
	  "./Resource/texture/skybox3/top.jpg",
	  "./Resource/texture/skybox3/bottom.jpg",
	  "./Resource/texture/skybox3/front.jpg",
	  "./Resource/texture/skybox3/back.jpg"
	};
	Skybox skybox(faces, min, max);
	cout << "finish" << endl;

	// ------------------阴影----------------
	// 1. 为渲染的深度贴图创建一个帧缓冲对象
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// 2. 创建一个2D纹理，提供给帧缓冲的深度缓冲使用
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 3. 将生成的深度纹理作为帧缓冲的深度缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	// glDepthFunc(GL_LESS);

	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);

	cout << "Begin to render" << endl;
	while (!glfwWindowShouldClose(window)) {
		// 处理输入
		process_input(window);
		// 处理鼠标
		glfwSetCursorPosCallback(window, mouse_callback);
		// 处理滚轮
		glfwSetScrollCallback(window, scroll_callback);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.updateSpeed();
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		float near_plane = 1.0f, far_plane = 200.0f;

		//lightPos.x += glm::sin(glfwGetTime()) * 100;
		//lightPos.y += glm::cos(glfwGetTime()) * 100;

		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		model = glm::mat4(1.0f);
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		depthShader.setMat4("model", model);
		// 1. 渲染深度贴图
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glCullFace(GL_FRONT);
			for (unsigned int i = 0; i < num_model; i++) {
				// cout << "depth" << i << endl;
				models[i].Draw(depthShader);
			}
			glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		debugDepthQuad.use();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		// render_quad();

		// 渲染模型
		shader.use();
		projection = glm::perspective(glm::radians(camera.getfov()), float(width) / height, near_plane, far_plane);
		view = camera.getViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		shader.setVec3("light.position", lightPos);
		shader.setVec3("light.ambient", ambient, ambient, ambient);
		shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
		shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("light.shininess", 64);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		for (unsigned int i = 0; i < num_model; i++) {
			// cout << "model" << i << endl;
			models[i].Draw(shader);
		}

		// 设置光源
		lampShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(1.0f));
		lampShader.setMat4("model", model);
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		//render_cube();

		// 设置平面
		//planeShader.use();
		//model = glm::mat4(1.0f);
		//cout << Mymodel.min_y << endl;
		//model = glm::translate(model, glm::vec3(0.0f, Mymodel.min_y - 5.0f, 0.0f));
		//view = camera.getViewMatrix();
		//projection = glm::perspective(glm::radians(camera.getfov()), (float)width / (float)height, near_plane, far_plane);
		//planeShader.setMat4("view", view);
		//planeShader.setMat4("model", model);
		//planeShader.setMat4("projection", projection);
		//render_plane();

		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		skyboxShader.setInt("skybox", 0);
		view = glm::mat4(glm::mat3(camera.getViewMatrix()));
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		skybox.Show();
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}