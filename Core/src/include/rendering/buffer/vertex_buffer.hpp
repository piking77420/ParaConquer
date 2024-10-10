#pragma once

#include "gpu_buffer.hpp"
#include "rendering/render_harware_interface/vertex.hpp"

BEGIN_PCCORE

class VertexBuffer : public GPUBuffer
{
public:

    VertexBuffer(VertexBuffer&& _other) noexcept;
   
    VertexBuffer& operator=(VertexBuffer&& _other) noexcept;
    
    VertexBuffer(CommandPool* _transfertPool, const std::vector<Vertex>& _vertices);
    
    VertexBuffer();
    
    ~VertexBuffer();

    
};

END_PCCORE