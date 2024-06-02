#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool pcfEnabled;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float random(vec4 seed4)
{
   float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
   return fract(sin(dot_product) * 43758.5453);
}

float ShadowCalculation(vec3 fragPos)
{
    float shadow = 0.0;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    //float diskRadius = 0.05;
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
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
    vec3 fragToLight = fragPos - lightPos;
    // Use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;
    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    if (true) 
    {
        float shadow =  ShadowCalculation(fs_in.FragPos);                      
        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
        FragColor = vec4(lighting, 1.0);
    }
    else{
        shadowDebug(fs_in.FragPos);
    }
}