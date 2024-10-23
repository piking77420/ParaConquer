#include "rendering/buffer/uniform_buffer.hpp"
#include <rendering/render_harware_interface/RHI.hpp>

using namespace PC_CORE;

PC_CORE::UniformBuffer::UniformBuffer(UniformBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    m_MapData = _other.m_MapData;

    _other.handleId = nullptr;
    _other.size = 0;
    _other.m_MapData = nullptr;
}

PC_CORE::UniformBuffer& PC_CORE::UniformBuffer::operator=(UniformBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    m_MapData = _other.m_MapData;

    _other.handleId = nullptr;
    _other.size = 0;
    _other.m_MapData = nullptr;

    return *this;
}


void UniformBuffer::Update(size_t _size, size_t _offset, const void* _data)
{
    memcpy(static_cast<uint8_t*>(m_MapData) + _offset, _data, _size);
}

UniformBuffer::UniformBuffer(CommandPool* _commandPool, size_t _size)
{
    size = _size;
    handleId = RHI::GetInstance()->BufferData(_commandPool, _size, BUFFER_USAGE_UNIFORM);
    RHI::GetInstance()->MapData(handleId, &m_MapData);
}

UniformBuffer::UniformBuffer()
{

}

UniformBuffer::~UniformBuffer()
{
    if (handleId == nullptr && size == 0)
        return;

    size = 0;

    RHI* instance = RHI::GetInstance();
    if (instance != nullptr)
    {
        instance->UnMapData(handleId);
        instance->DestroyBuffer(handleId);
    }

    handleId = nullptr;
}
