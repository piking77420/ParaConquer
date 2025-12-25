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

using namespace Vulkan;

VulkanContext::VulkanContext(PC_CORE::Rhi& _Rhi, const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo)
    : RhiContext(_Rhi, rhiContextCreateInfo)
    , descritptorManager(*this)
{
    PERF_REGION_SCOPED;

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
    rhiSwapChain = std::make_shared<VulkanSwapChain>(m_Rhi, uwidht, uheight, *vkPhysicalDevice, *vkDevice,  vkInstance->surface);
    rhiSwapChain->SetName("MainSwapChain");

    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    transferFence = GetDevice()->GetDevice().createFence(fenceInfo);

    CreateMemoryAllocator();
    CreateCommandPools();
    CreateSyncObjects();
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

void VulkanContext::WaitIdleInstance()
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
        //syncObjects[i].renderFinishedSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);
        //syncObjects[i].computeInFlightFence = vulkanDevice->GetDevice().createFence(fenceInfo);
        //syncObjects[i].computeFinishedSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);
        syncObjects[i].inFlightFence = vulkanDevice->GetDevice().createFence(fenceInfo);
    }
}

void VulkanContext::DestroySyncObjects()
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(rhiDevice);
    for (size_t i = 0; i < syncObjects.size(); i++)
    {
        //vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].renderFinishedSemaphore);
        vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].imageAvailableSemaphore);
        vulkanDevice->GetDevice().destroyFence(syncObjects[i].inFlightFence);
        //vulkanDevice->GetDevice().destroySemaphore(syncObjects[i].computeFinishedSemaphore);
        //vulkanDevice->GetDevice().destroyFence(syncObjects[i].computeInFlightFence);
    }
}
