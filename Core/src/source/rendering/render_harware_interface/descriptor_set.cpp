#include "rendering/render_harware_interface/descriptor_set.hpp"

PC_CORE::DescriptorSet::DescriptorSet(DescriptorSet&& _other) noexcept
{
    m_Handle = _other.m_Handle;
    _other.m_Handle = nullptr;
}

PC_CORE::DescriptorSet& PC_CORE::DescriptorSet::operator=(DescriptorSet&& _other) noexcept
{
    m_Handle = _other.m_Handle;
    _other.m_Handle = nullptr;
}

