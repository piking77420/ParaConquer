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

class CommandList;
class RhiShaderProgram;
class RhiTexture;
class RhiBuffer;
class RhiSwapChain;

class Rhi
{
public:
    PC_CORE_API Rhi(const RenderHardwareInterfaceCreateInfo& _createInfo);

    PC_CORE_API Rhi() = default;

    PC_CORE_API ~Rhi();

    Rhi(const Rhi& other) noexcept = default; Rhi(Rhi&& other) noexcept = default; Rhi& operator=(const Rhi& other) noexcept = default; Rhi& operator=(Rhi&& other) noexcept = default;;

    GraphicAPI GetGraphicsApi() const { return m_GraphicsApi; }

    PC_CORE_API RhiSwapChain* CreateRhiSwapChain();

    PC_CORE_API RhiShaderProgram* CreateRhiShaderProgram(const std::string& _programName, const RhiShaderProgram::ProgramShaderCreateInfo& _programShaderCreateInfo);

    PC_CORE_API RhiShaderProgram* CreateRhiShaderProgram(std::string&& _programName, const RhiShaderProgram::ProgramShaderCreateInfo& _programShaderCreateInfo);

    PC_CORE_API CommandList* CreateCommandList(const std::string& name,const CommandListCreateInfo& _commandListCreateInfo);

    PC_CORE_API CommandList* CreateCommandList(std::string&& _name, const CommandListCreateInfo& _commandListCreateInfo);

    PC_CORE_API RhiRenderPass* CreateRenderPass(const std::string& _name, 
        RhiFormat _colorFormat, RhiFormat _depthFormat);

    PC_CORE_API RhiRenderPass* CreateRenderPass(const std::string& _name, RhiFormat _colorFormat);

    PC_CORE_API RhiRenderPass* CreateRenderPass(const std::string& _name, RhiFormat _colorFormat, uint32_t sampleCount);

    PC_CORE_API RhiRenderPass* CreateRenderPass(const std::string& _name, const RenderPassDescriptor& _renderPassDescriptor);

    PC_CORE_API RhiFrameBuffer* CreateFrameBuffer(const std::string& _name, const CreateFrameInfo& _createFrameInfo);

    PC_CORE_API RhiBuffer* CreateBuffer(const std::string& _name, const RhiBuffer::RhiBufferDescriptor& _rhiBufferDescriptor, RhiResource::MemoryUsage _memoryUsage);
        
    PC_CORE_API RhiTexture* CreateTexture(const std::string& _name, const RhiTexture::RhiTextureDesciptor& _rhiTextureDesciptor, RhiResource::MemoryUsage _memoryUsage);

    PC_CORE_API RhiTexture* CreateTexture(std::string&& _name, const RhiTexture::RhiTextureDesciptor& _rhiTextureDesciptor, RhiResource::MemoryUsage _memoryUsage);

    PC_CORE_API RhiSampler* CreateSampler(const std::string& _name, const SamplerCreateInfo& _samplerCreateInfo);

    PC_CORE_API RhiFence* CreateFence(const std::string& _name, const RhiFenceCreateInfo& _rhiFenceCreateInfo);

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

    void Init(const RenderHardwareInterfaceCreateInfo& _createInfo);

    void VulkanInitialize(const RhiContextCreateInfo& _createInfo);

    void DX12Initialize(const RhiContextCreateInfo& _createInfo);
};


END_PCCORE
