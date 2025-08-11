#version 450



vec2 positions[4] = vec2[](
    vec2(-1.0, -1.0), // 0: bottom-left
    vec2( 1.0, -1.0), // 1: bottom-right
    vec2(-1.0,  1.0), // 2: top-left
    vec2( 1.0,  1.0)  // 3: top-right
);

vec2 coords[4] = vec2[](
vec2(0.0, 0.0), // bottom-left
vec2(1.0, 0.0), // bottom-right
vec2(0.0, 1.0), // top-left
vec2(1.0, 1.0)  // top-right
);

layout(location = 0) out vec2 uv;

    
void main() 
{
    int index = gl_VertexIndex;

    vec4 pos = vec4(positions[index], 0.0, 1.0);
    pos = pos.xyww;
    gl_Position = pos;
    uv = coords[index];
}
