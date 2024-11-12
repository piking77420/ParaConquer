#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 0) uniform sampler2D texSampler;


void main() 
{
    outColor = texture(texSampler, fragTexCoord); 
}
