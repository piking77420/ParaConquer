#ifndef MESHLET_HEADER
#define MESHLET_HEADER

struct Meshlet
{
	uint VertexOffset;
	uint TriangleOffset;
	uint VertexCount;
	uint TriangleCount;
};

#endif //MESHLET_HEADER