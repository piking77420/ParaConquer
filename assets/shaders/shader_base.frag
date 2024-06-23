#version 450

#include "include/gpu_light_typedef.glsl"

layout(std430, set = 0, binding = 2) buffer GpuLightBuffer  
{
    int nbrOfDirLight;
    int nbrOfPointLight;
    int nbrOfSpotLight;

    GpuDirLight gpuDirLights[MAX_DIRLIGHT_COUNT];
    GpuPointLight gpuPointLights[MAX_POINTLIGHT_COUNT];
    GpuSpotLight gpuSpotLight[MAX_SPOTLIGHT_COUNT];
};


layout(set = 1,binding = 0) uniform sampler2D texSampler;


layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;


layout(location = 0) out vec4 outColor;

vec3 ComputeDirLight()
{
    vec3 result = vec3(0.0);
    
    for (int i = 0; i < nbrOfDirLight; i++)
    {
        float ambient = 0.f;

        // diffuse 
        vec3 norm = normalize(fragNormal);
        vec3 lightDir = normalize(gpuDirLights[i].direction);
        float diff = max(dot(norm, lightDir), 0.0);
        float diffuse = 1.f * diff;

        /*
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = 0.3 * spec;*/

        result += (ambient + diffuse) * gpuDirLights[i].color * gpuDirLights[i].intensity * texture(texSampler, fragTexCoord).rgb;
    }
    
    
    return result;
}


void main()
{
    outColor = vec4(ComputeDirLight(),1.0);
}