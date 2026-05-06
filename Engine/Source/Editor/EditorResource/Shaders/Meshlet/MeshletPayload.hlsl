#ifndef MESH_SHADER_PAYLOAD_HEADER
#define MESH_SHADER_PAYLOAD_HEADER

#define AS_GROUP_SIZE 32
struct Payload
{
	uint MeshletIndices[AS_GROUP_SIZE];
	#if defined(PAYLOAD_COLOR)
	float4 BoundColor[AS_GROUP_SIZE];
	#endif
};

#endif // MESH_SHADER_PAYLOAD_HEADER