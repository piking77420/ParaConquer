#pragma once

#include <memory>
#include <queue>
#include <unordered_set>

#include "CoreHeader.hpp"
#include "RenderInstance.hpp"
#include "PhysicalDevices.hpp"
#include "RhiDevice.hpp"
#include "RhiSampler.hpp"
#include "SwapChain.hpp"
#include "RhiFence.hpp"
#include "RhiResourceUpdate.hpp"

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

    PC_CORE_API RhiContext(Rhi& _Rhi);

    PC_CORE_API RhiContext() = delete;

    PC_CORE_API virtual ~RhiContext() = default;

    PC_CORE_API virtual void WaitIdle() = 0;

    PC_CORE_API virtual void SendEnqueuCommand(CommandList* _EnqueuCommands, PC_CORE::GpuPipelineStage waitStage) = 0;

    PC_CORE_API virtual void ProceedResourceUpdateBranch() = 0;

    PC_CORE_API RHI::ResourceUpdateBranch* ResourceUpdateBranch();

    PC_CORE_API RHI::ResourceUpdateBranch* ResourceUpdateBranch_AssumeLock();

    PC_CORE_API bool PendingTransferOperation() const;

    std::mutex lock;
protected:
    Rhi& m_Rhi;

    bool m_PendingTransferOperation = false;

    std::vector<std::unique_ptr<RHI::ResourceUpdateBranch>> m_ResourceUpdate;

};

END_PCCORE
