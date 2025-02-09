#pragma once

#include <memory>

#include "command_list.hpp"
#include "core_header.hpp"
#include "rhi_context.hpp"
#include "rhi_typedef.h"
#include "io/window.hpp"
#include "resources/shader_program.h"

BEGIN_PCCORE
    struct RenderHardwareInterfaceCreateInfo
{
    PC_CORE::GraphicAPI GraphicsAPI;
    Window* window;
    const char* appName;
};

class Rhi
{
public:

    PC_CORE_API Rhi(Rhi&& other) noexcept;

    PC_CORE_API Rhi(const RenderHardwareInterfaceCreateInfo& _createInfo);

    PC_CORE_API Rhi() = default;

    PC_CORE_API ~Rhi();

    inline PC_CORE::GraphicAPI GetGraphicsAPI() const { return m_GraphicsApi; }

    PC_CORE_API Rhi& operator=(Rhi&& other) noexcept;

    PC_CORE_API static Rhi& GetInstance();

    PC_CORE_API static ShaderProgram* CreateShader(const ProgramShaderCreateInfo& _programShaderCreateInfo);

    PC_CORE_API static void DestroyShader(ShaderProgram* _shaderprogram);

    PC_CORE_API static std::shared_ptr<CommandList> CreateCommandList(const PC_CORE::CommandListCreateInfo& _commandListCreateInfo);

    PC_CORE_API static std::shared_ptr<RhiRenderPass> CreateRenderPass(PC_CORE::RHIFormat _colorFormat, PC_CORE::RHIFormat _depthFormat);

    PC_CORE_API static std::shared_ptr<RhiRenderPass> CreateRenderPass(PC_CORE::RHIFormat _colorFormat);

    PC_CORE_API static std::shared_ptr<FrameBuffer> CreateFrameBuffer(const CreateFrameInfo& _createFrameInfo);

    PC_CORE_API static RhiContext* GetRhiContext();
    
    PC_CORE_API static void NextFrame();
 
    PC_CORE_API static __forceinline  uint32_t GetFrameIndex() noexcept;

private:
    PC_CORE_API static inline Rhi* m_Instance = nullptr;
    
    PC_CORE::GraphicAPI m_GraphicsApi;

    RhiContext* m_RhiContext = nullptr;

    uint32_t  m_CurrentFrame = 0;

    void Init(const RenderHardwareInterfaceCreateInfo& _createInfo);

    void VulkanInitialize(const RhiContextCreateInfo& _createInfo);

    void DX12Initialize(const RhiContextCreateInfo& _createInfo);

};


END_PCCORE
