#version 330 core

out vec4 color;
in vec3 normalvecin;
uniform int id;
void main()
{
    		color = vec4(id/255.0f, 0, 0, 1.0f);
}