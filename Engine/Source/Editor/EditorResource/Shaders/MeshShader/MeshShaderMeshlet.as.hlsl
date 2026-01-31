#include "MeshShaderPayload.hlsl"
#include "MeshShaderDrawCall.hlsl"

groupshared Payload sPayload;


[numthreads(AS_GROUP_SIZE, 1, 1)]
void Main(
    uint3 gtid : SV_GroupThreadID,
    uint3 dtid : SV_DispatchThreadID,
    uint3 gid : SV_GroupID
)   
{
    if (dtid.x >= DrawCall.SubMeshMeshletCount)
        return;
    sPayload.MeshletIndices[gtid.x] = dtid.x;
    // Assumes all meshlets are visible
    DispatchMesh(AS_GROUP_SIZE, 1, 1, sPayload);
}