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


    class RhiContext
    {
    public:
        std::shared_ptr<RenderInstance> renderInstance;

        std::shared_ptr<PhysicalDevices> physicalDevices;

        std::shared_ptr<RhiDevice> rhiDevice;

        std::shared_ptr<SwapChain> swapChain;

        static RhiContext& GetContext()
        {
            return *m_CurrentContext;
        }

        PC_CORE_API RhiContext(const RhiContextCreateInfo& rhiContextCreateInfo);

        PC_CORE_API RhiContext() = delete;

        PC_CORE_API virtual ~RhiContext() = default;

        PC_CORE_API static void WaitIdle();

    protected:
        static inline RhiContext* m_CurrentContext = nullptr;

        PC_CORE_API virtual void WaitIdleInstance() = 0;


        std::vector<std::function<void(CommandList*)>> m_PendingResourceFuncion;
    };

END_PCCORE
