#include "vulkan_device.hpp"

vk::Device Vulkan::VulkanDevice::GetDevice() const
{
    return m_Device;
}

Vulkan::VulkanDevice::VulkanDevice(const std::shared_ptr<VulkanPhysicalDevices>& _vulkanPhysicalDevices, const std::vector<std::string>& _extensionToEnable,  vk::Queue* _graphicQueue ,
                                   vk::Queue* _presentQueue,  vk::Queue* _transferQueue)
{
    vk::PhysicalDevice vkPhysicalDevice = _vulkanPhysicalDevices->GetVulkanDevice();

    constexpr uint32_t QueuIndex = 0;
    constexpr uint32_t QueueCount= 3;

    std::vector<Vulkan::QueueFamilyIndices> queueFamilies = _vulkanPhysicalDevices->GetQueuesFamilies();
    
    if (!(queueFamilies[QueuIndex].familyProperties.queueFlags & vk::QueueFlagBits::eCompute && queueFamilies[QueuIndex].familyProperties.queueFlags & vk::QueueFlagBits::eCompute
        && queueFamilies[QueuIndex].familyProperties.queueFlags & vk::QueueFlagBits::eTransfer)
        )
    {
        throw std::runtime_error("Vulkan device does not support compute or transferrion");
    }
    
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo = {};
    std::array<float, QueueCount> queuePriority  = {1.f, 1.f, 1.f};

    queueCreateInfo.resize(1);
    for (uint32_t i = 0; i < 1; i++)
    {
        queueCreateInfo[i].sType = vk::StructureType::eDeviceQueueCreateInfo;
        queueCreateInfo[i].queueFamilyIndex = QueuIndex;
        queueCreateInfo[i].queueCount = QueueCount;
        queueCreateInfo[i].pQueuePriorities = queuePriority.data();
    }


    vk::PhysicalDeviceFeatures deviceFeatures = vkPhysicalDevice.getFeatures();

    std::vector<const char*> enabledExtensionNames;
    enabledExtensionNames.reserve(_extensionToEnable.size());
    
    for (const auto& extension : _extensionToEnable)
        {
        enabledExtensionNames.emplace_back(extension.c_str());
    }
    
    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfo.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfo.size());
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
    deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
    
#ifdef _DEBUG
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
    }
    
#endif
    m_Device = vkPhysicalDevice.createDevice(deviceCreateInfo, nullptr);

    
    if (_graphicQueue != nullptr)
        *_graphicQueue = m_Device.getQueue(QueuIndex, 0);

    if (_presentQueue != nullptr)
        *_presentQueue = m_Device.getQueue(QueuIndex, 1);

    if (_transferQueue != nullptr)
        *_transferQueue = m_Device.getQueue(QueuIndex, 2);
}

Vulkan::VulkanDevice::~VulkanDevice()
{
    if (m_Device != nullptr)
    {
        m_Device.destroy();
    }
}
