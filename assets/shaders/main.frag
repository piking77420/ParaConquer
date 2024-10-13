#version 450

layout(location = 0) in vec3 fragColor;  

layout(location = 0) out vec4 outColor;  

layout(push_constant) uniform constants  
{
    vec4 color;  
} PushConstants;  

void main() 
{
    outColor = vec4(PushConstants.color.rgb, 1.0); 
}
