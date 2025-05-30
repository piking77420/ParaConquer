#version 450
#include "camera.glsl"


layout(set = SCENE_DESCRIPTOR_SET, binding = 3) uniform AtmosphereBuffer {
    mat4 camToWorldMatrix;
    vec3 viewParams;
    float pad;
    vec3 cameraPos;
    float pad2;
    vec3 sunDir;
    float pad3;
    vec3 lightColor;
    float pad4;
    vec3 scatteringCoeff;
} atmosphereBuffer;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 inUv;



const int SCATTERING_POINT = 10;
const int NumberOfOpticalDepth = 10;

const float AtmosphereRadius = 2000;
const float PlanetRadius = 10;
const float densityFallOff = 4;

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
    vec3 offset = rayOrigin - center;
    float a = 1; // Set to dot(rayDir, rayDir) if rayDir might not be normalized
    float b = 2 * dot(offset, rayDir);
    float c = dot (offset, offset) - radius * radius;
    float d = b * b - 4 * a * c; // Discriminant from quadratic formula

    // Number of intersections: 0 when d < 0; 1 when d = 0; 2 when d > 0
    if (d > 0) {
        float s = sqrt(d);
        float dstToSphereNear = max(0, (-b - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);

        // Ignore intersections that occur behind the ray
        if (dstToSphereFar >= 0) {
            return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }
    // Ray did not intersect sphere
    return vec2(MAX_FLOAT, 0);
}

const float g = -0.85f;
const float gS = g * g; 
float PhaseFunction(float cosAngle)
{
    float t = 3.f * (1.f - gS) / 2.f * (2.f + gS);
    float t2 = (1 + (cosAngle * cosAngle)) / pow((1 + gS - 2 * g * cosAngle), 3 /2);
    
    return t / t2;
}

// Calculates the atmospheric density at a sample point
float densityAtPoint(vec3 planetPos, vec3 samplePoint)
{
    float heightAboveSurface = distance(samplePoint, planetPos) - PlanetRadius;
    float height01 = heightAboveSurface / (AtmosphereRadius - PlanetRadius);
    float density = exp(-height01 * densityFallOff) * (1.0 - height01);
    return density;
}

// Computes the optical depth (total density) along a ray
float ComputeOpticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 planetPos)
{
    vec3 densitySamplePoint = rayOrigin;
    float stepSize = rayLength / float(NumberOfOpticalDepth - 1);
    float opticalLength = 0.0;

    for (int i = 0; i < NumberOfOpticalDepth; i++)
    {
        float localDensity = densityAtPoint(planetPos, densitySamplePoint);
        opticalLength += localDensity * stepSize;
        densitySamplePoint += rayDir * stepSize;
    }

    return opticalLength;
}

// Computes the in-scattered light along a view ray
vec3 ComputeLight(vec3 planetPos, vec3 rayOrigin, vec3 rayDir, float rayLength)
{
    vec3 inScatteredPoint = rayOrigin;
    float stepSize = rayLength / float(SCATTERING_POINT - 1);
    vec3 inScatteringLight = vec3(0.0);

    for (int i = 0; i < SCATTERING_POINT; i++)
    {
        // Ray-sphere intersection to find length of sunlight through the atmosphere
        float sunRayLength = RaySphere(planetPos, AtmosphereRadius, inScatteredPoint, atmosphereBuffer.sunDir).y;

        float sunOpticalDepth = ComputeOpticalDepth(inScatteredPoint, atmosphereBuffer.sunDir, sunRayLength, planetPos);
        float viewOpticalDepth = ComputeOpticalDepth(inScatteredPoint, -rayDir, stepSize * float(i), planetPos);
        //return vec3(viewOpticalDepth);

        vec3 transmittance = exp(-(sunOpticalDepth + viewOpticalDepth) * atmosphereBuffer.scatteringCoeff) ;
        float localDensity = densityAtPoint(planetPos, inScatteredPoint);

        inScatteringLight += localDensity * transmittance * stepSize * atmosphereBuffer.scatteringCoeff;
        inScatteredPoint += rayDir * stepSize;
    }

    return inScatteringLight;
}


void main() 
{
    vec2 flippedUv = vec2(inUv.x, 1.0 - inUv.y);
    vec3 viewPointLocal = vec3(flippedUv - 0.5, 1.0) * atmosphereBuffer.viewParams;
    vec3 viewPoint = vec3(atmosphereBuffer.camToWorldMatrix * vec4(viewPointLocal, 1.0)) ;
    
    vec3 upVector = vec3(0, 1, 0);
    vec3 color = vec3(0, 0, 0);
    
    
    vec3 rayOrigin = atmosphereBuffer.cameraPos;
    vec3 rayDir = normalize(viewPoint - rayOrigin);

    vec3 PlanetPos = vec3(0,0,0);
    vec2 result = RaySphere(PlanetPos, AtmosphereRadius, rayOrigin, rayDir);
    float dstToAtmoSphere = result.x;
    float dstThroughtAtmoSphere = result.y;
    
    if (result.x != MAX_FLOAT)
    {
        const float epsilon = 0.001f;
        vec3 pointInSpehere = rayOrigin + rayDir * (dstToAtmoSphere + epsilon);
        vec3 light = ComputeLight(PlanetPos, pointInSpehere, rayDir, dstThroughtAtmoSphere - epsilon * 2);

        float cosTheta = dot(rayDir, atmosphereBuffer.sunDir);
        //float phase = PhaseFunction(cosTheta);
        color = light  * atmosphereBuffer.lightColor;
    }
    // we miss the atmosphere

    outColor = vec4(color, 1.0);
}