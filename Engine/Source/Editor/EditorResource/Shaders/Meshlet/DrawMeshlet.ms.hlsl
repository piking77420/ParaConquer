#include "StaticMeshVertex.hlsl"
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

#define MESHLET_SPACE space3    
StructuredBuffer<Vertex> Vertices : register(t0, MESHLET_SPACE);
StructuredBuffer<Meshlet> Meshlets : register(t1, MESHLET_SPACE);
StructuredBuffer<uint> VertexIndices : register(t2, MESHLET_SPACE);
StructuredBuffer<uint> TriangleIndices : register(t3, MESHLET_SPACE);


struct MeshOutput
{
    float4 Position : SV_POSITION;
#if defined(LIT) || defined(VIEWPOS)
    #if defined(VIEWPOS)
    float3 ViewSpacePosition : TEXCOORD0;
    #elif defined(LIT)
    float3 ViewSpacePosition : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 Tangent : TEXCOORD2;
    #endif
#endif

#if defined(USE_UV)
    float2 TexCoord : TEXCOORD3;
#endif

#if defined(USE_COLOR)
    nointerpolation float3 Color : COLOR0;
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
    uint MeshletID = payload.MeshletIndices[gid.x];
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
        RenderInstance renderInstance = RenderInstances[DrawCall.RenderInstanceID]; 

        uint vertexIndex = VertexIndices[DrawCall.SubMeshTriangleVertexOffset + localVertexIndex];
        Vertex input = Vertices[DrawCall.SubMeshVertexOffset + vertexIndex];

        float4 ViewPos = mul(renderInstance.ModelView, float4(input.Position.xyz, 1.0));
#if defined(LIT) || defined(VIEWPOS)
        vertices[gtid.x].ViewSpacePosition = ViewPos.xyz; // View position
#endif
        vertices[gtid.x].Position = mul(ClipSpaceCorrection, mul(Projection, ViewPos));
 

#if defined(LIT)
    float3 NormalL = input.Normal.xyz;
    float3 TangentL = input.Tangent.xyz;
    float3x3 ModelViewNormalInverseMatrix3 = (float3x3)renderInstance.ModelViewNormalInverseMatrix;
    float3x3 ModelView3 = (float3x3)renderInstance.ModelView;
    
    float3 NormalV = normalize(mul(ModelViewNormalInverseMatrix3, NormalL));
    float3 TangentV = normalize(mul(ModelView3, TangentL));
    TangentV = normalize(TangentV - NormalV * dot(NormalV, TangentV));

    vertices[gtid.x].Normal = NormalV;
    vertices[gtid.x].Tangent = float4(TangentV, input.Tangent.w);
#endif 

    // Need uvs
#if defined(USE_UV)
    vertices[gtid.x].TexCoord = input.TexCoord;
#endif


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