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

    uint keepCulling = MeshletCulling;
    float4 keepBounds = MeshletBounds[0];

    sPayload.MeshletIndices[gtid.x] = dtid.x + DrawCall.SubMeshMesletOffset;

    uint remaining = DrawCall.SubMeshMeshletCount - gid.x * AS_GROUP_SIZE;
    uint dispatchCount = min(remaining, (uint)AS_GROUP_SIZE);

    if (keepCulling == 0xFFFFFFFFu || any(isnan(keepBounds)))
    {
        dispatchCount = 0;
    }

    DispatchMesh(dispatchCount, 1, 1, sPayload);

}
