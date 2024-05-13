#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

vec2 alpha = {1.0f, 1.1f};

void main()
{
    FragColor = texture(texture1, TexCoord);
}