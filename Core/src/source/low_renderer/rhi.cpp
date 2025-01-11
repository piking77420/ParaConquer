#include "low_renderer/rhi.hpp"

#include "log.hpp"
#include "rhi_vulkan_parser.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_physical_devices.hpp"

using namespace PC_CORE;


Rhi::Rhi(const RenderHardwareInterfaceCreateInfo& _createInfo) : m_GraphicsApi(_createInfo.GraphicsAPI)
{
    PC_LOG("Rhi Initialize")
    if (m_Instance != nullptr)
    {
        PC_LOGERROR("Rhi already Existing");
        return;
    }
    m_Instance = this;
   
    Init(_createInfo);
}

Rhi::~Rhi()
{
    if (m_Instance != nullptr && renderInstance != nullptr)
    {
        PC_LOG("Rhi Deinitialized");
        m_Instance = nullptr;
    }
}

Rhi& Rhi::GetInstance()
{
    return *m_Instance;
}

void Rhi::Init(const RenderHardwareInterfaceCreateInfo& _createInfo)
{
    switch (m_GraphicsApi)
    {
    case GraphicAPI::VULKAN:
        VulkanInitialize(_createInfo);
        break;
    case GraphicAPI::DX3D12:
        DX12Initialize();
        break;
    case GraphicAPI::COUNT:
    case GraphicAPI::NONE:
        break;
    }
}

void Rhi::VulkanInitialize(const RenderHardwareInterfaceCreateInfo& _createInfo)
{
    RenderInstanceCreateInfo renderInstanceCreateInfo =
        {
        .appName = _createInfo.appName
        };
    
    renderInstance = std::make_shared<Vulkan::VulkanInstance>(renderInstanceCreateInfo);

    const PhysicalDevicesCreateInfo& physicalDevicesCreateInfo =
        {
            {
                SWAPCHAIN_EXT,MESH_SHADER_EXT,RAY_TRACING_EXT
            }
        };

    physicalDevices = std::make_shared<Vulkan::VulkanPhysicalDevices>(physicalDevicesCreateInfo);
}

void Rhi::DX12Initialize()
{
    
}
