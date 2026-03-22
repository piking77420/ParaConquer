#include "StaticMeshVertex.hlsl"
#include "MeshShaderPayload.hlsl"
#include "Meshlet.hlsl"
#include "StaticMeshVertex.hlsl"
#include "MeshShaderDrawCall.hlsl"


StructuredBuffer<Vertex> Vertices : register(t0, space0);
StructuredBuffer<Meshlet> Meshlets : register(t1, space0);
StructuredBuffer<uint> VertexIndices : register(t2, space0);
StructuredBuffer<uint> TriangleIndices : register(t3, space0);


struct MeshOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

[outputtopology("triangle")]
[numthreads(128, 1, 1)]
void Main(uint3 gtid : SV_GroupThreadID, 
         uint3 gid : SV_GroupID,
         in payload Payload payload,
         out indices uint3 triangles[128],
         out vertices MeshOutput vertices[64])
{
    uint SubmeshMeshlet = payload.MeshletIndices[gid.x];
    
    uint MeshletID = DrawCall.SubMeshMesletOffset + SubmeshMeshlet;
    Meshlet m = Meshlets[MeshletID];
    SetMeshOutputCounts(m.VertexCount, m.TriangleCount);
       
    if (gtid.x < m.TriangleCount)
    {
        //
        // meshopt stores the triangle offset in bytes since it stores the
        // triangle indices as 3 consecutive bytes. 
        //
        // Since we repacked those 3 bytes to a 32-bit uint, our offset is now
        // aligned to 4 and we can easily grab it as a uint without any 
        // additional offset math.
        //
        
        uint packed = TriangleIndices[DrawCall.SubMeshTriangleOffset + m.TriangleOffset + gtid.x];
        uint vIdx0 = (packed >> 0) & 0xFF;
        uint vIdx1 = (packed >> 8) & 0xFF;
        uint vIdx2 = (packed >> 16) & 0xFF;
        triangles[gtid.x] = uint3(vIdx0, vIdx1, vIdx2);
    }

    if (gtid.x < m.VertexCount)
    {
        uint localVertexIndex = m.VertexOffset + gtid.x;
        
        uint vertexIndex = VertexIndices[DrawCall.SubMeshTriangleVertexOffset + localVertexIndex];
        
        vertices[gtid.x].Position = mul(float4(Vertices[DrawCall.SubMeshVertexOffset + vertexIndex].Position.xyz, 1.0), DrawCall.ModelViewProjection);
        float3 color = float3(
            float(gid.x & 1),
            float(gid.x & 3) / 4,
            float(gid.x & 7) / 8);
        vertices[gtid.x].Color = color;
    }
}