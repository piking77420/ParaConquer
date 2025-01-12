#pragma once

#include <memory>

#include "core_header.hpp"
#include "render_instance.hpp"
#include "physical_devices.hpp"
#include "rhi_device.hpp"
#include "swap_chain.hpp"

BEGIN_PCCORE
    struct RhiContextCreateInfo
    {
        const void* WindowHandle = nullptr;
        RenderInstanceCreateInfo* instanceCreate;
        PhysicalDevicesCreateInfo* physicalDevicesCreateInfo;
    };


    class RhiContext
    {
    public:
        std::shared_ptr<PC_CORE::RenderInstance> renderInstance;

        std::shared_ptr<PC_CORE::PhysicalDevices> physicalDevices;

        std::shared_ptr<PC_CORE::RhiDevice> rhiDevice;

        std::shared_ptr<PC_CORE::SwapChain> swapChain;

        static RhiContext& GetContext() 
        {
            return *m_CurrentContext;
        }

        PC_CORE_API RhiContext(const RhiContextCreateInfo& rhiContextCreateInfo)  
        {
            m_CurrentContext = this;
        };

        PC_CORE_API RhiContext() = delete;

        PC_CORE_API virtual ~RhiContext() = default;

    private:
        static inline RhiContext* m_CurrentContext = nullptr;
    };

END_PCCORE
