#pragma once

#include "vulkan_header.h"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include <spirv_reflect.h>
#include <filesystem>

namespace Vulkan
{

    VULKAN_API void InitCompilerProcess();

    VULKAN_API void DestroyCompilerProcess();

    VULKAN_API void CreateModuleFromSource(vk::Device _device, const char* _source, const char* _path,
        PC_CORE::ShaderStageType _lowLevelShaderStage, SpvReflectShaderModule* _ReflectedModule,
        vk::ShaderModule* _shaderModule);

}
