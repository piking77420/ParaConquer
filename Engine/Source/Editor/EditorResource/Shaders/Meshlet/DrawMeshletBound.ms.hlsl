
#include "StaticMeshVertex.hlsl"
#define PAYLOAD_COLOR 1
#include "MeshletPayload.hlsl"
#include "Meshlet.hlsl"
#include "StaticMeshVertex.hlsl"
#include "MeshletDrawCall.hlsl"
#include "HashColor.hlsl"
  

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"


#define RENDER_INSTANCE_BUFFER_BINDING t1
#define RENDER_INSTANCE_BUFFER_SPACE space0
#include "InstanceBuffer.hlsl"

#define DEBUG_MODEL_BOUNDS space2    
StructuredBuffer<float4> Vertices : register(t2, space0);
StructuredBuffer<uint> Indicies : register(t3, space0);

StructuredBuffer<float4> MeshletBounds : register(t0, space1); // center, radius


struct MeshOutput
{
    float4 Position : SV_POSITION;

#if defined(USE_COLOR)
    nointerpolation float3 Color : COLOR0;
#endif
};

#define WIRE_SPHERE_VERTEX_COUNT 144
#define WIRE_SPHERE_INDICIES_COUNT 150
#define WIRE_SPHERE_LINE_COUNT 75

[outputtopology("line")]
[numthreads(WIRE_SPHERE_VERTEX_COUNT, 1, 1)]
void Main(uint3 gtid : SV_GroupThreadID, 
         uint3 gid : SV_GroupID,
         in payload Payload payload,
         out indices uint2 lines[WIRE_SPHERE_LINE_COUNT],
         out vertices MeshOutput vertices[WIRE_SPHERE_VERTEX_COUNT])
{
    uint MeshletIndex = payload.MeshletIndices[gid.x];
    float4 MeshletBound = MeshletBounds[MeshletIndex];
    
    SetMeshOutputCounts(WIRE_SPHERE_VERTEX_COUNT, WIRE_SPHERE_LINE_COUNT);
       
    if (gtid.x < WIRE_SPHERE_LINE_COUNT)
    {
           lines[gtid.x] = uint2(
            Indicies[gtid.x * 2 + 0],
            Indicies[(gtid.x * 2 + 1)]
        );
    }

    if (gtid.x < WIRE_SPHERE_VERTEX_COUNT)
    {
        RenderInstance renderInstance = RenderInstances[DrawCall.RenderInstanceID];
        float3 LocalPos = Vertices[gtid.x].xyz * MeshletBound.w + MeshletBound.xyz;
        float4 ViewPos = mul(renderInstance.ModelView, float4(LocalPos, 1.0f));
        vertices[gtid.x].Position = mul(Projection, ViewPos);
        #if defined(USE_COLOR)
        float3 color = float3(
            float(gid.x & 1 ),
            float(gid.x & 3 ) / 4,
            float(gid.x & 7 ) / 8);
            vertices[gtid.x].Color = color.xyz;
        #endif
    }
}