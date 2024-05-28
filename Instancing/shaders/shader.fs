#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D reflection;
    float shininess;
}; 

uniform Material material1;

void main()
{
    FragColor = texture(material1.diffuse, TexCoords);   
}