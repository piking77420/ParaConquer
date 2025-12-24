#include "Rendering/Buffer/IndexBuffer.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiBuffer.h"

namespace PC_CORE
{
    IndexBuffer::IndexBuffer(Rhi& rhi, const std::string& _name, size_t _indexCount, RhiBuffer::IndexFormat _indexFormat)
        : m_RhiBufferFormat(_indexFormat)
        , m_IndiciesCount(_indexCount)
    {
        m_RhiBuffer.reset(rhi.CreateBuffer());

        m_RhiBuffer
            ->SetSize(_indexCount * static_cast<size_t>(_indexFormat))
            .SetName(_name);

    }

    IndexBuffer::IndexBuffer(Rhi& rhi, std::string&& _name, size_t _indexCount, RhiBuffer::IndexFormat _indexFormat)
        : m_RhiBufferFormat(_indexFormat)
        , m_IndiciesCount(_indexCount)
    {
        m_RhiBuffer.reset(rhi.CreateBuffer());
        m_RhiBuffer
            ->SetSize(_indexCount * static_cast<size_t>(_indexFormat))
            .SetName(std::forward<std::string>(_name));
    }
}
