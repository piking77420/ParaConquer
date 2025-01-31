#pragma once

#include <memory>

#include "core_header.hpp"
#include "gpu_allocator.hpp"
#include "render_instance.hpp"
#include "physical_devices.hpp"
#include "rhi_device.hpp"
#include "swap_chain.hpp"

BEGIN_PCCORE
    struct RhiContextCreateInfo
    {
        GLFWwindow* WindowHandle = nullptr;
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

        std::shared_ptr<PC_CORE::GpuAllocator> gpuAllocator;
        
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

        PC_CORE_API static void WaitIdle();
    
    protected:
        static inline RhiContext* m_CurrentContext = nullptr;

        PC_CORE_API virtual void WaitIdleInstance() = 0;

    };

END_PCCORE
