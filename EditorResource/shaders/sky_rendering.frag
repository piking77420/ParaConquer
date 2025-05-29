#version 450
#include "camera.glsl"


layout(set = SCENE_DESCRIPTOR_SET, binding = 3) uniform ViewFrustumBuffer {
    mat4 camToWorldMatrix;
    vec3 viewParams;
    float pad;
    vec3 cameraPos;
} frustum;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 inUv;

const float AtmosphereRaddius = 10;
const int SCATTERING_POINT = 10; 


// atmosphere coefficient

    //  Scattering 
const float Raylieght = 5.802;
const float Mie = 3.996;
const float OzoneCoeffScattering = 0;

// return dstToSphere, dstThroughSphere
// if ray origin inside sphere, dstToSphere = 0
// if ray miss , dstToSphere = maxValue; dstThroughSphere = 0
vec2 RaySphere(vec3 center, float radius, vec3 rayOrigin, vec3 rayDir)
{
    vec3 oc = rayOrigin - center;
    float a = dot(rayDir, rayDir);
    float b = dot(oc, rayDir);
    float c = dot(oc, oc) - radius * radius;
    float d = b * b - a * c;

    if (d > 0.0)
    {
        float sqrtD = sqrt(d);
        float t1 = (-b - sqrtD) / a;
        float t2 = (-b + sqrtD) / a;

        // Swap to make sure t1 is the near intersection
        if (t1 > t2) {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }

        // Ignore if both intersections are behind the ray
        if (t2 >= 0.0)
        {
            float nearDist = max(t1, 0.0);
            float hitLength = t2 - nearDist;
            return vec2(nearDist, hitLength);
        }
    }

    return vec2(MAX_FLOAT); // No intersection
}


void main() 
{
    vec2 flippedUv = vec2(inUv.x, 1.0 - inUv.y);
    vec3 viewPointLocal = vec3(flippedUv - 0.5, 1.0) * frustum.viewParams;
    vec3 viewPoint = vec3(frustum.camToWorldMatrix * vec4(viewPointLocal, 1.0)) ;
    
    vec3 upVector = vec3(0, 1, 0);
    vec3 color = vec3(0, 0, 0);
    
    
    vec3 rayOrigin = frustum.cameraPos;
    vec3 rayDir = normalize(viewPoint - rayOrigin);


    vec2 result = RaySphere(vec3(0,0,0), AtmosphereRaddius, rayOrigin, rayDir);

    // we miss the atmosphere
    if (result.x != MAX_FLOAT)
    {
        float raylenght = result.y;
        float stepSize = raylenght / (SCATTERING_POINT - 1);

        for (int i = 0; i < SCATTERING_POINT; i++)
        {
            vec3 samplePoint = rayOrigin + i * stepSize;


            // we miss the sphere
            color += vec3(result.x, result.x, result.x);
        }
        color = vec3(result.y / (AtmosphereRaddius * 2));
    }
    
    outColor = vec4(color, 1.0);
}