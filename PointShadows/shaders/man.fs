#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D reflection;
    float shininess;
}; 

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform PointLight light;
uniform Material material1;
uniform samplerCube depthMap;

uniform vec3 viewPos;
uniform float far_plane;

uniform bool shadows;
uniform bool pcfEnabled;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    float shadow = 0.0;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    //float diskRadius = 0.05;
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - light.position;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    if (!pcfEnabled)
    {
        float bias = 0.005;
        // Use the light to fragment vector to sample from the depth map    
        float closestDepth = texture(depthMap, fragToLight).r;
        // It is currently in linear range between [0,1]. Re-transform back to original value
        closestDepth *= far_plane;
        // Now test for shadows
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
    else 
    {
        float bias = 0.15;
        for(int i = 0; i < samples; ++i)
        {
            float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
            closestDepth *= far_plane;   // undo mapping [0;1]
            if(currentDepth - bias > closestDepth)
                shadow += 1.0;
        }
        shadow /= float(samples);
    }

    return shadow;
}

void shadowDebug(vec3 fragPos)
{
    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - light.position;
    // Use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;
    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // calculate shadow
    float shadow = shadows ? ShadowCalculation(Position) : 0.0;
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material1.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material1.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material1.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    // apply shadow 
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);
    return (ambient + diffuse + specular);

}

void main()
{             
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - Position);

    if (false) 
    {
        shadowDebug(Position);
    }
    else
    { 
        vec3 result = CalcPointLight(light, normal, Position, viewDir);
        FragColor = vec4(result, 1.0);
    }

}