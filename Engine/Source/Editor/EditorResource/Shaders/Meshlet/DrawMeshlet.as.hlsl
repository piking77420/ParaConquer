#include "MeshletPayload.hlsl"
#include "MeshletDrawCall.hlsl"

groupshared Payload sPayload;

[numthreads(AS_GROUP_SIZE, 1, 1)]
void Main(
    uint3 gtid : SV_GroupThreadID,
    uint3 dtid : SV_DispatchThreadID,
    uint3 gid : SV_GroupID
)
{    
    sPayload.MeshletIndices[gtid.x] = dtid.x;
    // Last group may have fewer meshlets
    uint remaining = DrawCall.SubMeshMeshletCount - gid.x * AS_GROUP_SIZE;
    uint dispatchCount = min(remaining, (uint) AS_GROUP_SIZE);
    DispatchMesh(dispatchCount, 1, 1, sPayload);
}
