#pragma once
#include "vulkan_frame_buffer.hpp"
#include "handles/vulkan_buffer_handle.hpp"
#include "handles/vulkan_image_handle.hpp"
#include "low_renderer/gpu_allocator.hpp"

namespace Vulkan
{
    class VulkanGpuAllocator : public PC_CORE::GpuAllocator
    {
    public:
        bool CreateGPUBuffer(const PC_CORE::GPUBufferCreateInfo& _createInfo, std::shared_ptr<PC_CORE::GpuHandle>* _bufferptr) override;
        
        bool DestroyBuffer(std::shared_ptr<PC_CORE::GpuHandle>* _bufferptr) override;

        bool MapBuffer(const std::shared_ptr<PC_CORE::GpuHandle>& _bufferptr, void** _mapPtr) override;

        bool UnMapBuffer(const std::shared_ptr<PC_CORE::GpuHandle>& _bufferptr) override;

        bool CreateTexture(const PC_CORE::CreateTextureInfo& _createTextureInfo,
            std::shared_ptr<PC_CORE::GpuHandle>* _texturePtr) override;
        
        bool DestroyTexture(std::shared_ptr<PC_CORE::GpuHandle>* _textureHandle) override;
        
        VulkanGpuAllocator(const VmaAllocatorCreateInfo& _createInfo);
        
        VulkanGpuAllocator() = default;
        
        ~VulkanGpuAllocator();

    private:
        static VmaMemoryUsage GetVmaMemoryUsage(PC_CORE::BufferUsage bufferUsage);

        static vk::BufferUsageFlags GetVulkanBufferUsageFlagsClient(PC_CORE::BufferUsage bufferUsage);
        
        void CopyBuffer(vk::CommandBuffer* _commandBuffer, vk::Buffer _src, vk::Buffer _dst, vk::DeviceSize _size);

        VulkanBufferHandle CreateBuffer(size_t size, vk::BufferUsageFlags _bufferUsageFlagBits, VmaMemoryUsage _memoryUsage);

        VulkanImageHandle CreateImage(uint32_t width, uint32_t height, uint32_t depth,
                                      uint32_t _mimpLevel,
                                      vk::ImageType _imageType,
                                      vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                                      VmaMemoryUsage imageMemory);
        
        VmaAllocator m_allocator;
    };

}

