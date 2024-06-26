﻿#include "rendering/vulkan/vulkan_wrapper.hpp"

#include <stacktrace>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void PC_CORE::CreateBufferVma(VkBuffer* _vkBuffer, VmaAllocation* _allocation, VkBufferUsageFlags _vkBufferUsageFlags,
    VmaMemoryUsage _vmaMemoryUsage, size_t _size)
{
    //allocate vertex buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //this is the total size, in bytes, of the buffer we are allocating
    bufferInfo.size = _size;
    //this buffer is going to be used as a Vertex Buffer
    bufferInfo.usage = _vkBufferUsageFlags;
	
    //let the VMA library know that this data should be writeable by CPU, but also readable by GPU
    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = _vmaMemoryUsage;
    
    //allocate the buffer
    VkResult result = vmaCreateBuffer(VulkanInterface::GetAllocator(), &bufferInfo, &vmaallocInfo, _vkBuffer, _allocation, nullptr);
    VK_CHECK_ERROR(result,"Failded to createBuffer");
}

void PC_CORE::TransitionImageLayout(VkImage _image, const VkImageAspectFlags _aspectFlags , VkFormat _format, VkImageLayout _oldLayout,
    VkImageLayout _newLayout , uint32_t _layerCount)
{

    VulkanCommandPool* commandPool = &VulkanInterface::vulkanCommandPoolGraphic; 
    VkCommandBuffer commandBuffer;
    const VulkanDevice& device = VulkanInterface::GetDevice();
    const VkQueue* queue = &device.graphicsQueue.Queue;

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = _oldLayout;
    barrier.newLayout = _newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _image;
    barrier.subresourceRange.aspectMask = _aspectFlags;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = _layerCount;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

        commandPool = &VulkanInterface::vulkanCommandPoolGraphic;
        queue = &device.graphicsQueue.Queue;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        commandPool = &VulkanInterface::vulkanCommandPoolGraphic;
        queue = &device.graphicsQueue.Queue;

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT; 
    }
    else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        commandPool = &VulkanInterface::vulkanCommandPoolGraphic;
        queue = &device.graphicsQueue.Queue;

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        commandPool = &VulkanInterface::vulkanCommandPoolGraphic;
        queue = &device.graphicsQueue.Queue;

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        commandPool = &VulkanInterface::vulkanCommandPoolGraphic;
        queue = &device.graphicsQueue.Queue;

        barrier.srcAccessMask = 0;  // No access flags required, since we're transitioning from UNDEFINED
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        commandPool = &VulkanInterface::vulkanCommandPoolGraphic;
        queue = &device.graphicsQueue.Queue;

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } 
    else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    commandPool->BeginSingleCommand();
    commandPool->GetSingleCommandBuffer(&commandBuffer);
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    commandPool->SubmitSingleCommandBuffer(*queue);

}

void PC_CORE::CreateImageView(VkImageViewType _vkImageViewType, VkImage _image, VkFormat _format, VkImageView* _vkImageView,VkImageAspectFlags aspectFlags, uint32_t _layerCout)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _image;
    viewInfo.viewType = _vkImageViewType;
    viewInfo.format = _format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1; 
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = _layerCout;

    VkImageView imageView;
    VkResult result = vkCreateImageView(VulkanInterface::GetDevice().device, &viewInfo, nullptr, &imageView);

    VK_CHECK_ERROR(result,"failed to create image view!");
    
    *_vkImageView = imageView;
}

void PC_CORE::CopyBufferToImage(VkBuffer _buffer, VkImage _image, const std::vector<VkBufferImageCopy>& _region)
{
    VulkanInterface::vulkanCommandPoolGraphic.BeginSingleCommand();
    VkCommandBuffer cmdbuffer = {};
    VulkanInterface::vulkanCommandPoolGraphic.GetSingleCommandBuffer(&cmdbuffer);
    
    vkCmdCopyBufferToImage(cmdbuffer, _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(_region.size()), _region.data());

    VulkanInterface::vulkanCommandPoolGraphic.SubmitSingleCommandBuffer(VulkanInterface::GetDevice().graphicsQueue.Queue);
}

void PC_CORE::CopyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height , uint32_t layer)
{
    VulkanInterface::vulkanCommandPoolGraphic.BeginSingleCommand();
    VkCommandBuffer cmdbuffer = {};
    VulkanInterface::vulkanCommandPoolGraphic.GetSingleCommandBuffer(&cmdbuffer);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layer;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        _width,
        _height,
        1
    };

    vkCmdCopyBufferToImage(cmdbuffer, _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VulkanInterface::vulkanCommandPoolGraphic.SubmitSingleCommandBuffer(VulkanInterface::GetDevice().graphicsQueue.Queue);
}
