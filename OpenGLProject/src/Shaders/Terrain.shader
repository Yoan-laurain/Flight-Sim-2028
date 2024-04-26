﻿#shader vertex
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

#define MAX_TEXTURE_UNITS X

uniform sampler2D u_Diffuse[MAX_TEXTURE_UNITS/2];
uniform sampler2D u_Specular[MAX_TEXTURE_UNITS/2];

uniform vec3 u_CamPos;

float when_neq(float x, float y) {
    return float(x != y);
}

vec4 directLight(float _textureIndex)
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

	vec4 diffuse1 = texture(u_Diffuse[0], v_TexCoord);
	vec4 diffuse0 = texture(u_Diffuse[1], v_TexCoord);
	vec4 specular1 = texture(u_Specular[0], v_TexCoord);
	vec4 specular0 = texture(u_Specular[1], v_TexCoord);

	vec4 finalDiffuse;
	vec4 finalSpecular;

	if (_textureIndex > 0.9) {
	    finalDiffuse = diffuse1;
	    finalSpecular = specular1;
	} else if (_textureIndex < 0.5) {
	    finalDiffuse = diffuse0;
	    finalSpecular = specular0;
	} else {
	    finalDiffuse = mix(diffuse0, diffuse1, _textureIndex);
	    finalSpecular = mix(specular0, specular1, _textureIndex);
	}
	return (finalDiffuse * (diffuse + ambient) + finalSpecular.r * specular) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void main()
{
    vec3 normal = normalize(v_Normal);
	
    float dotProduct = dot(v_Normal, vec3(0.0, 1.0, 0.0));
	float textureThreshold =  abs(dotProduct);	
	vec4 tempColor = directLight(textureThreshold);
    tempColor.a = 1.0f;

    color = tempColor;
}