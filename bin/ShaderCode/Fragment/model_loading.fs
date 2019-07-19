#version 330 core
out vec4 FragColor;

struct Light {
	vec3 position;
	// vec3 direction;
	// ШЈжи
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;
};

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;
// mtl
in vec4 Ambient;
in vec4 Diffuse;
in vec4 Specular;

uniform vec3 viewPos;
uniform Light light;

uniform sampler2D shadowMap;

float ShadowCaculation(vec4 fragPosLightSpace) {
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closesDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth? 1.0: 0.0;
		}
	}
	if (projCoords.z > 1.0)
		shadow = 0.0;
	return shadow;
}

void main()
{   
	// ambient
	vec3 ambient = light.ambient * Diffuse.rgb;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * Diffuse.rgb;
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir =  reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), light.shininess);
	vec3 specular = light.specular * spec * Specular.rgb;
	// shadow
	float shadow = 0.0f;
	// float shadow = ShadowCaculation(FragPosLightSpace);

	vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);

    FragColor = vec4(result, 1.0f);
}