#include "Rendering/Buffer/UniformBuffer.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::UniformBuffer::UniformBuffer(PC_CORE::Rhi& rhi, const std::string& _name, size_t _size)
{

    m_RhiBuffer.reset(rhi.CreateBuffer(_name));
    m_RhiBuffer
        ->SetSize(_size)
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Uniform);
}