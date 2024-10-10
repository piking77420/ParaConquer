#pragma once

#include "vulkan_header.h"
#include "render_harware_interface/rhi_typedef.h"
#include <spirv_reflect.h>

namespace VK_NP
{
    class VulkanShaderCompiler
    {
    public:
        VulkanShaderCompiler();

        ~VulkanShaderCompiler();

        void CreateModuleFromSource(vk::Device _device, const char* _source,
                                    PC_CORE::LowLevelShaderStageType _lowLevelShaderStage,
                                    SpvReflectShaderModule* _ReflectedModule, vk::ShaderModule* _shaderModule);

    private:
    };
}
