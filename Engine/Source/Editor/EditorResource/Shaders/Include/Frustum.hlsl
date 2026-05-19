#ifndef FRUSTUM_HEADER_HLSL
#define FRUSTUM_HEADER_HLSL

#include "NDC.hlsl"

// CORNERS
#define FRUSTUM_NEAR_TOP_LEFT 0
#define FRUSTUM_NEAR_TOP_RIGHT 1

#define FRUSTUM_NEAR_BOTTOM_RIGHT 2
#define FRUSTUM_NEAR_BOTTOM_LEFT 3

#define FRUSTUM_FAR_TOP_LEFT 4
#define FRUSTUM_FAR_TOP_RIGHT 5

#define FRUSTUM_FAR_BOTTOM_RIGHT 6
#define FRUSTUM_FAR_BOTTOM_LEFT 7


// FACES
#define FRUSTUM_FACE_NEAR 0
#define FRUSTUM_FACE_FAR 1

#define FRUSTUM_FACE_LEFT 2
#define FRUSTUM_FACE_RIGHT 3

#define FRUSTUM_FACE_TOP 4
#define FRUSTUM_FACE_BOTTOM 5


#define FRUSTUM_EPSILON 0.0001f;

struct Plane
{
    float4 NormalAndDistance;
};

struct Frustum 
{
    Plane planes[FRUSTUM_FACE_BOTTOM + 1];
};

Plane PlaneFromPoints(float4 _P1, float4 _P2, float4 _P3)
{
    const float4 V1 = (_P2 - _P1);
    const float4 V2 = (_P3 - _P1);
	float3 Normal = normalize(cross(V1.xyz, V2.xyz));
	float Distance = -dot(Normal, _P1.xyz);

    Plane p;
    p.NormalAndDistance.xyz = Normal;
    p.NormalAndDistance.w = Distance;

    return p;
}

Frustum FromNdcMatrix(float4x4 NdcToSpace)
{
    float4 Corner[8];
    for(int i = 0; i < 8; i++)
    {
        float4 corner4 = mul(NdcToSpace, float4(NdcCorner[i], 1.0f));
        corner4 /= corner4.w;
        corner4.w = 1.0f; // just in case
        Corner[i] = corner4;
    }

    Frustum Frustum;

    // Normals are facing outside
    Frustum.planes[FRUSTUM_FACE_NEAR] = PlaneFromPoints(Corner[FRUSTUM_NEAR_BOTTOM_LEFT], Corner[FRUSTUM_NEAR_TOP_LEFT], Corner[FRUSTUM_NEAR_BOTTOM_RIGHT]);
    Frustum.planes[FRUSTUM_FACE_FAR] = PlaneFromPoints(Corner[FRUSTUM_FAR_BOTTOM_LEFT], Corner[FRUSTUM_FAR_BOTTOM_RIGHT], Corner[FRUSTUM_FAR_TOP_LEFT]);

    Frustum.planes[FRUSTUM_FACE_LEFT] = PlaneFromPoints(Corner[FRUSTUM_NEAR_BOTTOM_LEFT], Corner[FRUSTUM_FAR_BOTTOM_LEFT], Corner[FRUSTUM_FAR_TOP_LEFT]);
    Frustum.planes[FRUSTUM_FACE_RIGHT] = PlaneFromPoints(Corner[FRUSTUM_NEAR_BOTTOM_RIGHT], Corner[FRUSTUM_FAR_TOP_RIGHT], Corner[FRUSTUM_FAR_BOTTOM_RIGHT]);

    Frustum.planes[FRUSTUM_FACE_TOP] = PlaneFromPoints(Corner[FRUSTUM_NEAR_TOP_LEFT], Corner[FRUSTUM_FAR_TOP_LEFT], Corner[FRUSTUM_FAR_TOP_RIGHT]);
    Frustum.planes[FRUSTUM_FACE_BOTTOM] = PlaneFromPoints(Corner[FRUSTUM_NEAR_BOTTOM_LEFT], Corner[FRUSTUM_FAR_BOTTOM_RIGHT], Corner[FRUSTUM_FAR_BOTTOM_LEFT]);

    return Frustum;
}

float SignedDistanceToPlane(Plane _Plane, float3 _Point)
{
    return dot(_Point, _Plane.NormalAndDistance.xyz) + _Plane.NormalAndDistance.w;
} 
 
bool IsInsideOrIntersects(Plane _Plane, float3 _SphereCenterPoint, float _Radius)
{
	return SignedDistanceToPlane(_Plane, _SphereCenterPoint) - _Radius <= FRUSTUM_EPSILON;
}

bool IsInsideOrIntersects(Frustum _Frustum, float3 _SphereCenterPoint, float _Radius)
{
	return IsInsideOrIntersects(_Frustum.planes[0], _SphereCenterPoint, _Radius)
        && IsInsideOrIntersects(_Frustum.planes[1], _SphereCenterPoint, _Radius)
        && IsInsideOrIntersects(_Frustum.planes[2], _SphereCenterPoint, _Radius)
        && IsInsideOrIntersects(_Frustum.planes[3], _SphereCenterPoint, _Radius)
        && IsInsideOrIntersects(_Frustum.planes[4], _SphereCenterPoint, _Radius)
        && IsInsideOrIntersects(_Frustum.planes[5], _SphereCenterPoint, _Radius);
}

#endif