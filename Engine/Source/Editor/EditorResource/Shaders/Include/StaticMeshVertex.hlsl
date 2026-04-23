#ifndef STATIC_MESH_VERTEX_HEADER
#define STATIC_MESH_VERTEX_HEADER

struct Vertex
{
	float4 Position;
	float4 Normal;
	float4 Tangent;
	float2 TexCoord;
    float2 pad;
};

#endif // STATIC_MESH_VERTEX_HEADER