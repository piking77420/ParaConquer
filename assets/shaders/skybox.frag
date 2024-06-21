#version 450

layout(location = 0) out vec4 outColor;

layout(set = 0,binding = 1) uniform samplerCube skybox;

layout(location = 0) in vec3 TexCoords;



void main()
{
    vec3 flipCoord = vec3(TexCoords.x,TexCoords.y,TexCoords.z);
    outColor = texture(skybox, flipCoord);  
}