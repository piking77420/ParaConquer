#ifndef MESH_SHADER_DRAW_CALL_HEADER
#define MESH_SHADER_DRAW_CALL_HEADER


struct MeshShaderDrawCall
{
	float4x4 ModelViewProjection; // 64
	unsigned int SubMeshMeshletCount;
	unsigned int SubMeshMesletOffset; // 4  68
	unsigned int SubMeshVertexOffset;
	unsigned int SubMeshTriangleVertexOffset;
	unsigned int SubMeshTriangleOffset;
};
[[vk::push_constant]]
MeshShaderDrawCall DrawCall;

#endif