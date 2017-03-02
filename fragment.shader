#version 330 core

in vec3 newColour;
out vec4 frag_color;

void main()
{
	frag_color = vec4(newColour, 1.0f);
}