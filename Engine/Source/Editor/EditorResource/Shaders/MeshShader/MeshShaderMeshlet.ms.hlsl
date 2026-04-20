#include "StaticMeshVertex.hlsl"
#include "MeshShaderPayload.hlsl"
#include "Meshlet.hlsl"
#include "StaticMeshVertex.hlsl"
#include "MeshShaderDrawCall.hlsl"
#include "HashColor.hlsl"

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"


StructuredBuffer<Vertex> Vertices : register(t0, space1);
StructuredBuffer<Meshlet> Meshlets : register(t1, space1);
StructuredBuffer<uint> VertexIndices : register(t2, space1);
StructuredBuffer<uint> TriangleIndices : register(t3, space1);


struct MeshOutput
{
    float4 Position : SV_POSITION;
#if defined(USE_COLOR)
    float3 Color : COLOR;
#endif
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
        float3 Verticies = Vertices[DrawCall.SubMeshVertexOffset + vertexIndex].Position.xyz;
        
        vertices[gtid.x].Position = mul(mul(float4(Verticies, 1.0), DrawCall.ModelView), Projection);
#if defined(USE_COLOR)
        float3 color = float3(0,0,0);
        uint id = 0;
#if defined(DRAW_TRIANGLE)
    uint InstanceID = 1;
    uint VertexID = vertexIndex;
    uint combined = InstanceID * 73856093u ^ VertexID * 19349663u;
    vertices[gtid.x].Color = float4(hash3(combined), 1.0);
    /*color = float3(
            float(gtid.x & 1 ),
            float(gtid.x & 3 ) / 4,
            float(gtid.x & 7 ) / 8);
        vertices[gtid.x].Color = color;*/
#else
    color = float3(
            float(gid.x & 1 ),
            float(gid.x & 3 ) / 4,
            float(gid.x & 7 ) / 8);
        vertices[gtid.x].Color = color;
#endif // DRAW_TRIANGLE
        
#endif // USE_COLOR
    }
}