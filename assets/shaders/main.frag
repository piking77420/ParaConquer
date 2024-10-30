#version 450

layout(location = 0) in vec3 fragColor;  
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float time;


layout(location = 0) out vec4 outColor;  

layout(binding = 1) uniform sampler2D texSampler;


void main() 
{
    outColor = texture(texSampler, mix(fragTexCoord * tan(time),fragTexCoord * tan(time) + sin(time),time )); 
}
