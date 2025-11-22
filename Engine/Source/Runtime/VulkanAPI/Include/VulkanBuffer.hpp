#pragma once

#include <vma/vk_mem_alloc.h>

#include "VulkanHeader.h"
#include "VulkanContext.hpp"
#include "LowRenderer/RhiBuffer.h"


namespace Vulkan
{
    struct BufferAndAlloc
    {
        vk::Buffer buffer = VK_NULL_HANDLE;
        VmaAllocation alloc = VK_NULL_HANDLE;
        RhiResourceState ResourceState = RhiResourceState::Undefined;

        DEFAULT_CONSTRUCTOR_DESTRUCTOR(BufferAndAlloc)

        DEFAULT_COPY_MOVE_OPERATIONS(BufferAndAlloc)
    };

    class VULKAN_API VulkanBuffer : public PC_CORE::RhiBuffer
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanBuffer)
        
        explicit VulkanBuffer(PC_CORE::Rhi& _Rhi, const std::string& _name, const RhiBufferDescriptor& _rhiBufferDescriptor, MemoryUsage _memoryUsage);
        
        explicit VulkanBuffer(PC_CORE::Rhi& _Rhi, std::string&& _name, const RhiBufferDescriptor& _rhiBufferDescriptor, MemoryUsage _memoryUsage);
        
        ~VulkanBuffer() override;

        const void* GetFrameNativeHandle(size_t _frameIndex) const override;
        
        void* GetFrameNativeHandle(size_t _frameIndex) override;
        
        bool Build() override;
        
        void UploadData(PC_CORE::CommandList* _commandList, const void* _data, size_t _sizeInBytes) override;

        char* BeginFullDynamicBufferUpdateForCurrentFrame() override;

        void EndFullDynamicBufferUpdateForCurrentFrame() override;
    
        static void CreateStagingBufferForCopy(VulkanContext& _VkContext, BufferAndAlloc* bufferAndAlloc, size_t _sizeInBytes);
    
        static void FreeAlloc(VulkanContext& _VkContext, BufferAndAlloc& _handle);
        
        const BufferAndAlloc* GetBufferAndAlloc(size_t _frameIndex) const;

        BufferAndAlloc* GetBufferAndAlloc(size_t _frameIndex);

    private:
        std::vector<BufferAndAlloc> m_Handles;
        
        void* m_CurrentFrameMappedData = nullptr;
        
    };
}
