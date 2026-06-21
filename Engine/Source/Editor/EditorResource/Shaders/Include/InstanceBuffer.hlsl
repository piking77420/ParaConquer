#ifndef INSTANCE_BUFFER_INC
#define INSTANCE_BUFFER_INC

struct RenderInstance
{
    float4x4 ModelView;
    float4x4 ModelNormalInverseMatrix;
};

StructuredBuffer<RenderInstance> RenderInstances : register(RENDER_INSTANCE_BUFFER_BINDING, RENDER_INSTANCE_BUFFER_SPACE);


#endif // INSTANCE_BUFFER_INC