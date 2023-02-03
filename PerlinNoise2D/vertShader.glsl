#version 440

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 normal;

out vec3 vPos;
out vec3 vColor;
out vec3 vNormal;

uniform mat4 modelMat;
uniform mat4 projMat;
uniform mat4 viewMat;

void main()
{
	vPos = vec4(modelMat * vec4(vertex_pos, 1.0)).xyz;
	vColor = vertex_color;
	vNormal = mat3(modelMat) * normal;

	gl_Position = projMat * viewMat * modelMat * vec4(vertex_pos, 1.0);
}

