#include "rendering/render_harware_interface/descriptor_pool.hpp"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::DescriptorPool::DescriptorPool(DescriptorPool&& _other) noexcept
{
    m_Handle = _other.m_Handle;
    _other.m_Handle = nullptr;
}

PC_CORE::DescriptorPool& PC_CORE::DescriptorPool::operator=(DescriptorPool&& _other) noexcept
{
    m_Handle = _other.m_Handle;
    _other.m_Handle = nullptr;

    return *this;
}

void PC_CORE::DescriptorPool::AllocDescriptorSet(DescriptorSet* descriptorSet, uint32_t descriptorSetCount,
    const DescriptorSetLayout& descriptorSetLayoutayout)
{
    RHI::GetInstance().AllocDescriptorSet(descriptorSet,descriptorSetCount, m_Handle, descriptorSetLayoutayout.GetHandle());
}

PC_CORE::DescriptorPool::DescriptorPool(const DescriptorPoolSize* desciptorPoolSize, uint32_t descriptorCount,
                                        uint32_t maxSets)
{
    m_Handle = RHI::GetInstance().CreateDescriptorPool(desciptorPoolSize, descriptorCount, maxSets);
}

PC_CORE::DescriptorPool::~DescriptorPool()
{
    if (m_Handle != nullptr)
    {
        RHI::GetInstance().DestroyDescriptorPool(m_Handle);
        m_Handle = nullptr;
    }
}
