#pragma once

#include "rendering/descriptor_layout_binding.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include "resources/Resource.hpp"

BEGIN_PCCORE

class DescriptorSetLayout : public PC_CORE::Resource
{
public:
    ~DescriptorSetLayout() override;

    DescriptorSetLayout() = default;

    DescriptorSetLayout(const std::vector<PC_CORE::DescriptorLayoutBinding>& _descriptorSetLayout);
private:
    PC_CORE::DescriptorSetLayoutHandle m_Handle = nullptr;
};

END_PCCORE