#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 modelview;
out vec3 normalvecin;
uniform mat4 model;

void main()
{
	normalvecin= normalize(mat3(transpose(inverse(model)))*normalize(normal));
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
}