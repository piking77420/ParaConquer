
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "vulkan_context.hpp"

#include <vma/vk_mem_alloc.h>

#include "vulkan_gpu_allocator.hpp"
#include "vulkan_swap_chain.hpp"

using namespace Vulkan;

VulkanContext::VulkanContext(const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo) : RhiContext(rhiContextCreateInfo)
{
    std::vector<std::string> extensionToEnable;
    
    renderInstance = std::make_shared<VulkanInstance>(*rhiContextCreateInfo.instanceCreate, rhiContextCreateInfo.WindowHandle);
    
    physicalDevices = std::make_shared<VulkanPhysicalDevices>(*rhiContextCreateInfo.physicalDevicesCreateInfo, &extensionToEnable);
    rhiDevice = std::make_shared<Vulkan::VulkanDevice>(std::reinterpret_pointer_cast<VulkanPhysicalDevices>(physicalDevices), extensionToEnable,  &graphicsQueue, &presentQueue, &transferQueu);

    GLFWwindow* window = const_cast<GLFWwindow*>(static_cast<const GLFWwindow*>(rhiContextCreateInfo.WindowHandle));

    int32_t widht;
    int32_t height;
    glfwGetFramebufferSize(window, &widht, &height);

    const uint32_t uwidht = static_cast<uint32_t>(widht);
    const uint32_t uheight = static_cast<uint32_t>(height);
    swapChain = std::make_shared<Vulkan::VulkanSwapChain>(uwidht, uheight);

    CreateMemoryAllocator();
    
    CreateCommandPools();
}

VulkanContext::~VulkanContext()
{
    auto device = GetDevice();

    device->GetDevice().destroyCommandPool(commandPool);    
    commandPool = nullptr;
    device->GetDevice().destroyCommandPool(transferCommandPool);    
    transferCommandPool = nullptr;
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
  
    gpuAllocator = std::make_shared<Vulkan::VulkanGpuAllocator>(createInfo);
}

void VulkanContext::CreateCommandPools()
{
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
