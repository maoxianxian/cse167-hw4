#version 330 core

uniform samplerCube skybox;
uniform mat4 V;
in vec3 outNormal;
in vec3 Position;
uniform vec3 cameraPos;
out vec4 color;
void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
	color = texture(skybox, reflected);
}
