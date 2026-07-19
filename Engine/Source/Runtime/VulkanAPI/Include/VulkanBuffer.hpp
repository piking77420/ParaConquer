#pragma once

#include <vk_mem_alloc.h>

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
        
        explicit VulkanBuffer(PC_CORE::Rhi& _Rhi);
        
        ~VulkanBuffer() override;
        
        bool Build() override;
        
        bool UploadData(PC_CORE::CommandList* _commandList, const void* _data, size_t _sizeInBytes) override;

        char* BeginFullDynamicBufferUpdateForCurrentFrame() override;

        void EndBufferUpdate() override;

        char* BeginBufferUpdateForCurrentFrame() override;
    
        static void CreateStagingBufferForCopy(VulkanContext& _VkContext, BufferAndAlloc* bufferAndAlloc, size_t _sizeInBytes, const char* BufferName);
            
        const BufferAndAlloc* GetBufferAndAlloc(size_t _frameIndex) const;

        BufferAndAlloc* GetBufferAndAlloc(size_t _frameIndex);

        const BufferAndAlloc* GetVkAlloc(size_t _frameIndex) const;

        BufferAndAlloc* GetVkAlloc(size_t _frameIndex);

    private:

        const BufferAndAlloc* GetVkStagingBuffer(size_t _frameIndex) const;

        BufferAndAlloc* GetVkStagingBuffer(size_t _frameIndex);

        std::vector<BufferAndAlloc> m_Handle;
        
        std::vector<void*> m_CurrentFrameMappedData;

        std::vector<BufferAndAlloc> m_StagingBuffers;

        std::vector<size_t> m_StaginBuffersSizes;
        
    };
}
