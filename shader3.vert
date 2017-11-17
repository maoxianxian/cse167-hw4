#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 outNormal;
out vec3 Position;


uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

void main()
{
    outNormal = mat3(transpose(inverse(model))) * normal;
	Position = vec3(model * vec4(position, 1.0));
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
}
