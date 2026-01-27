
#include "Meshlet.hlsl"
#include "StaticMeshVertex.hlsl"


StructuredBuffer<float4> Vertices : register(t0, space1);
StructuredBuffer<Meshlet> Meshlets : register(t1, space1);
StructuredBuffer<uint> VertexIndices : register(t2, space1);
StructuredBuffer<uint> TriangleIndices : register(t3, space1);



#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"

struct PushConstant
{
    float4x4 ModelView; // 64
    unsigned int MesletOffset; // 4  68
};

[[vk::push_constant]]
PushConstant pushConstant;

struct MeshOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

[outputtopology("triangle")]
[numthreads(128, 1, 1)]
void Main(uint gtid : SV_GroupThreadID, 
         uint gid : SV_GroupID,
         out indices uint3 triangles[128],
         out vertices MeshOutput vertices[64])
{
    Meshlet m = Meshlets[pushConstant.MesletOffset + gid];
    SetMeshOutputCounts(m.VertexCount, m.TriangleCount);
       
    if (gtid < m.TriangleCount)
    {
        //
        // meshopt stores the triangle offset in bytes since it stores the
        // triangle indices as 3 consecutive bytes. 
        //
        // Since we repacked those 3 bytes to a 32-bit uint, our offset is now
        // aligned to 4 and we can easily grab it as a uint without any 
        // additional offset math.
        //
        
        uint packed = TriangleIndices[m.TriangleOffset + gtid];
        uint vIdx0 = (packed >> 0) & 0xFF;
        uint vIdx1 = (packed >> 8) & 0xFF;
        uint vIdx2 = (packed >> 16) & 0xFF;
        triangles[gtid] = uint3(vIdx0, vIdx1, vIdx2);
    }

    if (gtid < m.VertexCount)
    {
        // meshlet offset + wrap instance
        uint localVertexIndex = m.VertexOffset + gtid;
        
        uint vertexIndex = VertexIndices[localVertexIndex];
        
        vertices[gtid].Position = mul(mul(float4(Vertices[vertexIndex].xyz, 1.0), pushConstant.ModelView), Projection);
        float3 color = float3(
            float(gid & 1),
            float(gid & 3) / 4,
            float(gid & 7) / 8);
        vertices[gtid].Color = color;
    }
}