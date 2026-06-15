#include "MeshletPayload.hlsl"
#include "MeshletDrawCall.hlsl"

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"


#define RENDER_INSTANCE_BUFFER_BINDING t1
#define RENDER_INSTANCE_BUFFER_SPACE space0
#include "InstanceBuffer.hlsl"


#define MESHLET_SPACE space3
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
        uint LocalMeshletIndex = gid.x * AS_GROUP_SIZE + gtid.x;
        bool Valid = LocalMeshletIndex < DrawCall.SubMeshMeshletCount;
        bool isVisible = false;
        uint MeshletIndex = DrawCall.SubMeshMesletOffset + LocalMeshletIndex;
        if (Valid)
        {
            RenderInstance renderInstance = RenderInstances[DrawCall.RenderInstanceID];
            float4 MeshletBound = MeshletBounds[MeshletIndex];
            float4x4 LocalToView = mul(FrustumViewMatrix, mul(ViewInv, renderInstance.ModelView));          // ModelCamera to -> Model -> to frustum view space

            float3 SphereCenterView = mul(LocalToView, float4(MeshletBound.xyz, 1.0f)).xyz;
            
            float Sx = length(float3(LocalToView[0].xyz)); // transformed local X axis
            float Sy = length(float3(LocalToView[1].xyz)); // transformed local Y axis
            float Sz = length(float3(LocalToView[2].xyz)); // transformed local Z axis
            float RadiusView = MeshletBound.w * max(Sx, max(Sy, Sz));

            isVisible = IsInsideOrIntersects(Frustum, SphereCenterView, RadiusView); 
            //isVisible = dtid.x % 2; 
        }

        uint index = WavePrefixCountBits(isVisible);
        if (isVisible)
        {
            sPayload.MeshletIndices[index] = MeshletIndex;
        }

        uint VisibleCount = WaveActiveCountBits(isVisible);    
        DispatchMesh(VisibleCount, 1, 1, sPayload);
    }
    else
    {
        sPayload.MeshletIndices[gtid.x] = dtid.x + DrawCall.SubMeshMesletOffset;

        uint remaining = DrawCall.SubMeshMeshletCount - gid.x * AS_GROUP_SIZE;
        uint dispatchCount = min(remaining, (uint)AS_GROUP_SIZE);
        DispatchMesh(dispatchCount, 1, 1, sPayload);
    }
    

}
