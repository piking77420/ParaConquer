#pragma once

#include <Map>
#include <Stack>

#include "VulkanHeader.h"
#include "LowRenderer/RhiDescriptorSet.hpp"



namespace Vulkan
{
    class VulkanContext;

    class VulkanDescritptorManager
    {
    public:
        explicit VulkanDescritptorManager(VulkanContext& _Context);

    private:
        VulkanContext& m_Context;

        
    };
}
