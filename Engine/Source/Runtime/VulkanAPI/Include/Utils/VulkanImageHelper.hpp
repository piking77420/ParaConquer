#pragma once

#include <vma/vk_mem_alloc.h>

#include "VulkanHeader.h"
#include "LowRenderer/RhiBuffer.h"
#include "LowRenderer/RhiTexture.hpp"

namespace Vulkan::Utils
{

    VULKAN_API void GenerateMipMapFunc(vk::CommandBuffer _commandBuffer, 
                                       vk::Image _image,
                                       vk::Filter _Filter,
                                       vk::ImageLayout newImageLayout,
                                       int32_t _imageWidth,     
                                       int32_t _imageHeight, 
                                       vk::Format _format, 
                                       uint32_t _mipLevel,
                                       vk::ImageAspectFlags _aspectFlag);

}
