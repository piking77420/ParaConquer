#ifndef InstanceBuffer
#define InstanceBuffer

struct RenderInstance
{
    float4x4 ModelView;
    float4x4 NormalInverseMatrixView;
};

StructuredBuffer<RenderInstance> RenderInstances : register(RENDER_INSTANCE_BUFFER_BINDING, RENDER_INSTANCE_BUFFER_SPACE);


#endif // InstanceBuffer