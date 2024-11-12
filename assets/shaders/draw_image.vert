#version 450


#include "include/buffers.glsl"

layout(location = 0) out vec2 texCoords;


const vec2 pos[] =
{
    vec2(-1.0,  1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2( 1.0, -1.0)
};


void main()
{
    texCoords = 0.5 * pos[gl_VertexID] + vec2(0.5);
    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0)

}