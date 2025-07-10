#version 450
#include "camera.glsl"

layout(location = 0) in vec4 point1;
layout(location = 1) in vec4 point2; // w = distance
layout(location = 2) in vec4 color;



layout(location = 0) out vec3 outColor;


void main()
{
    vec4 currentPoint = gl_VertexIndex == 0 ? point1 : point2;

    vec4 vertexCameraSpace = vec4(currentPoint.x - camera.cameraPos.x, currentPoint.y - camera.cameraPos.y, currentPoint.z - camera.cameraPos.z, 1);
    gl_Position = camera.vp * vertexCameraSpace;
    outColor = color.rgb;
}