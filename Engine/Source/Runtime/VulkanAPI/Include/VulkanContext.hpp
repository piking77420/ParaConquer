#pragma once

#include <variant>
#include <vector>
#include <vma/vk_mem_alloc.h>

#include "VulkanDescritptorManager.hpp"
#include "LowRenderer/RhiContext.hpp"
#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"

namespace Vulkan
{
    class VulkanCommandList;
    struct TextureAndAlloc;
    struct BufferAndAlloc;

    struct SyncObject
    {
        vk::Semaphore imageAvailableSemaphore;
        vk::Semaphore renderFinishedSemaphore;
        vk::Semaphore transferFinishSemaphore;

        //vk::Semaphore computeFinishedSemaphore;
        //vk::m_ResourceUpdateFence computeInFlightFence;

        vk::Fence inFlightFence;
    };

    struct FlushCommand
    {
        std::vector<vk::CommandBuffer> Commands;
        std::vector<vk::Semaphore> Semaphores;
        std::vector<vk::PipelineStageFlags> BatchPipelineStageFlag;

        void Clear()
        {
            Commands.clear();
            Semaphores.clear();
            BatchPipelineStageFlag.clear();
        }
    };

    class VulkanContext : public PC_CORE::RhiContext
    {
    public:
        std::array<SyncObject, MaxFramesInFlight> syncObjects;

        vk::Queue mainQueue;

        vk::CommandPool commandPool = VK_NULL_HANDLE;

        vk::CommandPool transferCommandPool = VK_NULL_HANDLE;

        vk::Fence transferFence;

        VmaAllocator allocator = VK_NULL_HANDLE;

        FlushCommand flushedCommands;

        VulkanDescritptorManager descritptorManager;

        std::vector<vk::SubmitInfo> SubmitInfoBuffer;

        VULKAN_API explicit VulkanContext(PC_CORE::Rhi& _Rhi);

        VULKAN_API ~VulkanContext() override;

        VULKAN_API void Init(const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo); // TODO BUILD PATTER

        VULKAN_API void WaitIdle() override;

        VULKAN_API void SendEnqueuCommand(PC_CORE::CommandList* _EnqueuCommands, PC_CORE::GpuPipelineStage waitStage) override;

        VULKAN_API void ProceedResourceUpdateBranch() override;

        VULKAN_API void ProceedDefferdDestroy(uint32_t _FrameIndex) override final;

        VULKAN_API std::shared_ptr<VulkanInstance> GetInstance();

        VULKAN_API std::shared_ptr<VulkanDevice> GetDevice();

        VULKAN_API std::shared_ptr<VulkanPhysicalDevices> GetPhysicalDevices();

        VULKAN_API void DefferdDestroy(BufferAndAlloc& BufferAndAlloc, uint32_t _FrameIndex);

        VULKAN_API void DefferdDestroy(TextureAndAlloc& TextureAndAlloc, uint32_t _FrameIndex);

    private:
        struct DefferdDestroyBuffer
        {
            vk::Buffer buffer = VK_NULL_HANDLE;
            VmaAllocation alloc = VK_NULL_HANDLE;
        };

        struct DefferdDestroyBufferTexture
        {
            vk::Image image = VK_NULL_HANDLE;
            vk::ImageView imageView = VK_NULL_HANDLE;
            VmaAllocation alloc = VK_NULL_HANDLE;
        };

        using DefferedDestroyOperation = std::variant<DefferdDestroyBuffer, DefferdDestroyBufferTexture>;

        std::array<std::vector<DefferedDestroyOperation>, MaxFramesInFlight> m_PendingDefferedDestroy;

        VULKAN_API void CreateMemoryAllocator();

        VULKAN_API void CreateCommandPools();

        VULKAN_API void CreateSyncObjects();

        VULKAN_API void DestroySyncObjects();

        std::unique_ptr<VulkanCommandList> m_TransferCommandList;
    };

#define GET_VK_CONTEXT \
    reinterpret_cast<VulkanContext&>(m_Rhi.GetRhiContext())

#define GET_VK_INSTANCE \
    std::reinterpret_pointer_cast<VulkanInstance>(m_Rhi.GetRhiContext().renderInstance)
#define GET_VK_DEVICE \
    std::reinterpret_pointer_cast<VulkanDevice>(m_Rhi.GetRhiContext().rhiDevice)->GetDevice()
#ifdef  DEBUG_GPU_ON
#define SET_VK_DEBUG_NAME(debugInfo)\
    GET_VK_INSTANCE->SetDebugName(GET_VK_DEVICE, &debugInfo)
#else
#define SET_VK_DEBUG_NAME(x)
#endif
}
