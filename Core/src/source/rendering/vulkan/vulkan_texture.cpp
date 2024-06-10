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
    

    TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(stagingbuffer, textureImage, static_cast<uint32_t>(_imageSize.x), static_cast<uint32_t>(_imageSize.y));
    TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vmaDestroyBuffer(VulkanInterface::GetAllocator(), stagingbuffer, stagingAllocation);

    CreateImageView(textureImage,VK_FORMAT_R8G8B8A8_SRGB, &textureImageView, VK_IMAGE_ASPECT_COLOR_BIT);

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
    
    CreateImageView(textureImage,VK_FORMAT_R8G8B8A8_SRGB, &textureImageView,VK_IMAGE_ASPECT_COLOR_BIT);

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

void VulkanTexture::Init(VkImageCreateInfo _imageInfo , const VkImageAspectFlags aspectFlags)
{
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;
    
    const VkResult result = vmaCreateImage(VulkanInterface::GetAllocator(), &_imageInfo, &allocCreateInfo, &textureImage, &allocation, nullptr);
    VK_CHECK_ERROR(result,"vmaCreateImage failed on create image")
    CreateImageView(textureImage,_imageInfo.format, &textureImageView, aspectFlags);

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
