#pragma once

#include "vulkan_header.h"
#include "render_harware_interface/rhi_typedef.h"
#include <spirv_reflect.h>
#include <filesystem>

namespace VK_NP
{
    class VulkanShaderCompiler
    {
    public:
        VulkanShaderCompiler();

        ~VulkanShaderCompiler();

        void CreateModuleFromSource(vk::Device _device, const char* _source, const char* _path,
            PC_CORE::ShaderStageType _lowLevelShaderStage, SpvReflectShaderModule* _ReflectedModule,
            vk::ShaderModule* _shaderModule);

    private:

        std::string IncludePath(const std::string& source, const std::filesystem::path& path);

        std::string GetFileAsString(const std::filesystem::path& path);
    };
}
