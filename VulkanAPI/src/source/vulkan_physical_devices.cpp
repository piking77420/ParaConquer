#include "vulkan_physical_devices.hpp"

#include "log.hpp"
#include "vulkan_instance.hpp"
#include "low_renderer/rhi.hpp"


    


Vulkan::VulkanPhysicalDevices::VulkanPhysicalDevices(
    const PC_CORE::PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo)
: PhysicalDevices(_physicalDevicesCreateInfo)
{
    PC_CORE::Rhi& rhi = PC_CORE::Rhi::GetInstance();
    std::shared_ptr instanceInterface(std::reinterpret_pointer_cast<VulkanInstance>(rhi.renderInstance));
    vk::Instance vulkanInstance = instanceInterface->Get();

    // Get Vulkan Device NBR
    uint32_t physicalDeviceCount = 0;
    VK_CALL(vulkanInstance.enumeratePhysicalDevices(&physicalDeviceCount, nullptr));

    // Get Vulkan Device
    std::vector<vk::PhysicalDevice> vkPhysicalDevices;
    vkPhysicalDevices.resize(physicalDeviceCount);
    VK_CALL(vulkanInstance.enumeratePhysicalDevices(&physicalDeviceCount, vkPhysicalDevices.data()));
    
    std::vector<std::string> requestVulkanExtensions = GetVulkanRequestExtensions(_physicalDevicesCreateInfo.requestExtensions);
    
    m_PhysicalDevices.resize(vkPhysicalDevices.size());
    LookForSuitableDevices(vkPhysicalDevices, requestVulkanExtensions);

    std::string message = "Successfully created physical devices : " + GetPhysicalDevice().name;
    PC_LOG(message)
}

void Vulkan::VulkanPhysicalDevices::LookForSuitableDevices(const std::vector<vk::PhysicalDevice>& _physicalDevices, std::vector<std::string> _requestExtensions)
{
    std::vector<int32_t> devicesScore;
    devicesScore.resize(_physicalDevices.size());
    
    for (size_t i = 0; i < _physicalDevices.size(); i++)
    {
        const vk::PhysicalDevice& vkphysicalDevice = _physicalDevices[i];
        devicesScore[i] = GetDeviceScore(vkphysicalDevice, i);
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

    
    m_CurrentVulkanDevices = _physicalDevices[m_PhysicalDeviceIndex];
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

std::vector<std::string> Vulkan::VulkanPhysicalDevices::GetVulkanRequestExtensions(const std::vector<std::string>& _requestExtensions)
{
    std::vector<std::string> out;
    out.resize(_requestExtensions.size());

    for (size_t i = 0; i < _requestExtensions.size(); i++)
    {
        if (_requestExtensions[i] == SWAPCHAIN_EXT)
            out[i] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        
        if (_requestExtensions[i] == MESH_SHADER_EXT)
            out[i] = VK_EXT_mesh_shader;
        
        if (_requestExtensions[i] == RAY_TRACING_EXT)
            out[i] = VK_KHR_ray_tracing_pipeline;
    }

    return out;
}


int32_t Vulkan::VulkanPhysicalDevices::GetDeviceScore(const vk::PhysicalDevice& _physicalDevice, size_t _deviceIndex)
{
    int32_t score = 0;

    vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures = {};
    rayTracingFeatures.sType = vk::StructureType::ePhysicalDeviceRayTracingPipelineFeaturesKHR;

    vk::PhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures = {};
    meshShaderFeatures.sType = vk::StructureType::ePhysicalDeviceMeshShaderFeaturesEXT;
    meshShaderFeatures.pNext = &rayTracingFeatures;
    
    vk::PhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = vk::StructureType::ePhysicalDeviceFeatures2;
    deviceFeatures2.pNext = &meshShaderFeatures;

    vk::PhysicalDevice physicalDevice = _physicalDevice;
    physicalDevice.getFeatures2(&deviceFeatures2);

    vk::PhysicalDeviceProperties2 deviceProperties = {};
    deviceProperties.sType = vk::StructureType::ePhysicalDeviceProperties2;
    physicalDevice.getProperties2(&deviceProperties);

    GetDeviceProperties(&m_PhysicalDevices[_deviceIndex], deviceProperties.properties);
    GetDeviceFeatures(&m_PhysicalDevices[_deviceIndex], deviceFeatures2.features);


    
    // Evaluate score based on VkPhysicalDeviceFeatures
    size_t nbrOfBool = sizeof(vk::PhysicalDeviceFeatures) / sizeof(vk::PhysicalDeviceFeatures::samplerAnisotropy);

    static_assert(offsetof(vk::PhysicalDeviceFeatures, robustBufferAccess) == 0, "Robust buffer access should be the same");
    const vk::Bool32* ptr = &deviceFeatures2.features.robustBufferAccess;
    for (size_t i = 0; i < nbrOfBool; i++) {
        if (ptr[i] == vk::True)
            score++;
    }

    

    // Evaluate score based on vk::PhysicalDeviceRayTracingPipelineFeaturesKHR
    nbrOfBool = sizeof(vk::PhysicalDeviceRayTracingPipelineFeaturesKHR) / sizeof(vk::PhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipeline);
    ptr = &rayTracingFeatures.rayTracingPipeline;
    for (size_t i = 0; i < nbrOfBool; i++) {
        if (ptr[i] == vk::True)
            score++;
    }


    // Evaluate score based on vk::PhysicalDeviceMeshShaderFeaturesEXT
    nbrOfBool = sizeof(vk::PhysicalDeviceMeshShaderFeaturesEXT) / sizeof(vk::PhysicalDeviceMeshShaderFeaturesEXT::taskShader);
    ptr = &meshShaderFeatures.taskShader;
    for (size_t i = 0; i < nbrOfBool; i++) {
        if (ptr[i] == vk::True)
            score++;
    }
   
    return score;
}


