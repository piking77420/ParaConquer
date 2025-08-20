
cbuffer CameraBuffer : register(CAMERA_BINDING, CAMERA_SPACE)
{
    float4x4 view;
    float4x4 proj;
    float4x4 vp;
    float4x4 vpIn;
    float4x4 viewInv;
    float4x4 projInv;

    float time;
    float deltaTime;
    float nearZ;
    float farZ;

    float3 cameraPos;
};