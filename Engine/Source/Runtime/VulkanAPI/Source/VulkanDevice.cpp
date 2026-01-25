#include "VulkanDevice.hpp"

#include "PerfRegion.hpp"

vk::Device Vulkan::VulkanDevice::GetDevice() const
{
    return m_Device;
}

Vulkan::VulkanDevice::VulkanDevice(const std::vector<PC_CORE::RhiExtension>& RhiExtension, 
                                    const std::shared_ptr<VulkanPhysicalDevices>& _vulkanPhysicalDevices,
                                   const std::set<std::string>& _extensionToEnable,
                                   vk::Queue* _graphicQueue)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::PhysicalDevice vkPhysicalDevice = _vulkanPhysicalDevices->GetVulkanDevice();

    constexpr uint32_t QueuIndex = 0;
    constexpr uint32_t QueueCount = 3;

    std::vector<QueueFamilyIndices> queueFamilies = _vulkanPhysicalDevices->GetQueuesFamilies();

    if (!(queueFamilies[QueuIndex].familyProperties.queueFlags & vk::QueueFlagBits::eCompute && queueFamilies[QueuIndex]
            .familyProperties.queueFlags & vk::QueueFlagBits::eCompute
            && queueFamilies[QueuIndex].familyProperties.queueFlags & vk::QueueFlagBits::eTransfer)
    )
    {
        throw std::runtime_error("Vulkan device does not support compute or transferrion");
    }

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo = {};
    std::array<float, QueueCount> queuePriority = {1.f, 1.f, 1.f};

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

#ifdef PROFILING
    VkPhysicalDeviceHostQueryResetFeatures hostQueryResetFeatures{};
    hostQueryResetFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &hostQueryResetFeatures;

    vkGetPhysicalDeviceFeatures2(vkPhysicalDevice, &features2);

    hostQueryResetFeatures.hostQueryReset = VK_TRUE;
#endif // PROFILING

    vk::PhysicalDeviceExtendedDynamicState2FeaturesEXT extendedFeatures2{};
    extendedFeatures2.sType =
        vk::StructureType::ePhysicalDeviceExtendedDynamicState2FeaturesEXT;

    vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT extendedFeatures3{};
    extendedFeatures3.sType =
        vk::StructureType::ePhysicalDeviceExtendedDynamicState3FeaturesEXT;
    extendedFeatures3.pNext = &extendedFeatures2;


    VkBaseOutStructure* chainHead =
        reinterpret_cast<VkBaseOutStructure*>(&extendedFeatures3);

    vk::PhysicalDeviceMeshShaderFeaturesEXT meshFeatures{};
    for (auto ext : RhiExtension)
    {
        switch (ext)
        {
        case PC_CORE::RhiExtension::MeshShader:
            meshFeatures.sType =
                vk::StructureType::ePhysicalDeviceMeshShaderFeaturesEXT;
            meshFeatures.meshShader = vk::True;
            meshFeatures.taskShader = vk::False;

            meshFeatures.pNext = chainHead;
            chainHead = reinterpret_cast<VkBaseOutStructure*>(&meshFeatures);
            break;

        default:
            break;
        }
    }

#ifdef PROFILING
    hostQueryResetFeatures.pNext = chainHead;
    chainHead =
        reinterpret_cast<VkBaseOutStructure*>(&hostQueryResetFeatures);
#endif

    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
    deviceCreateInfo.pNext = chainHead; // ✅ CORRECT
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfo.data();
    deviceCreateInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfo.size());
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount =
        static_cast<uint32_t>(enabledExtensionNames.size());
    deviceCreateInfo.ppEnabledExtensionNames =
        enabledExtensionNames.data();

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
    GetExtensionFunctions();

    if (_graphicQueue != nullptr)
        *_graphicQueue = m_Device.getQueue(QueuIndex, 0);
}

Vulkan::VulkanDevice::~VulkanDevice()
{
    if (m_Device != nullptr)
    {
        m_Device.destroy();
    }
}

void Vulkan::VulkanDevice::GetExtensionFunctions()
{
#ifdef  PROFILING
    m_Qpreset = reinterpret_cast<PFN_vkResetQueryPoolEXT>(
        vkGetDeviceProcAddr(m_Device, "vkResetQueryPoolEXT")
    );
    m_Gct = reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(
        vkGetDeviceProcAddr(m_Device, "vkGetCalibratedTimestampsEXT")
    );

    if (m_Gct == nullptr || m_Qpreset == nullptr)
    {
        PC_LOGERROR("Enable to get profiling func");
        assert(false);
    }
#endif
}
