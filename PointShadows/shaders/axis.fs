#version 330 core

in vec3 Color;
out vec4 Frag;

void main()
{
    Frag = vec4(Color, 1.0);
}