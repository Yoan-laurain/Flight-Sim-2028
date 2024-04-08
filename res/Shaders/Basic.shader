#shader vertex
#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 colors;
layout (location = 3) in vec2 texCoord;
layout (location = 4) in int indexModel;
layout (location = 5) in int indexDiffuse;
layout (location = 6) in int indexSpecular;

struct TransformData
{
    mat4 transform;
    mat4 rotation;
	mat4 scale;
	mat4 model;
};

layout(std430, binding = 7) buffer TransformBuffer { TransformData Data[]; } values;

out vec3 v_currentPosition;
out vec3 v_Normal;
out vec2 v_TexCoord;
flat out int v_IndexDiffuse;
flat out int v_IndexSpecular;

uniform mat4 u_CamMatrix;

void main()
{
	v_currentPosition = vec3(values.Data[indexModel].model * values.Data[indexModel].transform * values.Data[indexModel].rotation * values.Data[indexModel].scale * vec4(position, 1.0f));
	v_Normal = normal;
	v_TexCoord = mat2(0.0, -1.0, 1.0, 0.0) * texCoord;
	v_IndexDiffuse = indexDiffuse;
	v_IndexSpecular = indexSpecular;

	gl_Position = u_CamMatrix * vec4(v_currentPosition, 1.0);
}

#shader fragment
#version 460 core

out vec4 color;

in vec3 v_currentPosition;
in vec3 v_Normal;
in vec2 v_TexCoord;
flat in int v_IndexDiffuse;
flat in int v_IndexSpecular;

uniform sampler2D u_Diffuse[20];
uniform sampler2D u_Specular[20];

uniform vec3 u_CamPos;

float when_neq(float x, float y) {
    return float(x != y);
}

vec4 directLight()
{
	float ambient = 1.0f;

	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDir), 0.0f);

	float specularLight = 0.5f;
	vec3 viewDir = normalize(u_CamPos - v_currentPosition);
	vec3 halfway = normalize(viewDir + lightDir);
	float specAmount = pow(max(dot(normal, halfway), 0.0f), 16);
	float specular = specAmount * specularLight * when_neq(diffuse, 0);

	return (texture(u_Diffuse[v_IndexDiffuse], v_TexCoord) * (diffuse + ambient) + texture(u_Specular[v_IndexSpecular], v_TexCoord).r * specular ) * vec4(1.0f,1.0f,1.0f,1.0f);
}

void main()
{
	vec4 tempColor = directLight();
	tempColor.a = 1.0f;

	color = tempColor;
}