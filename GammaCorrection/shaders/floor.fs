#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;
uniform bool gamma;


vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    // defuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff_fact = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff_fact * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos, fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec_fact = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec_fact = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec_fact * lightColor;

    // attenuation
    float max_distance = 1.5;
    float distance = length(lightPos, fragPos);
    float attenuation = 1.0 / (gamma ? distance * distance: distance);

    diffuse *= attenuation;
    specular *= attenuation;
}

void main()
{           
    vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;


    FragColor = vec4(ambient + diffuse + specular, 1.0);
}