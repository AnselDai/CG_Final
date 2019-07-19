#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

// mtlÎÄ¼þ
uniform Mat {
	vec4 aAmbient;
	vec4 aDiffuse;
	vec4 aSpecular;
};

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

out vec4 Ambient;
out vec4 Diffuse;
out vec4 Specular;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;

void main() {
	FragPos = vec3(model*vec4(aPos, 1.0f));
	Normal = vec3(transpose(inverse(model))) * aNormal;

	Ambient = aAmbient;
	Diffuse = aDiffuse;
	Specular = aSpecular;

	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
}