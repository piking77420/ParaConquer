
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "vulkan_context.hpp"

#include "vulkan_swap_chain.hpp"

using namespace Vulkan;

VulkanContext::VulkanContext(const PC_CORE::RhiContextCreateInfo& rhiContextCreateInfo) : RhiContext(rhiContextCreateInfo)
{
    std::vector<std::string> extensionToEnable;
    
    renderInstance = std::make_shared<VulkanInstance>(*rhiContextCreateInfo.instanceCreate);
    InitSurface(rhiContextCreateInfo.WindowHandle);
    
    physicalDevices = std::make_shared<VulkanPhysicalDevices>(*rhiContextCreateInfo.physicalDevicesCreateInfo, &extensionToEnable);
    rhiDevice = std::make_shared<Vulkan::VulkanDevice>(std::reinterpret_pointer_cast<VulkanPhysicalDevices>(physicalDevices), extensionToEnable,  &graphicsQueue, &presentQueue);

    GLFWwindow* window = const_cast<GLFWwindow*>(static_cast<const GLFWwindow*>(rhiContextCreateInfo.WindowHandle));

    int32_t widht;
    int32_t height;
    glfwGetFramebufferSize(window, &widht, &height);

    const uint32_t uwidht = static_cast<uint32_t>(widht);
    const uint32_t uheight = static_cast<uint32_t>(height);
    swapChain = std::make_shared<Vulkan::VulkanSwapChain>(uwidht, uheight);
    
    CreateCommandPool();
}

VulkanContext::~VulkanContext()
{
    auto device = GetDevice();

    device->GetDevice().destroyCommandPool(commandPool);    
    commandPool = nullptr;

    std::shared_ptr<VulkanSwapChain> vSwapChain = std::reinterpret_pointer_cast<VulkanSwapChain>(m_CurrentContext->swapChain);

    
    std::shared_ptr<VulkanInstance> vulkanInstance = std::reinterpret_pointer_cast<VulkanInstance>(renderInstance);
    vulkanInstance->Get().destroySurfaceKHR(m_Surface, nullptr);
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

void VulkanContext::InitSurface(const void* _windowHandle)
{
    if (_windowHandle == nullptr) {
        throw std::invalid_argument("Window handle is null");
    }

    vk::Win32SurfaceCreateInfoKHR win32SurfaceCreate {};
    win32SurfaceCreate.sType = vk::StructureType::eWin32SurfaceCreateInfoKHR;

    const GLFWwindow* window = static_cast<const GLFWwindow*>(_windowHandle);
    win32SurfaceCreate.hwnd = glfwGetWin32Window(const_cast<GLFWwindow*>(window));
    win32SurfaceCreate.hinstance = GetModuleHandle(nullptr);

    auto vulkanInstance = std::static_pointer_cast<VulkanInstance>(renderInstance); 

    if (!vulkanInstance) {
        throw std::runtime_error("Failed to cast renderInstance to VulkanInstance.");
    }

    // Create the surface
    vk::Result r = vulkanInstance->Get().createWin32SurfaceKHR(&win32SurfaceCreate, nullptr, &m_Surface);
    VK_CHECK_CALL(r);
}

void VulkanContext::CreateCommandPool()
{
    const std::vector<QueueFamilyIndices>& queueFamilyIndices = GetPhysicalDevices()->GetQueuesFamilies();
    auto device = GetDevice();
    
    vk::CommandPoolCreateInfo commandPoolCreateInfo {};
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPoolCreateInfo.queueFamilyIndex = 0;
    
    commandPool = device->GetDevice().createCommandPool(commandPoolCreateInfo);
}
