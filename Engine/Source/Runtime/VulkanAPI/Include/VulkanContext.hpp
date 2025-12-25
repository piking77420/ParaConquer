#pragma once

#include <vma/vk_mem_alloc.h>

#include "VulkanDescritptorManager.hpp"
#include "LowRenderer/RhiContext.hpp"
#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"

namespace Vulkan
{


    struct SyncObject
    {
        vk::Semaphore imageAvailableSemaphore;
        vk::Semaphore renderFinishedSemaphore;

        //vk::Semaphore computeFinishedSemaphore;
        //vk::m_ResourceUpdateFence computeInFlightFence;

        vk::Fence inFlightFence;
    };

    // to do make it batchable
    struct FlushCommand
    {
        vk::CommandBuffer cmd;
        vk::Semaphore semaphore;
        PC_CORE::GpuPipelineStage waitStages;
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

        VulkanDescritptorManager descritptorManager;

        std::vector<FlushCommand> flushedCommands;

        VULKAN_API explicit VulkanContext(PC_CORE::Rhi& _Rhi, const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo);

        VULKAN_API ~VulkanContext() override;

        VULKAN_API void WaitIdle() override;

        VULKAN_API std::shared_ptr<VulkanInstance> GetInstance();
        
        VULKAN_API std::shared_ptr<VulkanDevice> GetDevice();

        VULKAN_API std::shared_ptr<VulkanPhysicalDevices> GetPhysicalDevices();

    private:
        VULKAN_API void CreateMemoryAllocator();

        VULKAN_API void CreateCommandPools();


        VULKAN_API void CreateSyncObjects();

        VULKAN_API void DestroySyncObjects();
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
