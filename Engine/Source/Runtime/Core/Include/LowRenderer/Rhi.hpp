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
class RhiDescriptorSet;

class Rhi
{
public:
    PC_CORE_API Rhi(const RenderHardwareInterfaceCreateInfo& _CreateInfo);

    PC_CORE_API ~Rhi();

    DEFAULT_COPY_MOVE_OPERATIONS(Rhi)

    GraphicAPI GetGraphicsApi() const { return m_GraphicsApi; }

    PC_CORE_API RhiSwapChain* CreateRhiSwapChain();

    PC_CORE_API RhiShaderProgram* CreateRhiShaderProgram();

    PC_CORE_API RhiDescriptorSet* CreateDescriptorSet();

    PC_CORE_API CommandList* CreateCommandList();

    PC_CORE_API RhiRenderPass* CreateRenderPass(RhiFormat _colorFormat, RhiFormat _depthFormat);

    PC_CORE_API RhiRenderPass* CreateRenderPass(RhiFormat _colorFormat);

    PC_CORE_API RhiRenderPass* CreateRenderPass(RhiFormat _colorFormat, uint32_t sampleCount);

    PC_CORE_API RhiRenderPass* CreateRenderPass();

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


private:
    GraphicAPI m_GraphicsApi{};

    uint32_t m_CurrentFrame = 0;

    std::unique_ptr<RhiContext> m_RhiContext = nullptr;

    void VulkanInitialize(const RhiContextCreateInfo& _CreateInfo);

    void DX12Initialize(const RhiContextCreateInfo& _CreateInfo);
};


END_PCCORE
