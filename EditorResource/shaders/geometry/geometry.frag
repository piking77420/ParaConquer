#version 450

// Outputs vers le framebuffer (G-Buffer)
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outNormal;
layout(location = 2) out vec4 outRoughnessMetallicAo;
layout(location = 3) out vec4 outWorldPosition;

// Inputs du vertex shader
layout(location = 0) in vec2 inNormal;
layout(location = 1) in vec4 inRoughnessMetallicAo;
layout(location = 2) in vec4 inWorldPosition;
layout(location = 3) in vec2 inTexCoord;

// Texture albedo
layout(set = MATERIAL_DESCRIPTOR_SET, binding = ALBEDO_BINDING) uniform sampler2D u_AlbedoTexture;

void main()
{
    outColor = texture(u_AlbedoTexture, inTexCoord);
    outNormal = inNormal;
    outRoughnessMetallicAo = inRoughnessMetallicAo;
    outWorldPosition = inWorldPosition;
}
