﻿#include "rendering/vulkan/vulkan_wrapper.hpp"

#include <stacktrace>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void PC_CORE::CreateBufferVma(VkBuffer* _vkBuffer, VmaAllocation* _allocation, VkBufferUsageFlags _vkBufferUsageFlags,
    VmaMemoryUsage _vmaMemoryUsage, void const* _data, size_t _size)
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
    
    void* data;
    vmaMapMemory(VulkanInterface::GetAllocator(), *_allocation, &data);

    memcpy(data, _data, _size);

    vmaUnmapMemory(VulkanInterface::GetAllocator(), *_allocation);
}

void PC_CORE::TransitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout,
    VkImageLayout _newLayout)
{
    VulkanInterface::vulkanCommandPoolGraphic.BeginSingleCommand();

    VkCommandBuffer commandBuffer;
    VulkanInterface::vulkanCommandPoolGraphic.GetSingleCommandBuffer(&commandBuffer);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = _oldLayout;
    barrier.newLayout = _newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
    commandBuffer,
            sourceStage, destinationStage,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
    );

    VulkanInterface::vulkanCommandPoolGraphic.SubmitSingleCommandBuffer(VulkanInterface::GetDevice().transferQueue.Queue);

}

void PC_CORE::CreateImageView(VkImage _image, VkFormat _format, VkImageView* _vkImageView)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = _format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    VkResult result = vkCreateImageView(VulkanInterface::GetDevice().device, &viewInfo, nullptr, &imageView);

    VK_CHECK_ERROR(result,"failed to create image view!");
    
    *_vkImageView = imageView;
}