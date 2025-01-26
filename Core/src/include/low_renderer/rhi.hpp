#pragma once

#include <memory>

#include "core_header.hpp"
#include "rhi_context.hpp"
#include "rhi_typedef.h"
#include "io/window.hpp"
#include "resources/shader_program.h"

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
    DEFAULT_COPY_MOVE_OPERATIONS(Rhi)

    Rhi(const RenderHardwareInterfaceCreateInfo& _createInfo);

    Rhi() = default;

    ~Rhi();

    inline PC_CORE::GraphicAPI GetGraphicsAPI() const { return m_GraphicsApi; }

    static Rhi& GetInstance();

    static ShaderProgram* CreateShader(const ProgramShaderCreateInfo& _programShaderCreateInfo);

private:
    
    static inline Rhi* m_Instance = nullptr;
    
    PC_CORE::GraphicAPI m_GraphicsApi;

    std::shared_ptr<RhiContext> m_RhiContext;

    void Init(const RenderHardwareInterfaceCreateInfo& _createInfo);

    void VulkanInitialize(const RhiContextCreateInfo& _createInfo);

    void DX12Initialize(const RhiContextCreateInfo& _createInfo);

};
END_PCCORE