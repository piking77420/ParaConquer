#include "resources/descriptor_set_layout.h"

#include "rendering/render_harware_interface/RHI.hpp"

using namespace PC_CORE;

DescriptorSetLayout::~DescriptorSetLayout()
{
    if (m_Handle != NULL_HANDLE)
    {
        RHI::GetInstance().DestroyDescriptorSetLayout(m_Handle);
        m_Handle = NULL_HANDLE;
    }
}

DescriptorSetLayout::DescriptorSetLayout(const std::vector<DescriptorLayoutBinding>& _descriptorSetLayout)
{
    m_Handle = RHI::GetInstance().CreateDescriptorSetLayout(_descriptorSetLayout);
}
