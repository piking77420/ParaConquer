#version 450

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform pushConstant 
{
    layout(offset = 0) mat4 meshesMatrixIndex;
    layout(offset = 64) vec3 color;
}ps;

void main()
{
    outColor = vec4(0.0,1.0,0.0,1.0);
}