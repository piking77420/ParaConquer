#pragma once

#include <Memory>
#include <Stack>

#include "CoreHeader.hpp"
#include "RhiTypedef.h"

#include "Io/Window.hpp"
#include "RhiContext.hpp"

BEGIN_PCCORE
struct RenderHardwareInterfaceCreateInfo
{
    GraphicAPI GraphicsAPI;
    Window* window;
    const char* appName;
    bool gpuDebug;
};

class RhiSwapChain;
class RhiShaderProgram;
class CommandList;
class RhiTexture;
class RhiBuffer;
class RhiSampler;
class RhiFence;

class Rhi
{
public:
    PC_CORE_API Rhi() = default;

    PC_CORE_API ~Rhi();

    DEFAULT_COPY_MOVE_OPERATIONS(Rhi)

    void Init(const RenderHardwareInterfaceCreateInfo& _createInfo);

    GraphicAPI GetGraphicsApi() const { return m_GraphicsApi; }

    PC_CORE_API RhiSwapChain* CreateRhiSwapChain();

    PC_CORE_API RhiShaderProgram* CreateRhiShaderProgram();

    PC_CORE_API CommandList* CreateCommandList();

    PC_CORE_API RhiRenderPass* CreateRenderPass(RhiFormat _colorFormat, RhiFormat _depthFormat);

    PC_CORE_API RhiRenderPass* CreateRenderPass(RhiFormat _colorFormat);

    PC_CORE_API RhiRenderPass* CreateRenderPass(RhiFormat _colorFormat, uint32_t sampleCount);

    PC_CORE_API RhiRenderPass* CreateRenderPass(const RenderPassDescriptor& _renderPassDescriptor);

    PC_CORE_API RhiFrameBuffer* CreateFrameBuffer();

    PC_CORE_API RhiBuffer* CreateBuffer();

    PC_CORE_API RhiTexture* CreateTexture();

    PC_CORE_API RhiSampler* CreateSampler();

    PC_CORE_API RhiFence* CreateFence();

    PC_CORE_API RhiContext& GetRhiContext();

    PC_CORE_API const RhiContext& GetRhiContext() const;

    PC_CORE_API void NextFrame();

    PC_CORE_API uint32_t GetFrameIndex() noexcept
    {
        return m_CurrentFrame;
    }

    PC_CORE_API void PushResourceUpdate(const std::function<void(CommandList*)>& _resourceUpdates);
        
    PC_CORE_API void ProcessResourceUpdate();

private:
    GraphicAPI m_GraphicsApi{};

    uint32_t m_CurrentFrame = 0;

    std::unique_ptr<RhiContext> m_RhiContext = nullptr;

    std::vector<std::function<void(CommandList*)>> m_ResourceUpdateStack;

    std::unique_ptr<CommandList> m_ResourceUpdateCommandList;

    std::unique_ptr<RhiFence> m_ResourceUpdateFence;

    void VulkanInitialize(const RhiContextCreateInfo& _createInfo);

    void DX12Initialize(const RhiContextCreateInfo& _createInfo);
};


END_PCCORE
