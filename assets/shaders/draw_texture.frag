#version 450

layout(location = 0) out vec4 outColor;

layout(set = 0,binding = 0) uniform sampler2D texSampler;

layout(location = 2) in vec2 fragTexCoord;

void main()
{
    outColor = texture(texSampler,fragTexCoord);
}