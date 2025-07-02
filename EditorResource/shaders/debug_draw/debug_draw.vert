#version 450
#include "camera.glsl"


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// TO DO use inused float in mat4 to store RGB value
// x x x x
// x x x x
// x x x x
// Y Y Y x
// use Y
// extract Y Y Y 
// replace with 0 0 0

layout(location = 4) in mat4 InstanceMatrix;


 
void main()
{
    mat4 model = mat4(InstanceMatrix);
    vec3 color = vec3(model[3], model[7],model[11]);
    model[3] = 0;
    model[7] = 0;
    model[11] = 0;
    
    vec4 worldPos = model * vec4(inPosition, 1.0);
    gl_Position = camera.vp * worldPos;

    fragColor = color;
}