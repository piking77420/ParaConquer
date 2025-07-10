#version 450


layout(location = 0) out vec2 fragTexCoord;

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
void main() {
    fragTexCoord = coords[gl_VertexIndex];
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}