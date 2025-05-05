#include "vulkan_physical_devices.hpp"

#include <set>

#include "log.hpp"
#include "vulkan_context.hpp"
#include "vulkan_instance.hpp"
#include "low_renderer/rhi.hpp"


float Vulkan::VulkanPhysicalDevice::GetMaxSamplerAnisotropy() const
{
    vk::PhysicalDeviceProperties properties;

    physicalDevice.getProperties(&properties);

    return properties.limits.maxSamplerAnisotropy;
}



vk::PhysicalDevice Vulkan::VulkanPhysicalDevices::GetVulkanDevice() const
{
    return reinterpret_cast<VulkanPhysicalDevice*>(m_PhysicalDevices[m_PhysicalDeviceIndex])->physicalDevice;
}


const std::vector<Vulkan::QueueFamilyIndices>& Vulkan::VulkanPhysicalDevices::GetQueuesFamilies()
{
    return m_QueuesFamiliesProperty;
}

Vulkan::SwapChainSupportDetails Vulkan::VulkanPhysicalDevices::GetSwapChainSupportDetails() const
{
    return m_SwapChainSupportDetails;
}

Vulkan::VulkanPhysicalDevices::VulkanPhysicalDevices(
    const PC_CORE::PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo, std::vector<std::string>* _extensionToEnable)
    : PhysicalDevices(_physicalDevicesCreateInfo)
{
    Initialize(_physicalDevicesCreateInfo, _extensionToEnable);
    QueryQueueFamilies();
}

Vulkan::VulkanPhysicalDevices::~VulkanPhysicalDevices()
{
    for (auto& device : m_PhysicalDevices)
        delete device;
}

Vulkan::SwapChainSupportDetails Vulkan::VulkanPhysicalDevices::UpdateSwapChainSupport(const vk::SurfaceKHR& _surfaceKhr)
{
    vk::PhysicalDevice device = GetSelectedPhysicalDevice()->physicalDevice;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surfaceKhr, reinterpret_cast<VkSurfaceCapabilitiesKHR*>(&m_SwapChainSupportDetails.capabilities));
    m_SwapChainSupportDetails.formats = device.getSurfaceFormatsKHR(_surfaceKhr);
    m_SwapChainSupportDetails.presentModes = device.getSurfacePresentModesKHR(_surfaceKhr);
    return m_SwapChainSupportDetails;
}

void Vulkan::VulkanPhysicalDevices::LookForSuitableDevices(const std::vector<vk::PhysicalDevice>& _physicalDevices,
                                                           const std::vector<std::string>& _requestExtensions)
{
    std::vector<int32_t> devicesScore;
    devicesScore.resize(_physicalDevices.size());

    for (size_t i = 0; i < _physicalDevices.size(); i++)
    {
        const vk::PhysicalDevice& vkphysicalDevice = _physicalDevices[i];
        devicesScore[i] = GetDeviceScore(vkphysicalDevice, _requestExtensions,  i);
    }

    int32_t deviceBestScore = std::numeric_limits<int32_t>::min();
    int32_t deviceBestScoreIndex = std::numeric_limits<int32_t>::min();
    for (size_t i = 0; i < devicesScore.size(); i++)
    {
        if (devicesScore[i] > deviceBestScore)
        {
            deviceBestScoreIndex = static_cast<int32_t>(i);
            deviceBestScore = devicesScore[i];
        }
    }

    if (deviceBestScoreIndex == std::numeric_limits<int32_t>::min())
    {
        throw std::runtime_error("failed to find suitable GPU!");
    }
    m_PhysicalDeviceIndex = deviceBestScoreIndex;
    
    VulkanContext& vulkanContext = reinterpret_cast<VulkanContext&>(VulkanContext::GetContext());
    const Vulkan::SwapChainSupportDetails swapChainSupportDetails = QuerySwapChainSupport(vulkanContext.GetSurface(), GetSelectedPhysicalDevice()->physicalDevice);
    m_SwapChainSupportDetails = swapChainSupportDetails;
}

void Vulkan::VulkanPhysicalDevices::GetDeviceProperties(PC_CORE::PhysicalDevice* _physicalDevice,
                                                        const vk::PhysicalDeviceProperties& _physicalDeviceProperties)
{
    _physicalDevice->name = _physicalDeviceProperties.deviceName.data();
    _physicalDevice->driverVersion = _physicalDeviceProperties.driverVersion;
}

void Vulkan::VulkanPhysicalDevices::GetDeviceFeatures(PC_CORE::PhysicalDevice* _physicalDevice,
                                                      const vk::PhysicalDeviceFeatures& _physicalDeviceProperties)
{
}

std::vector<std::string> Vulkan::VulkanPhysicalDevices::GetVulkanRequestExtensions(
    const std::vector<std::string>& _requestExtensions)
{
    std::vector<std::string> out;
    out.resize(_requestExtensions.size());

    for (size_t i = 0; i < _requestExtensions.size(); i++)
    {
        if (_requestExtensions[i] == SWAPCHAIN_EXT)
            out[i] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

        if (_requestExtensions[i] == MESH_SHADER_EXT)
            out[i] = VK_EXT_MESH_SHADER_EXTENSION_NAME;

        if (_requestExtensions[i] == ACCELERATION_EXT)
            out[i] = VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME;

        if (_requestExtensions[i] == RAY_TRACING_EXT)
            out[i] = VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME;

        if (_requestExtensions[i] == DEFFERED_HOST_OP)
            out[i] = VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME;
    }

    return out;
}

bool Vulkan::VulkanPhysicalDevices::CheckDeviceExtensionSupport(
    const std::vector<vk::ExtensionProperties>& availableExtensions,
    const std::set<std::string>& requiredExtensions)
{
    std::set<std::string> remainingExtensions(requiredExtensions.begin(), requiredExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        remainingExtensions.erase(extension.extensionName);
    }

    return remainingExtensions.empty();
}

void Vulkan::VulkanPhysicalDevices::QueryQueueFamilies()
{
    vk::PhysicalDevice currentDevice = GetSelectedPhysicalDevice()->physicalDevice;

    
    uint32_t queueFamilyCount = 0;
    currentDevice.getQueueFamilyProperties(&queueFamilyCount, nullptr);

    std::vector<vk::QueueFamilyProperties> queueFamilyPropertieses = std::vector<vk::QueueFamilyProperties>(queueFamilyCount);
    currentDevice.getQueueFamilyProperties(&queueFamilyCount, queueFamilyPropertieses.data());
    m_QueuesFamiliesProperty.resize(queueFamilyPropertieses.size());

    vk::SurfaceKHR surfaceKhr = reinterpret_cast<VulkanContext&>(Vulkan::VulkanContext::GetContext()).GetSurface();

    for (size_t i = 0; i < queueFamilyPropertieses.size(); i++)
    {
        m_QueuesFamiliesProperty[i].familyProperties = queueFamilyPropertieses[i];
        m_QueuesFamiliesProperty[i].supportPresent = currentDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surfaceKhr);
    }
}

Vulkan::SwapChainSupportDetails Vulkan::VulkanPhysicalDevices::QuerySwapChainSupport(const vk::SurfaceKHR& _surfaceKhr,
    vk::PhysicalDevice _physicalDevices)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevices, _surfaceKhr, reinterpret_cast<VkSurfaceCapabilitiesKHR*>(&details.capabilities));

    details.formats = _physicalDevices.getSurfaceFormatsKHR(_surfaceKhr);
    details.presentModes = _physicalDevices.getSurfacePresentModesKHR(_surfaceKhr);
    return details;
}


Vulkan::VulkanPhysicalDevice* Vulkan::VulkanPhysicalDevices::GetSelectedPhysicalDevice()
{
    return reinterpret_cast<Vulkan::VulkanPhysicalDevice*>(m_PhysicalDevices[m_PhysicalDeviceIndex]);
}

void Vulkan::VulkanPhysicalDevices::Initialize(const PC_CORE::PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo, std::vector<std::string>* _extensionToEnable)
{
    PC_CORE::RhiContext& RhiContext = PC_CORE::RhiContext::GetContext();
    std::shared_ptr instanceInterface(std::reinterpret_pointer_cast<VulkanInstance>(RhiContext.renderInstance));
    vk::Instance vulkanInstance = instanceInterface->Get();

    // Get Vulkan Device NBR
    uint32_t physicalDeviceCount = 0;
    VK_CHECK_CALL(vulkanInstance.enumeratePhysicalDevices(&physicalDeviceCount, nullptr));

    // Get Vulkan Device
    std::vector<vk::PhysicalDevice> vkPhysicalDevices;
    vkPhysicalDevices.resize(physicalDeviceCount);
    VK_CHECK_CALL(vulkanInstance.enumeratePhysicalDevices(&physicalDeviceCount, vkPhysicalDevices.data()));

    // Get Extension as vulkna ext
    std::vector<std::string> requestVulkanExtensions = GetVulkanRequestExtensions(
        _physicalDevicesCreateInfo.requestExtensions);

    // Look for base device
    m_PhysicalDevices.resize(vkPhysicalDevices.size());
    for (size_t i = 0; i < vkPhysicalDevices.size(); i++)
    {
        m_PhysicalDevices[i] = new VulkanPhysicalDevice();
    }
    
    
    LookForSuitableDevices(vkPhysicalDevices, requestVulkanExtensions);
    *_extensionToEnable = requestVulkanExtensions;

    std::string message = "Successfully created physical devices : " + GetPhysicalDevice().name;
    PC_LOG(message)
}

int32_t Vulkan::VulkanPhysicalDevices::GetDeviceScore(const vk::PhysicalDevice& _physicalDevice,
                                                      const std::vector<std::string>& _requestExtensions,
                                                      size_t _deviceIndex)
{

    VulkanPhysicalDevice* myPhysicalDevice = reinterpret_cast<VulkanPhysicalDevice*>(m_PhysicalDevices[_deviceIndex]);
    myPhysicalDevice->physicalDevice = _physicalDevice;
    
    int32_t score = 0;

    uint32_t extensionCount;
    VK_CALL(myPhysicalDevice->physicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr));

    std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
    VK_CALL(myPhysicalDevice->physicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()));
    std::set<std::string> requiredExtensions(_requestExtensions.begin(), _requestExtensions.end());

    if (!CheckDeviceExtensionSupport(availableExtensions, requiredExtensions))
    {
        score = std::numeric_limits<int32_t>::min();
        PC_LOGERROR("Unsuported extension!")
    }
    
    VulkanContext* vulkanContext = reinterpret_cast<VulkanContext*>(&VulkanContext::GetContext());
    Vulkan::SwapChainSupportDetails swapChainSupportDetails = QuerySwapChainSupport(vulkanContext->GetSurface(), myPhysicalDevice->physicalDevice);

    if (swapChainSupportDetails.formats.empty() || swapChainSupportDetails.presentModes.empty())
    {
        score = std::numeric_limits<int32_t>::min();
        PC_LOGERROR("Unsuported surface!")
    }

    vk::PhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = vk::StructureType::ePhysicalDeviceFeatures2;

    myPhysicalDevice->physicalDevice.getFeatures2(&deviceFeatures2);

    vk::PhysicalDeviceProperties2 deviceProperties = {};
    deviceProperties.sType = vk::StructureType::ePhysicalDeviceProperties2;
    myPhysicalDevice->physicalDevice.getProperties2(&deviceProperties);

    GetDeviceProperties(myPhysicalDevice, deviceProperties.properties);
    GetDeviceFeatures(myPhysicalDevice, deviceFeatures2.features);
    // Evaluate score based on VkPhysicalDeviceFeatures
    size_t nbrOfBool = sizeof(vk::PhysicalDeviceFeatures) / sizeof(vk::PhysicalDeviceFeatures::samplerAnisotropy);

    static_assert(
        offsetof(vk::PhysicalDeviceFeatures, robustBufferAccess) == 0, "Robust buffer access should be the same");
    const vk::Bool32* ptr = &deviceFeatures2.features.robustBufferAccess;
    for (size_t i = 0; i < nbrOfBool; i++)
    {
        if (ptr[i] == vk::True)
            score++;
    }

    

    return score;
}
