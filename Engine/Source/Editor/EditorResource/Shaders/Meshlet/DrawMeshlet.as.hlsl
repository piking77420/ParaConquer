#include "MeshletPayload.hlsl"
#include "MeshletDrawCall.hlsl"

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"

#define MESHLET_SPACE space2
StructuredBuffer<float4> MeshletBounds : register(t4, MESHLET_SPACE);

groupshared Payload sPayload;

[numthreads(AS_GROUP_SIZE, 1, 1)]
void Main(
    uint3 gtid : SV_GroupThreadID,
    uint3 dtid : SV_DispatchThreadID,
    uint3 gid : SV_GroupID
)
{
    if (MeshletCulling)
    {
        DispatchMesh(MeshletBounds[0].x, MeshletBounds[0].y, MeshletBounds[0].z, sPayload);
    }
    else
    {
        sPayload.MeshletIndices[gtid.x] = dtid.x;
        // Last group may have fewer meshlets
        uint remaining = DrawCall.SubMeshMeshletCount - gid.x * AS_GROUP_SIZE;
        uint dispatchCount = min(remaining, (uint) AS_GROUP_SIZE);
        DispatchMesh(dispatchCount, 1, 1, sPayload);
    }
    
    
}
