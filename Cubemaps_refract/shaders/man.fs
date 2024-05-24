#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoord;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_reflection1;
    float shininess;
}; 

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform vec3 viewPos;
uniform samplerCube skybox;

uniform DirLight light;
uniform Material material;

void main()
{             
    vec3 viewDir = normalize(viewPos - Position);
    vec3 normal = normalize(Normal);
    // reflection
    vec3 I = -viewDir;
    vec3 R = reflect(I, normal);
    vec3 reflectMap = vec3(texture(material.texture_reflection1, TexCoord)); // where to reflect
    vec3 reflection = texture(skybox, R).rgb * reflectMap;
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
    // diffuse
    vec3 lightDir = normalize(light.direction);
    float diff_influence = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff_influence *  vec3(texture(material.texture_diffuse1, TexCoord));
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec_influence = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec_influence * vec3(texture(material.texture_specular1, TexCoord));
    
    vec3 result =  ambient + diffuse + specular + reflection;

    FragColor = vec4(result, 1.0);
}