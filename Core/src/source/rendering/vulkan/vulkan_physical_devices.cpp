#include "..\..\..\include\rendering\vulkan\vulkan_physical_devices.hpp"

#include <cassert>

#include "log.hpp"

using namespace PC_CORE;

static void PrintImageUsageFlags(const VkImageUsageFlags& flags)
{
    if (flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        Log::Debug("Image usage transfer src is supported");
    }

    if (flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        Log::Debug("Image usage transfer dest is supported");
    }

    if (flags & VK_IMAGE_USAGE_SAMPLED_BIT) {
        Log::Debug("Image usage sampled is supported");
    }

    if (flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        Log::Debug("Image usage color attachment is supported");
    }

    if (flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        Log::Debug("Image usage depth stencil attachment is supported");
    }

    if (flags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
        Log::Debug("Image usage transient attachment is supported");
    }

    if (flags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
        Log::Debug("Image usage input attachment is supported");
    }
}

static void PrintMemoryProperty(VkMemoryPropertyFlags PropertyFlags)
{
    if (PropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        Log::Debug("DEVICE LOCAL ");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        Log::Debug("HOST VISIBLE ");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        Log::Debug("HOST COHERENT ");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
        Log::Debug("HOST CACHED ");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) {
        Log::Debug("LAZILY ALLOCATED ");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT) {
        Log::Debug("PROTECTED ");
    }
}


void VulkanPhysicalDevices::Init(VkInstance _instance, VkSurfaceKHR _surfaceKhr)
{
    Log::Debug("Get Physical Devices");

    uint32_t physicalDeviceCount = 0;
    VkResult res = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);
    VK_CHECK_ERROR(res, "vkEnumeratePhysicalDevices error")

    if (physicalDeviceCount == 0)
    {
        Log::Debug("There is no physical Device");
        exit(1);
    }

 
    m_Devices.resize(physicalDeviceCount);

    std::vector<VkPhysicalDevice> devices;
    devices.resize(physicalDeviceCount);

    res = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, devices.data());
    VK_CHECK_ERROR(res, "vkEnumeratePhysicalDevices error 2")

    for (size_t i = 0; i < devices.size(); i++)
    {
        Log::Debug("-----------------------------");
        
        const VkPhysicalDevice& phyDevice = devices[i];
        m_Devices[i].physDevice = phyDevice;
        
        vkGetPhysicalDeviceProperties(m_Devices[i].physDevice, &m_Devices[i].devProps);

        Log::Debug("[Device : " + std::string(m_Devices[i].devProps.deviceName) + ']');

        uint32_t numQFamilies = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &numQFamilies, nullptr);

        m_Devices[i].qFamilyProps.resize(numQFamilies);
        m_Devices[i].qSupportsPresent.resize(numQFamilies);
        m_Devices[i].qFamilyPropsStatus.resize(numQFamilies);

        vkGetPhysicalDeviceQueueFamilyProperties(m_Devices[i].physDevice, &numQFamilies, m_Devices[i].qFamilyProps.data());

        for (uint32_t q = 0; q < numQFamilies; q++)
        {
            const VkQueueFamilyProperties& QfamiliProp = m_Devices[i].qFamilyProps[q];

            Log::Debug("Families " + std::to_string(q) + " Num queues: " + std::to_string(QfamiliProp.queueCount));

            res = vkGetPhysicalDeviceSurfaceSupportKHR(phyDevice, q, _surfaceKhr, &(m_Devices[i].qSupportsPresent[q]));
            VK_CHECK_ERROR(res,"vkGetPhysicalDeviceSurfaceSupportKHR error")
        }

        uint32_t NumFormats = 0;
        res = vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevice, _surfaceKhr, &NumFormats, NULL);
        VK_CHECK_ERROR(res, "vkGetPhysicalDeviceSurfaceFormatsKHR (1)");
        assert(NumFormats > 0);

        m_Devices[i].surfaceFormats.resize(NumFormats);

        res = vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevice, _surfaceKhr, &NumFormats, m_Devices[i].surfaceFormats.data());
        VK_CHECK_ERROR(res, "vkGetPhysicalDeviceSurfaceFormatsKHR (2)");

        for (uint32_t j = 0; j < NumFormats; j++)
        {
            const VkSurfaceFormatKHR& SurfaceFormat = m_Devices[i].surfaceFormats[j];
            Log::Debug("Format "+ std::to_string(SurfaceFormat.format) +" color space "+ std::to_string(SurfaceFormat.colorSpace));
        }
        
        res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDevice, _surfaceKhr, &(m_Devices[i].surfaceCaps));
        VK_CHECK_ERROR(res,"vkGetPhysicalDeviceSurfaceCapabilitiesKHR error")

        
        PrintImageUsageFlags(m_Devices[i].surfaceCaps.supportedUsageFlags);
        uint32_t NumPresentModes = 0;
        res = vkGetPhysicalDeviceSurfacePresentModesKHR(phyDevice, _surfaceKhr, &NumPresentModes, NULL);
        VK_CHECK_ERROR(res, "vkGetPhysicalDeviceSurfacePresentModesKHR (1) error");

        if (NumPresentModes == 0)
        {
            Log::Debug("NumPresentModes == 0");
            exit(1);
        }

        m_Devices[i].presentModes.resize(NumPresentModes);

        res = vkGetPhysicalDeviceSurfacePresentModesKHR(phyDevice, _surfaceKhr, &NumPresentModes, m_Devices[i].presentModes.data());
        VK_CHECK_ERROR(res, "vkGetPhysicalDeviceSurfacePresentModesKHR (2) error");

        Log::Debug("Number of presentation modes " + std::to_string(NumPresentModes));

        vkGetPhysicalDeviceMemoryProperties(phyDevice, &(m_Devices[i].memProps));

        Log::Debug("Num memory types " + std::to_string(m_Devices[i].memProps.memoryTypeCount) );
        for (uint32_t j = 0; j < m_Devices[i].memProps.memoryTypeCount; j++) {
            printf("%d: flags %x heap %d ", j, 
                m_Devices[i].memProps.memoryTypes[j].propertyFlags,
                m_Devices[i].memProps.memoryTypes[j].heapIndex);

            PrintMemoryProperty(m_Devices[i].memProps.memoryTypes[j].propertyFlags);

        }
        Log::Debug("Num heap types " + std::to_string(m_Devices[i].memProps.memoryTypeCount));

        Log::Debug("Num heap types "+  std::to_string(m_Devices[i].memProps.memoryHeapCount));

        vkGetPhysicalDeviceFeatures(m_Devices[i].physDevice, &m_Devices[i].features);

        Log::Debug("-----------------------------");
    }
    
    
}

void VulkanPhysicalDevices::SelectDevice(const VkQueueFlags _RequiredQueueType, const bool _SupportsPresent)
{
    Log::Debug("SelectDevice");

    for (uint32_t i = 0; i < m_Devices.size(); i++)
    {
        // only select a gpu
        if (m_Devices[i].devProps.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            continue;

        
        for (uint32_t j = 0; j < m_Devices[i].qFamilyProps.size(); j++)
        {
            
            const VkQueueFamilyProperties& QFamilyProp = m_Devices[i].qFamilyProps[j];

            if ((QFamilyProp.queueFlags & _RequiredQueueType) && (static_cast<bool>(m_Devices[i].qSupportsPresent[j]) == _SupportsPresent))
            {
                m_CurrentDevice = static_cast<int>(i);
                const int QueueFamily = static_cast<int>(j);

                Log::Debug("Current Device name is " + std::string(m_Devices[i].devProps.deviceName));
                Log::Debug("Using GFX device " + std::to_string(m_CurrentDevice) + " and queue family " + std::to_string(QueueFamily));
                return;
            }
        }
    }

    VK_CHECK_ERROR(1,"Required queue type and supports present not found")
    
}

uint32_t VulkanPhysicalDevices::GetQueueFamilliesIndex(VkQueueFlags _RequiredQueueType, bool _SupportsPresent) const
{
    const PhysicalDevice& physicalDevice = GetCurrentDevice();
    
    for (uint32_t i = 0; i < physicalDevice.qFamilyProps.size(); i++)
    {
        const VkQueueFamilyProperties& QFamilyProp = physicalDevice.qFamilyProps[i];

        if (physicalDevice.qFamilyPropsStatus[i])
            continue;
        
        if (!_SupportsPresent && static_cast<bool>(physicalDevice.qSupportsPresent[i]) == _SupportsPresent && physicalDevice.qFamilyPropsStatus[i])
            continue;
        

        if ((QFamilyProp.queueFlags & _RequiredQueueType))
        {
            physicalDevice.qFamilyPropsStatus[i] = true;
            return i;
        }
    }

    VK_CHECK_ERROR(1,"Required queue type and supports present not found")
    return -1;
}


const PhysicalDevice& VulkanPhysicalDevices::GetCurrentDevice() const
{
    if (m_CurrentDevice < 0)
    {
        VK_CHECK_ERROR(1,"A physical device has not been selected");
    }
    return m_Devices[m_CurrentDevice];
}

PhysicalDevice& VulkanPhysicalDevices::GetCurrentDevice()
{
    if (m_CurrentDevice < 0)
    {
        VK_CHECK_ERROR(1,"A physical device has not been selected");
    }
    return m_Devices[m_CurrentDevice];
}
