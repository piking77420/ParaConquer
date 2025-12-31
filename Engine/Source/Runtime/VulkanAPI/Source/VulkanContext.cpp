#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <Vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "LowRenderer/Rhi.hpp"
#include "VulkanContext.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanCommandList.hpp"
#include "Utils/RhiToVulkan.hpp"

using namespace Vulkan;

VulkanContext::VulkanContext(PC_CORE::Rhi& _Rhi)
    : RhiContext(_Rhi)
    , descritptorManager(*this)
{
   

}

void Vulkan::VulkanContext::Init(const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    std::set<std::string> extensionToEnable;


    std::shared_ptr<VulkanInstance> vkInstance = std::make_shared<VulkanInstance>(*rhiContextCreateInfo.instanceCreate,
        rhiContextCreateInfo.WindowHandle);
    std::shared_ptr<VulkanPhysicalDevices> vkPhysicalDevice = std::make_shared<VulkanPhysicalDevices>(vkInstance->GetVulkanInstance(), vkInstance->surface, *rhiContextCreateInfo.physicalDevicesCreateInfo,
        &extensionToEnable);

    std::shared_ptr<VulkanDevice> vkDevice = std::make_shared<VulkanDevice>(vkPhysicalDevice,
        extensionToEnable, &mainQueue);

    renderInstance = vkInstance;
    rhiPhysicalDevices = vkPhysicalDevice;
    rhiDevice = vkDevice;

    auto window = const_cast<GLFWwindow*>(static_cast<const GLFWwindow*>(rhiContextCreateInfo.WindowHandle));
    int32_t widht;
    int32_t height;
    glfwGetFramebufferSize(window, &widht, &height);

    const uint32_t uwidht = static_cast<uint32_t>(widht);
    const uint32_t uheight = static_cast<uint32_t>(height);
    rhiSwapChain = std::make_shared<VulkanSwapChain>(m_Rhi, uwidht, uheight, *vkPhysicalDevice, *vkDevice, vkInstance->surface);
    rhiSwapChain->SetName("MainSwapChain");

    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceInfo.flags = {};
    transferFence = GetDevice()->GetDevice().createFence(fenceInfo);

    CreateMemoryAllocator();
    CreateCommandPools();
    CreateSyncObjects();

    m_TransferCommandList.reset(new VulkanCommandList(m_Rhi));
    m_TransferCommandList
        ->SetName("TransferCommandList")
        .Build();
}


VulkanContext::~VulkanContext()
{
    PERF_REGION_SCOPED;
    auto device = GetDevice();

    DestroySyncObjects();
    descritptorManager.ClearCaches();

    device->GetDevice().destroyFence(transferFence);
    transferFence = VK_NULL_HANDLE;

    device->GetDevice().destroyCommandPool(commandPool);
    commandPool = nullptr;

    device->GetDevice().destroyCommandPool(transferCommandPool);
    transferCommandPool = nullptr;

    vmaDestroyAllocator(allocator);
    allocator = nullptr;
}



std::shared_ptr<VulkanInstance> VulkanContext::GetInstance()
{
    return std::reinterpret_pointer_cast<VulkanInstance>(renderInstance);
}

std::shared_ptr<VulkanDevice> VulkanContext::GetDevice()
{
    return std::reinterpret_pointer_cast<VulkanDevice>(rhiDevice);
}

std::shared_ptr<VulkanPhysicalDevices> VulkanContext::GetPhysicalDevices()
{
    return std::reinterpret_pointer_cast<VulkanPhysicalDevices>(rhiPhysicalDevices);
}


void VulkanContext::WaitIdle()  
{
    GetDevice()->GetDevice().waitIdle();
}


void VulkanContext::CreateMemoryAllocator()
{
    PERF_REGION_SCOPED;
    vk::Instance instance = std::reinterpret_pointer_cast<VulkanInstance>(renderInstance)->GetVulkanInstance();
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(rhiDevice)->GetDevice();
    vk::PhysicalDevice phydevice = std::reinterpret_pointer_cast<VulkanPhysicalDevices>(rhiPhysicalDevices)->
        GetVulkanDevice();


    const VmaAllocatorCreateInfo createInfo =
    {
        .flags = {},
        .physicalDevice = phydevice,
        .device = device,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = nullptr,
        .instance = instance,
        .vulkanApiVersion = VK_API_VERSION_1_3,
        .pTypeExternalMemoryHandleTypes = nullptr
    };

    vmaCreateAllocator(&createInfo, &allocator);
}

void VulkanContext::CreateCommandPools()
{
    PERF_REGION_SCOPED;
    const std::vector<QueueFamilyIndices>& queueFamilyIndices = GetPhysicalDevices()->GetQueuesFamilies();
    auto device = GetDevice();

    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPoolCreateInfo.queueFamilyIndex = 0;

    commandPool = device->GetDevice().createCommandPool(commandPoolCreateInfo);

    commandPoolCreateInfo.flags = {};
    transferCommandPool = device->GetDevice().createCommandPool(commandPoolCreateInfo);
}


void VulkanContext::CreateSyncObjects()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(rhiDevice);

    vk::SemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    for (size_t i = 0; i < syncObjects.size(); i++)
    {
        syncObjects[i].imageAvailableSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);
        syncObjects[i].renderFinishedSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);
        syncObjects[i].transferFinishSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);

        //syncObjects[i].computeInFlightFence = vulkanDevice->GetDevice().createFence(fenceInfo);
        //syncObjects[i].computeFinishedSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);
        syncObjects[i].inFlightFence = vulkanDevice->GetDevice().createFence(fenceInfo);
    }
}

void VulkanContext::DestroySyncObjects()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(rhiDevice);
    for (size_t i = 0; i < syncObjects.size(); i++)
    {
        vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].renderFinishedSemaphore);
        vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].imageAvailableSemaphore);
        vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].transferFinishSemaphore);
        vulkanDevice->GetDevice().destroyFence(syncObjects[i].inFlightFence);
        //vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].computeFinishedSemaphore);
        //vulkanDevice->GetDevice().destroyFence(syncObjects[i].computeInFlightFence);
    }
}



void Vulkan::VulkanContext::SendEnqueuCommand(PC_CORE::CommandList* _EnqueuCommands, PC_CORE::GpuPipelineStage waitStage)
{
    PERF_REGION_SCOPED  
    PERF_REGION_COLOR(PerfRegion::Rhi)

    VulkanCommandList* vkCmdL = reinterpret_cast<VulkanCommandList*>(_EnqueuCommands);

    flushedCommands.Commands.emplace_back(vkCmdL->GetVkHandle());
    flushedCommands.Semaphores.emplace_back(vkCmdL->GetVkSemaphore());
    flushedCommands.BatchPipelineStageFlag.emplace_back(Utils::RhiPipelineStageToVulkan(waitStage));
}

void Vulkan::VulkanContext::ProceedResourceUpdateBranch()
{
    const size_t CurrentFrameIndex = m_Rhi.GetFrameIndex();
    
    bool NeedToSendToGpu = false;

    if (!m_ResourceUpdate.empty())
    {
        m_TransferCommandList->BeginRecordCommands();
        m_TransferCommandList->BeginDebugLabel("Resource Update", { 0.75f,0.5f, 0, 1.f });
        for (auto it = m_ResourceUpdate.begin(); it != m_ResourceUpdate.end(); )
        {
            PC_CORE::RHI::ResourceUpdate::ResourceUpdateStatus Status = it->Execute(*m_TransferCommandList);
            switch (Status)
            {
            case PC_CORE::RHI::ResourceUpdate::Success:
                NeedToSendToGpu = true;
                it++;
                break;
            case PC_CORE::RHI::ResourceUpdate::Failed:
                it = m_ResourceUpdate.erase(it);
                break;
            case PC_CORE::RHI::ResourceUpdate::Complete:
                it = m_ResourceUpdate.erase(it);
                NeedToSendToGpu = true;
                break;
            default:
                it++;
                break;
            }
        }
        m_TransferCommandList->EndDebugLabel();
        m_TransferCommandList->EndRecordCommands();
    }
    

    if (NeedToSendToGpu)
    {
        vk::Semaphore signalSemaphores[] = {
        syncObjects[CurrentFrameIndex].transferFinishSemaphore
        };

        vk::CommandBuffer cmd = m_TransferCommandList->GetVkHandle(CurrentFrameIndex);

        vk::SubmitInfo submitInfo{};
        submitInfo
            .setWaitSemaphoreCount(0)
            .setPWaitSemaphores(nullptr)
            .setPWaitDstStageMask(nullptr)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(signalSemaphores)
            .setCommandBufferCount(1)
            .setPCommandBuffers(&cmd);

        VK_CALL(mainQueue.submit(1u, &submitInfo, nullptr));
    }   
}
