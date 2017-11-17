#version 330 core
// This is a sample fragment shader.
uniform samplerCube skybox;
in vec3 fragTexCoord;
// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;
void main()
{
	color = texture(skybox, fragTexCoord);
}
