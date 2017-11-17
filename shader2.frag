#version 330 core

out vec4 color;
uniform vec3 id;
void main()
{
    		color = vec4(id, 1.0f);
}