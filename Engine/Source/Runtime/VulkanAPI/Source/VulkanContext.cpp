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
#include "VulkanTexture.hpp"
#include "VulkanBuffer.hpp"

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

    std::scoped_lock _(VulkanContextMutex());

    std::set<std::string> extensionToEnable;


    std::shared_ptr<VulkanInstance> vkInstance = std::make_shared<VulkanInstance>(*rhiContextCreateInfo.instanceCreate,
        rhiContextCreateInfo.WindowHandle);
    std::shared_ptr<VulkanPhysicalDevices> vkPhysicalDevice = std::make_shared<VulkanPhysicalDevices>(vkInstance->GetVulkanInstance(), vkInstance->surface, *rhiContextCreateInfo.physicalDevicesCreateInfo,
        &extensionToEnable);

    std::shared_ptr<VulkanDevice> vkDevice = std::make_shared<VulkanDevice>(
        rhiContextCreateInfo.physicalDevicesCreateInfo->requestExtensions, 
        vkPhysicalDevice,
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

    descritptorManager.CreateDesciptorPool();
    m_TransferCommandList.reset(new VulkanCommandList(m_Rhi));
    m_TransferCommandList
        ->SetName("TransferCommandList")
        .Build();
}


VulkanContext::~VulkanContext()
{
    PERF_REGION_SCOPED;
    auto device = GetDevice();

    for (uint32_t i = 0; i < static_cast<uint32_t>(m_PendingDefferedDestroy.size()); i++)
    {
        ProceedDefferdDestroy(i);
    }

    DestroySyncObjects();
    //descritptorManager.ClearCaches();

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

    flushedCommands.Commands.emplace_back(vkCmdL->GetVulkanCommandBufferHandle());
    flushedCommands.Semaphores.emplace_back(vkCmdL->GetVkSemaphore());
    flushedCommands.BatchPipelineStageFlag.emplace_back(Utils::RhiPipelineStageToVulkan(waitStage));
}

void Vulkan::VulkanContext::ProceedResourceUpdateBranch()
{
    std::scoped_lock _(m_ResourceUpdateLock);

    const size_t CurrentFrameIndex = m_Rhi.GetFrameIndex();
    
    m_PendingTransferOperation = false;

    if (!m_ResourceUpdate.empty())
    {
        m_TransferCommandList->BeginRecordCommands();
        m_TransferCommandList->BeginDebugLabel("Resource Update", { 0.75f,0.5f, 0, 1.f });
        for (auto it = m_ResourceUpdate.begin(); it != m_ResourceUpdate.end(); )
        {
            m_PendingTransferOperation = (*it)->Proceed(*m_TransferCommandList);
            if ((*it)->IsEmpty())
            {
                it = m_ResourceUpdate.erase(it);
            }
            else
            {
                ++it;
            }
        }
        m_TransferCommandList->EndDebugLabel();
        m_TransferCommandList->EndRecordCommands();
    }
    

    if (m_PendingTransferOperation)
    {
        vk::Semaphore signalSemaphores[] = {
        syncObjects[CurrentFrameIndex].transferFinishSemaphore
        };

        vk::CommandBuffer cmd = m_TransferCommandList->GetVulkanCommandBufferHandle(CurrentFrameIndex);

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

void VulkanContext::ProceedDefferdDestroy(uint32_t _FrameIndex)
{
    for (auto& v : m_PendingDefferedDestroy[_FrameIndex])
    {
        // TODO use visitor pattern
        if (std::holds_alternative<DefferdDestroyBuffer>(v))
        {
            DefferdDestroyBuffer& operation = std::get<DefferdDestroyBuffer>(v);

            if (operation.buffer == VK_NULL_HANDLE || operation.alloc == VK_NULL_HANDLE)
                continue;

            vmaDestroyBuffer(allocator, operation.buffer, operation.alloc);
        }
        else if (std::holds_alternative<DefferdDestroyBufferTexture>(v))
        {
            DefferdDestroyBufferTexture& operation = std::get<DefferdDestroyBufferTexture>(v);

            vk::Device device = GetDevice()->GetDevice();

            if (operation.alloc != VK_NULL_HANDLE)
            {
                if (operation.image != VK_NULL_HANDLE && operation.alloc != VK_NULL_HANDLE)
                {
                    device.destroyImageView(operation.imageView);
                    vmaDestroyImage(allocator, operation.image, operation.alloc);
                }
            }
            else if (operation.image != VK_NULL_HANDLE && operation.imageView != VK_NULL_HANDLE)
            {
                device.destroyImageView(operation.imageView);
                device.destroyImage(operation.image);
            }
        }
        else if (std::holds_alternative<DefferdDestroyBufferTexture>(v))
        {
            vk::ImageView& imageView = std::get<vk::ImageView>(v);
            vk::Device device = GetDevice()->GetDevice();

            if (imageView != VK_NULL_HANDLE)
            {
                device.destroyImageView(imageView);
            }
        }
    }

    m_PendingDefferedDestroy[_FrameIndex].clear();
}

void Vulkan::VulkanContext::DefferdDestroy(BufferAndAlloc& BufferAndAlloc, uint32_t _FrameIndex)
{
    if (BufferAndAlloc.buffer == VK_NULL_HANDLE || BufferAndAlloc.alloc == VK_NULL_HANDLE)
        return;

    m_PendingDefferedDestroy[_FrameIndex].emplace_back().emplace<DefferdDestroyBuffer>(BufferAndAlloc.buffer, BufferAndAlloc.alloc);
    BufferAndAlloc = {};
}

void Vulkan::VulkanContext::DefferdDestroy(TextureAndAlloc& TextureAndAlloc, uint32_t _FrameIndex)
{
    m_PendingDefferedDestroy[_FrameIndex].emplace_back().emplace<DefferdDestroyBufferTexture>(TextureAndAlloc.Image, TextureAndAlloc.ImageView, TextureAndAlloc.Allocation);
    TextureAndAlloc = {};
}

void Vulkan::VulkanContext::DefferdDestroy(vk::ImageView& imageView, uint32_t _FrameIndex)
{
    m_PendingDefferedDestroy[_FrameIndex].emplace_back().emplace<vk::ImageView>(imageView);
}