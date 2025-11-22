#include "VulkanPhysicalDevices.hpp"

#include "Log.hpp"
#include "VulkanContext.hpp"
#include "VulkanInstance.hpp"
#include "LowRenderer/Rhi.hpp"


float Vulkan::VulkanPhysicalDevice::GetMaxSamplerAnisotropy() const
{
    vk::PhysicalDeviceProperties properties;

    physicalDevice.getProperties(&properties);

    return properties.limits.maxSamplerAnisotropy;
}

uint32_t Vulkan::VulkanPhysicalDevice::GetMaxUsableSampleCount() const
{
    vk::PhysicalDeviceProperties properties;
    physicalDevice.getProperties(&properties);

    vk::SampleCountFlags counts = properties.limits.framebufferColorSampleCounts & properties.limits.
        framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64) { return 64; }
    if (counts & vk::SampleCountFlagBits::e32) { return 32; }
    if (counts & vk::SampleCountFlagBits::e16) { return 16; }
    if (counts & vk::SampleCountFlagBits::e8) { return 8; }
    if (counts & vk::SampleCountFlagBits::e4) { return 4; }
    if (counts & vk::SampleCountFlagBits::e2) { return 2; }

    return 1;
}


vk::PhysicalDevice Vulkan::VulkanPhysicalDevices::GetVulkanDevice() const
{
    return reinterpret_cast<VulkanPhysicalDevice*>(m_PhysicalDevices[m_PhysicalDeviceIndex])->physicalDevice;
}


const std::vector<Vulkan::QueueFamilyIndices>& Vulkan::VulkanPhysicalDevices::GetQueuesFamilies()
{
    return m_QueuesFamiliesProperty;
}

Vulkan::VulkanPhysicalDevices::VulkanPhysicalDevices(vk::Instance _VkInstance, vk::SurfaceKHR _Surface,
    const PC_CORE::PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo, std::set<std::string>* _extensionToEnable)
    : PhysicalDevices(_physicalDevicesCreateInfo)
    , m_VkInstance(_VkInstance)
{
    PERF_REGION_SCOPED;
    Initialize(_physicalDevicesCreateInfo, _extensionToEnable);
    QueryQueueFamilies(_Surface);
}

Vulkan::VulkanPhysicalDevices::~VulkanPhysicalDevices()
{
    for (auto& device : m_PhysicalDevices)
        delete device;
}

Vulkan::SwapChainSupportDetails Vulkan::VulkanPhysicalDevices::GetSwapChainSupport(const vk::SurfaceKHR& _surfaceKhr)
{
    SwapChainSupportDetails m_SwapChainSupportDetails;

    vk::PhysicalDevice device = GetSelectedPhysicalDevice()->physicalDevice;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surfaceKhr,
                                              reinterpret_cast<VkSurfaceCapabilitiesKHR*>(&m_SwapChainSupportDetails.
                                                  capabilities));
    m_SwapChainSupportDetails.formats = device.getSurfaceFormatsKHR(_surfaceKhr);
    m_SwapChainSupportDetails.presentModes = device.getSurfacePresentModesKHR(_surfaceKhr);
    return m_SwapChainSupportDetails;
}

void Vulkan::VulkanPhysicalDevices::LookForSuitableDevices(const std::vector<vk::PhysicalDevice>& _physicalDevices,
                                                           const std::set<std::string>& _requestExtensions)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::vector<int32_t> devicesScore;
    devicesScore.resize(_physicalDevices.size());

    for (size_t i = 0; i < _physicalDevices.size(); i++)
    {
        const vk::PhysicalDevice& vkphysicalDevice = _physicalDevices[i];
        devicesScore[i] = GetDeviceScore(vkphysicalDevice, _requestExtensions, i);
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
}

void Vulkan::VulkanPhysicalDevices::GetDeviceProperties(PC_CORE::PhysicalDevice* _physicalDevice,
                                                        const vk::PhysicalDeviceProperties& _physicalDeviceProperties,
                                                        size_t* _score)
{
    _physicalDevice->name = _physicalDeviceProperties.deviceName.data();
    _physicalDevice->driverVersion = _physicalDeviceProperties.driverVersion;

    const vk::PhysicalDeviceLimits& limits = _physicalDeviceProperties.limits;
    // TODO
}

void Vulkan::VulkanPhysicalDevices::GetDeviceFeatures(PC_CORE::PhysicalDevice* _physicalDevice,
                                                      const vk::PhysicalDeviceFeatures& _physicalDeviceProperties,
                                                      size_t* _score)
{
}

std::set<std::string> Vulkan::VulkanPhysicalDevices::GetVulkanRequestExtensions(
    const std::vector<PC_CORE::RhiExtension>& _requestExtensions)
{
    using namespace PC_CORE;

    std::set<std::string> out;

    for (size_t i = 0; i < _requestExtensions.size(); i++)
    {
        const auto rhiExt = _requestExtensions[i];
        switch (rhiExt)
        {
        case RhiExtension::RayTracing:
            out.emplace(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
            out.emplace(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
            out.emplace(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
            out.emplace(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
            out.emplace(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
            out.emplace(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
            out.emplace(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
            break;
        case RhiExtension::MeshShader:
            out.emplace(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
            out.emplace(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
            out.emplace(VK_EXT_MESH_SHADER_EXTENSION_NAME);

            break;
        default:
            break;
        }
    }

#ifdef PROFILING
    out.emplace(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME);
    out.emplace(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME);
#endif // PROFILING


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

void Vulkan::VulkanPhysicalDevices::QueryQueueFamilies(vk::SurfaceKHR _Surface)
{
    vk::PhysicalDevice currentDevice = GetSelectedPhysicalDevice()->physicalDevice;


    uint32_t queueFamilyCount = 0;
    currentDevice.getQueueFamilyProperties(&queueFamilyCount, nullptr);

    auto queueFamilyPropertieses = std::vector<vk::QueueFamilyProperties>(queueFamilyCount);
    currentDevice.getQueueFamilyProperties(&queueFamilyCount, queueFamilyPropertieses.data());
    m_QueuesFamiliesProperty.resize(queueFamilyPropertieses.size());

    for (size_t i = 0; i < queueFamilyPropertieses.size(); i++)
    {
        m_QueuesFamiliesProperty[i].familyProperties = queueFamilyPropertieses[i];
        m_QueuesFamiliesProperty[i].supportPresent = currentDevice.getSurfaceSupportKHR(
            static_cast<uint32_t>(i), _Surface);
    }
}

Vulkan::VulkanPhysicalDevice* Vulkan::VulkanPhysicalDevices::GetSelectedPhysicalDevice()
{
    return reinterpret_cast<VulkanPhysicalDevice*>(m_PhysicalDevices[m_PhysicalDeviceIndex]);
}

void Vulkan::VulkanPhysicalDevices::Initialize(const PC_CORE::PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo,
                                               std::set<std::string>* _extensionToEnable)
{
    // Get Vulkan Device NBR
    uint32_t physicalDeviceCount = 0;
    VK_CHECK_CALL(m_VkInstance.enumeratePhysicalDevices(&physicalDeviceCount, nullptr));

    // Get Vulkan Device
    std::vector<vk::PhysicalDevice> vkPhysicalDevices;
    vkPhysicalDevices.resize(physicalDeviceCount);
    VK_CHECK_CALL(m_VkInstance.enumeratePhysicalDevices(&physicalDeviceCount, vkPhysicalDevices.data()));

    // Get Extension as vulkna ext
    std::set<std::string> requestVulkanExtensions = GetVulkanRequestExtensions(
        _physicalDevicesCreateInfo.requestExtensions);

    // basic extension
    requestVulkanExtensions.emplace(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    requestVulkanExtensions.emplace(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
    requestVulkanExtensions.emplace(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME);
    requestVulkanExtensions.emplace(VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME);
    // Look for base device
    m_PhysicalDevices.resize(vkPhysicalDevices.size());
    for (size_t i = 0; i < vkPhysicalDevices.size(); i++)
        m_PhysicalDevices[i] = new VulkanPhysicalDevice();


    LookForSuitableDevices(vkPhysicalDevices, requestVulkanExtensions);
    *_extensionToEnable = requestVulkanExtensions;

    PC_LOG("Successfully created physical devices : {}", GetPhysicalDevice().name);
}

int32_t Vulkan::VulkanPhysicalDevices::GetDeviceScore(const vk::PhysicalDevice& _physicalDevice,
                                                      const std::set<std::string>& _requestExtensions,
                                                      size_t _deviceIndex)
{
    auto myPhysicalDevice = reinterpret_cast<VulkanPhysicalDevice*>(m_PhysicalDevices[_deviceIndex]);
    myPhysicalDevice->physicalDevice = _physicalDevice;

    size_t score = 0;

    uint32_t extensionCount;
    VK_CALL(myPhysicalDevice->physicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr));

    std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
    VK_CALL(
        myPhysicalDevice->physicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount,
            availableExtensions.data()));
    std::set<std::string> requiredExtensions(_requestExtensions.begin(), _requestExtensions.end());

    if (!CheckDeviceExtensionSupport(availableExtensions, requiredExtensions))
    {
        score = std::numeric_limits<size_t>::min();
        PC_LOGERROR("Unsuported extension!")

        for (const auto& extension : requiredExtensions)
        PC_LOGERROR("Extension requiered: {} ", extension);
    }

    vk::PhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = vk::StructureType::ePhysicalDeviceFeatures2;

    myPhysicalDevice->physicalDevice.getFeatures2(&deviceFeatures2);

    vk::PhysicalDeviceProperties2 deviceProperties = {};
    deviceProperties.sType = vk::StructureType::ePhysicalDeviceProperties2;
    myPhysicalDevice->physicalDevice.getProperties2(&deviceProperties);

    GetDeviceProperties(myPhysicalDevice, deviceProperties.properties, &score);
    GetDeviceFeatures(myPhysicalDevice, deviceFeatures2.features, &score);
    // Evaluate score based on VkPhysicalDeviceFeatures
    size_t nbrOfBool = sizeof(vk::PhysicalDeviceFeatures) / sizeof(vk::Bool32);

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
