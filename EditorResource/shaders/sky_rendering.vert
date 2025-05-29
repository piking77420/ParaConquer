#version 450



vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0), // 0: bottom-left
    vec2( 1.0, -1.0), // 1: bottom-right
    vec2(-1.0,  1.0), // 2: top-left
    vec2(-1.0,  1.0), // 3: top-left
    vec2( 1.0, -1.0), // 4: bottom-right
    vec2( 1.0,  1.0)  // 5: top-right
);
    
void main() 
{
    vec4 pos = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    pos = pos.xyww;
    gl_Position = pos; 
}
