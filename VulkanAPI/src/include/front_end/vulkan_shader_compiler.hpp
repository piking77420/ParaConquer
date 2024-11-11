#pragma once

#include "vulkan_header.h"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include <spirv_reflect.h>
#include <filesystem>

namespace Vulkan
{
    class VulkanShaderCompiler
    {
    public:
        VULKAN_API VulkanShaderCompiler();

        VULKAN_API ~VulkanShaderCompiler();

        VULKAN_API void CreateModuleFromSource(vk::Device _device, const char* _source, const char* _path,
            PC_CORE::ShaderStageType _lowLevelShaderStage, SpvReflectShaderModule* _ReflectedModule,
            vk::ShaderModule* _shaderModule);

    private:

        VULKAN_API std::string IncludePath(const std::string& source, const std::filesystem::path& path);

        VULKAN_API std::string GetFileAsString(const std::filesystem::path& path);
    };
}
