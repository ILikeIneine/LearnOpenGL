#version 330 core

out vec4 FragColor;
in vec4 ourColor;

uniform vec4 uniColor;

void main()
{
    FragColor = ourColor;
}