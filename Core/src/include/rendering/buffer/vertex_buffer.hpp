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


    inline uint32_t GetNbrOfVerticies() const 
    {
        return m_VertexCount;
    }

private:
    uint32_t m_VertexCount = 0;
};

END_PCCORE