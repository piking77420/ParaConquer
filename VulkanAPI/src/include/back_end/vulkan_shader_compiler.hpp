﻿#pragma once

#include "vulkan_header.h"
#include "rhi_typedef.h"
#include <spirv_reflect.h>

namespace VK_NP
{
    class VulkanShaderCompiler
    {
    public:
        VulkanShaderCompiler();

        ~VulkanShaderCompiler();

        void CreateModuleFromSource(const char* _source,
                                    PC_CORE::LowLevelShaderStageType _lowLevelShaderStage,
                                    SpvReflectShaderModule* _ReflectedModule, vk::ShaderModule* _shaderModule);

    private:
        vk::Device m_Device;
    };
}