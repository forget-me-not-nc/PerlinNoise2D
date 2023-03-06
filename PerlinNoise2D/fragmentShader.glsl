#version 440

in vec3 vPos;
in vec3 vColor;
in vec3 vNormal;

out vec4 fColor;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
	vec3 ambientLight = vec3(0.05, 0.05, 0.05);
	vec3 lightNormal = normalize(lightPos - vPos);
	vec3 diffuseColor  = vec3(1.0, 1.0, 1.0);

	float diffuse = clamp(dot(lightNormal, vNormal), 0.0, 1.0);

	diffuseColor = diffuseColor * diffuse;
	fColor = vec4(vColor, 1.0) * (vec4(ambientLight, 1.0) + vec4(diffuseColor, 1.0));
}
