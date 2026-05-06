#ifndef CAMERA_HEADER_HLSL
#define CAMERA_HEADER_HLSL

cbuffer CameraBuffer : register(CAMERA_BINDING, CAMERA_SET)
{
    float4x4 View;
    float4x4 ViewInv;
    float4x4 Projection;
    float4x4 ProjectionInv;
    float4x4 ViewProjection;
    float4x4 ViewProjectionInv;
    
    float CameraNear;
    float CameraFar;
    
    float DeltaTime;
    
    float Gamma;
    float Exposure;
    
    float2 RenderSize;
    float2 InvRenderSize;

    bool MeshletCulling;
};

#endif // CAMERA_HEADER_HLSL