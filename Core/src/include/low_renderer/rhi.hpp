#pragma once

#include <memory>

#include "core_header.hpp"
#include "physical_devices.hpp"
#include "render_instance.hpp"
#include "rhi_typedef.h"
#include "io/window.hpp"

BEGIN_PCCORE
    struct RenderHardwareInterfaceCreateInfo
{
    PC_CORE::GraphicAPI GraphicsAPI;
    const Window* window;
    const char* appName;
};

class Rhi
{
public:
    std::shared_ptr<PC_CORE::RenderInstance> renderInstance;

    std::shared_ptr<PC_CORE::PhysicalDevices> physicalDevices;

    DEFAULT_COPY_MOVE_OPERATIONS(Rhi)

    Rhi(const RenderHardwareInterfaceCreateInfo& _createInfo);

    Rhi() = default;

    ~Rhi();

    inline PC_CORE::GraphicAPI GetGraphicsAPI() const { return m_GraphicsApi; }

    static Rhi& GetInstance();

private:
    static inline Rhi* m_Instance = nullptr;
    
    PC_CORE::GraphicAPI m_GraphicsApi;

    void Init(const RenderHardwareInterfaceCreateInfo& _createInfo);

    void VulkanInitialize(const RenderHardwareInterfaceCreateInfo& _createInfo);

    void DX12Initialize();

};
END_PCCORE