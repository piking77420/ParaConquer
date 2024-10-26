#include "rendering/render_harware_interface/descriptor_set.hpp"

PC_CORE::DescriptorSet::DescriptorSet(DescriptorSet&& _other) noexcept
{
    handle = _other.handle;
    _other.handle = nullptr;
}

PC_CORE::DescriptorSet& PC_CORE::DescriptorSet::operator=(DescriptorSet&& _other) noexcept
{
    handle = _other.handle;
    _other.handle = nullptr;
    
    return *this;
}

