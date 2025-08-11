#version 450

layout(set = SPRITE_SET, binding = SPRITE_TEXTURE) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;


void main()
{
    outColor = texture(texSampler, fragTexCoord);
}