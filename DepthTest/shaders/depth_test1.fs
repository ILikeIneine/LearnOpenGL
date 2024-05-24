#version 420 core 

layout (depth_greater) out float gl_FragDepth;
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
    gl_FragDepth = gl_FragCoord.z + 0.1;
}