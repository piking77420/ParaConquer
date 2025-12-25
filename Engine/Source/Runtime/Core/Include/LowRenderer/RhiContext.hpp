#pragma once

#include <Memory>

#include "CoreHeader.hpp"
#include "RenderInstance.hpp"
#include "PhysicalDevices.hpp"
#include "RhiDevice.hpp"
#include "RhiSampler.hpp"
#include "SwapChain.hpp"
#include "RhiFence.hpp"

BEGIN_PCCORE
    struct RhiContextCreateInfo
    {
        GLFWwindow* WindowHandle = nullptr;
        const RenderInstanceCreateInfo* instanceCreate;
        const PhysicalDevicesCreateInfo* physicalDevicesCreateInfo;
    };
    
    class Rhi;

    class RhiContext
    {
    public:
        std::shared_ptr<RenderInstance> renderInstance;

        std::shared_ptr<PhysicalDevices> rhiPhysicalDevices;

        std::shared_ptr<RhiDevice> rhiDevice;

        std::shared_ptr<RhiSwapChain> rhiSwapChain;

        PC_CORE_API RhiContext(Rhi& _Rhi, const RhiContextCreateInfo& rhiContextCreateInfo);

        PC_CORE_API RhiContext() = delete;

        PC_CORE_API virtual ~RhiContext() = default;

        PC_CORE_API virtual void WaitIdle() = 0;

    protected:

        Rhi& m_Rhi;
    };

END_PCCORE
