
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "vulkan_context.hpp"
#include "vulkan_swap_chain.hpp"

using namespace Vulkan;

VulkanContext::VulkanContext(const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo) : RhiContext(rhiContextCreateInfo)
{
    PERF_REGION_SCOPED;
    
    std::vector<std::string> extensionToEnable;
    
    
    renderInstance = std::make_shared<VulkanInstance>(*rhiContextCreateInfo.instanceCreate, rhiContextCreateInfo.WindowHandle);
    
    physicalDevices = std::make_shared<VulkanPhysicalDevices>(*rhiContextCreateInfo.physicalDevicesCreateInfo, &extensionToEnable);
    rhiDevice = std::make_shared<Vulkan::VulkanDevice>(std::reinterpret_pointer_cast<VulkanPhysicalDevices>(physicalDevices), extensionToEnable,  &mainQueue, &computeQueu);
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>(rhiDevice);

    GLFWwindow* window = const_cast<GLFWwindow*>(static_cast<const GLFWwindow*>(rhiContextCreateInfo.WindowHandle));

    int32_t widht;
    int32_t height;
    glfwGetFramebufferSize(window, &widht, &height);

    const uint32_t uwidht = static_cast<uint32_t>(widht);
    const uint32_t uheight = static_cast<uint32_t>(height);
    swapChain = std::make_shared<Vulkan::VulkanSwapChain>(uwidht, uheight);

    CreateMemoryAllocator();
    CreateCommandPools();
    vk::FenceCreateInfo vkFenceCreateInfo;
        
        vkFenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo,
        vkFenceCreateInfo.pNext = nullptr,
        vkFenceCreateInfo.flags = {};
        
    transferFence = device->GetDevice().createFence(vkFenceCreateInfo);
    
    CreateSyncObjects();
}

VulkanContext::~VulkanContext()
{
    PERF_REGION_SCOPED;
    auto device = GetDevice();

    descritptorManager.ClearCaches();
    
    device->GetDevice().destroyFence(transferFence);
    transferFence = nullptr;
    DestroySyncObjects();
    
    device->GetDevice().destroyCommandPool(commandPool);    
    commandPool = nullptr;
    
    device->GetDevice().destroyCommandPool(transferCommandPool);    
    transferCommandPool = nullptr;

    vmaDestroyAllocator(allocator);
    allocator = nullptr;
}


std::shared_ptr<VulkanDevice> VulkanContext::GetDevice()
{
    return std::reinterpret_pointer_cast<VulkanDevice>(GetContext().rhiDevice);
}

std::shared_ptr<VulkanPhysicalDevices> VulkanContext::GetPhysicalDevices()
{
    return std::reinterpret_pointer_cast<VulkanPhysicalDevices>(GetContext().physicalDevices);
}

void VulkanContext::WaitIdleInstance()
{
    GetDevice()->GetDevice().waitIdle();
}


void VulkanContext::CreateMemoryAllocator()
{
    PERF_REGION_SCOPED;
    vk::Instance instance = std::reinterpret_pointer_cast<VulkanInstance>(renderInstance)->GetVulkanInstance();
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(rhiDevice)->GetDevice();
    vk::PhysicalDevice phydevice = std::reinterpret_pointer_cast<VulkanPhysicalDevices>(physicalDevices)->GetVulkanDevice();

    
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
  
    vmaCreateAllocator(&createInfo ,&allocator);

}

void VulkanContext::CreateCommandPools()
{
    PERF_REGION_SCOPED;
    const std::vector<QueueFamilyIndices>& queueFamilyIndices = GetPhysicalDevices()->GetQueuesFamilies();
    auto device = GetDevice();
    
    vk::CommandPoolCreateInfo commandPoolCreateInfo {};
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPoolCreateInfo.queueFamilyIndex = 0;
    
    commandPool = device->GetDevice().createCommandPool(commandPoolCreateInfo);
    
    commandPoolCreateInfo.flags = {};
    transferCommandPool = device->GetDevice().createCommandPool(commandPoolCreateInfo);

}


void VulkanContext::CreateSyncObjects()
{
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
        syncObjects[i].computeFinishedSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);
        syncObjects[i].inFlightFence = vulkanDevice->GetDevice().createFence(fenceInfo);
        syncObjects[i].computeInFlightFence = vulkanDevice->GetDevice().createFence(fenceInfo);

    }
}

void VulkanContext::DestroySyncObjects()
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(rhiDevice);
    for (size_t i = 0; i < syncObjects.size(); i++)
    {
        vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].renderFinishedSemaphore);
        vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].imageAvailableSemaphore);
        vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].computeFinishedSemaphore);
        vulkanDevice->GetDevice().destroyFence(syncObjects[i].inFlightFence);
        vulkanDevice->GetDevice().destroyFence(syncObjects[i].computeInFlightFence);

    }
}
