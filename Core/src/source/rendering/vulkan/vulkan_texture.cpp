#include "rendering/vulkan/vulkan_texture.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

void VulkanTexture::Init(void const* const _data, size_t _dataSize , Vector2i _imageSize)
{
    VkBuffer stagingbuffer;
    VmaAllocation stagingAllocation;

    CreateBufferVma(&stagingbuffer, &stagingAllocation,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VMA_MEMORY_USAGE_CPU_ONLY, _dataSize);
    void* data;
    vmaMapMemory(VulkanInterface::GetAllocator(), stagingAllocation, &data);
    memcpy(data, _data, _dataSize);
    vmaUnmapMemory(VulkanInterface::GetAllocator(), stagingAllocation);
    
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = _imageSize.x;
    imageInfo.extent.height = _imageSize.y;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;

    
    const VkResult result = vmaCreateImage(VulkanInterface::GetAllocator(), &imageInfo, &allocCreateInfo, &textureImage, &allocation, nullptr);
    VK_CHECK_ERROR(result,"vmaCreateImage failed on create image")
    

    const uint32_t layer = 1;
    TransitionImageLayout(textureImage,VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,layer);
    CopyBufferToImage(stagingbuffer, textureImage, static_cast<uint32_t>(_imageSize.x), static_cast<uint32_t>(_imageSize.y), layer);
    TransitionImageLayout(textureImage,VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,layer);

    vmaDestroyBuffer(VulkanInterface::GetAllocator(), stagingbuffer, stagingAllocation);

    CreateImageView(VK_IMAGE_VIEW_TYPE_2D, textureImage,VK_FORMAT_R8G8B8A8_SRGB, &textureImageView, VK_IMAGE_ASPECT_COLOR_BIT, imageInfo.arrayLayers);

    const VkPhysicalDeviceProperties& properties = VulkanInterface::GetPhysicalDevice().devProps;    
    const  VkSamplerCreateInfo samplerInfo =
       {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE 
        };

   VulkanInterface::vulkanTextureSampler.CreateSampler(samplerInfo, &samplerId);
    
}

void VulkanTexture::Init(size_t _dataSize, Vector2i _imageSize)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = _imageSize.x;
    imageInfo.extent.height = _imageSize.y;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;
    
    const VkResult result = vmaCreateImage(VulkanInterface::GetAllocator(), &imageInfo, &allocCreateInfo, &textureImage, &allocation, nullptr);
    VK_CHECK_ERROR(result,"vmaCreateImage failed on create image")

    CreateImageView(VK_IMAGE_VIEW_TYPE_2D, textureImage,VK_FORMAT_R8G8B8A8_SRGB, &textureImageView,VK_IMAGE_ASPECT_COLOR_BIT, imageInfo.arrayLayers);

    const VkPhysicalDeviceProperties& properties = VulkanInterface::GetPhysicalDevice().devProps;    
    const  VkSamplerCreateInfo samplerInfo =
       {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE 
        };

   VulkanInterface::vulkanTextureSampler.CreateSampler(samplerInfo, &samplerId);
}

void VulkanTexture::Init(VkImageCreateInfo _imageInfo , const VkImageAspectFlags aspectFlags,VkImageLayout  _imageLayout)
{
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;
    
    const VkResult result = vmaCreateImage(VulkanInterface::GetAllocator(), &_imageInfo, &allocCreateInfo, &textureImage, &allocation, nullptr);
    VK_CHECK_ERROR(result,"vmaCreateImage failed on create image")

    CreateImageView(VK_IMAGE_VIEW_TYPE_2D, textureImage,_imageInfo.format, &textureImageView, aspectFlags, _imageInfo.arrayLayers);
    
    if (_imageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
        TransitionImageLayout(textureImage, aspectFlags, _imageInfo.format, _imageInfo.initialLayout, _imageLayout, 1);

    const VkPhysicalDeviceProperties& properties = VulkanInterface::GetPhysicalDevice().devProps;    
    const  VkSamplerCreateInfo samplerInfo =
       {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE 
        };

   VulkanInterface::vulkanTextureSampler.CreateSampler(samplerInfo, &samplerId);
}

void VulkanTexture::Init(std::array<void*,6> _cubeMapsData, size_t width , size_t height)
{
    const VkDeviceSize imageSize = width * height * 4 * 6; //4 since I always load my textures with an alpha channel, and multiply it by 6 because the image must have 6 layers.
    const VkDeviceSize layerSize = imageSize / 6; //This is just the size of each layer.
    
    VkBuffer stagingbuffer = VK_NULL_HANDLE;
    VmaAllocation stagingAllocation = VK_NULL_HANDLE;

    CreateBufferVma(&stagingbuffer, &stagingAllocation,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VMA_MEMORY_USAGE_CPU_ONLY, imageSize);

    void* dataMap;
    vmaMapMemory(VulkanInterface::GetAllocator(), stagingAllocation, &dataMap);
    for (size_t i = 0; i < _cubeMapsData.size(); i++)
    {
        memcpy(reinterpret_cast<uint8_t*>(dataMap) + (layerSize * i), _cubeMapsData[i], static_cast<size_t>(layerSize));
    }
    vmaUnmapMemory(VulkanInterface::GetAllocator(), stagingAllocation);
    
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = _cubeMapsData.size();
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT; // Enable cube map compatibility

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;

    
    const VkResult result = vmaCreateImage(VulkanInterface::GetAllocator(), &imageInfo, &allocCreateInfo, &textureImage, &allocation, nullptr);
    VK_CHECK_ERROR(result,"vmaCreateImage failed on create image")

    std::vector<VkBufferImageCopy> copyRegions;
    copyRegions.resize(_cubeMapsData.size());

    for (size_t i = 0; i < copyRegions.size(); i++)
    {
        VkBufferImageCopy vkBufferImageCopy;
            vkBufferImageCopy.bufferOffset = layerSize * i;  
            vkBufferImageCopy.bufferRowLength = 0;  
            vkBufferImageCopy.bufferImageHeight = 0;  
            vkBufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
            vkBufferImageCopy.imageSubresource.mipLevel = 0;  
            vkBufferImageCopy.imageSubresource.baseArrayLayer = static_cast<uint32_t>(i);
            vkBufferImageCopy.imageSubresource.layerCount = 1; 
            vkBufferImageCopy.imageOffset = {0, 0, 0};
            vkBufferImageCopy.imageExtent = {
                static_cast<uint32_t>(width),   
                static_cast<uint32_t>(height),  
                1                   
            };
        copyRegions[i] = vkBufferImageCopy;
    }
    

    TransitionImageLayout(textureImage,VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6);
    CopyBufferToImage(stagingbuffer, textureImage, copyRegions);
    TransitionImageLayout(textureImage,VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 6);
    
    CreateImageView(VK_IMAGE_VIEW_TYPE_CUBE, textureImage,VK_FORMAT_R8G8B8A8_SRGB, &textureImageView, VK_IMAGE_ASPECT_COLOR_BIT, imageInfo.arrayLayers);

    const VkPhysicalDeviceProperties& properties = VulkanInterface::GetPhysicalDevice().devProps;    
    const  VkSamplerCreateInfo samplerInfo =
       {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE 
        };

    VulkanInterface::vulkanTextureSampler.CreateSampler(samplerInfo, &samplerId);
    
    vmaDestroyBuffer(VulkanInterface::GetAllocator(), stagingbuffer, stagingAllocation);
}

void VulkanTexture::Destroy()
{
    if (textureImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(VulkanInterface::GetDevice().device, textureImageView, nullptr);
        textureImageView = VK_NULL_HANDLE;
    }

    bool hasDestroyVma = false;

    if (allocation != VK_NULL_HANDLE) 
    {
        vmaDestroyImage(VulkanInterface::GetAllocator(), textureImage, allocation);
        textureImage = VK_NULL_HANDLE;
        hasDestroyVma = true;
    }
    if (textureImage != VK_NULL_HANDLE && !hasDestroyVma)
        vkDestroyImage(VulkanInterface::GetDevice().device, textureImage, nullptr);

}
