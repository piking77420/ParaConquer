#ifndef MESH_SHADER_DRAW_CALL_HEADER
#define MESH_SHADER_DRAW_CALL_HEADER


struct MeshShaderDrawCall
{
	unsigned int RenderInstanceID;
	unsigned int SubMeshMeshletCount; 
	unsigned int SubMeshMesletOffset;
	unsigned int SubMeshVertexOffset; 
	unsigned int SubMeshTriangleVertexOffset; 
	unsigned int SubMeshTriangleOffset; 
};
[[vk::push_constant]]
MeshShaderDrawCall DrawCall;

#endif