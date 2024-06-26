#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D reflection;
    float shininess;
}; 

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform vec3 viewPos;
uniform sampler2D shadowMap;

uniform DirLight light;
uniform Material material1;
uniform bool poisson;
uniform bool biasEnabled;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec4 seed4)
{
   float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
   return fract(sin(dot_product) * 43758.5453);
}

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
    {
        return 0.0;
    }
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    if (poisson)
    {
        for (int i = 0; i < 4; ++i)
        {
            int index = int(16.0 * random(vec4(fragPosLightSpace.xyy, i))) % 16;
            if(texture(shadowMap, projCoords.xy + poissonDisk[index] * texelSize).r < currentDepth - bias)
                shadow += 1.0;
        }
        shadow /= 4.0;
    }
    else 
    {
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
    }

    return shadow;
}

void main()
{             
    vec3 viewDir = normalize(viewPos - Position);
    vec3 normal = normalize(Normal);
    // reflection
    /*
    vec3 I = -viewDir;
    vec3 R = reflect(I, normal);
    vec3 reflectMap = vec3(texture(material1.reflection, TexCoord)); // where to reflect
    vec3 reflection = texture(skybox, R).rgb * reflectMap;
    */
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material1.diffuse, TexCoord));
    // diffuse
    vec3 lightDir = normalize(light.direction);
    float diff_influence = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff_influence *  vec3(texture(material1.diffuse, TexCoord));
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec_influence = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec_influence * vec3(texture(material1.specular, TexCoord));

    // calculate shadow
    float bias = 0.0f;
    if (biasEnabled)
    {
        bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    }
    float shadow = ShadowCalculation(FragPosLightSpace, bias);
    
    vec3 result =  ambient + (1.0 - shadow) * (diffuse + specular);

    FragColor = vec4(result, 1.0);
}