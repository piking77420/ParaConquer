#version 450



layout(location = 0) out vec2 fragTexCoord;

vec2 positions[4] = vec2[](
        vec2(-1, -1),
        vec2(1, -1),
        vec2(-1, 1),
        vec2(1, 1)
    );
    vec2 coords[4] = vec2[](
        vec2(0, 0),
        vec2(1, 0),
        vec2(0, 1),
        vec2(1, 1)
    );

void main()  
{
    fragTexCoord = coords[gl_VertexIndex];
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0); 
}