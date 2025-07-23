#pragma once

#include <memory>
#include <stack>

#include "core_header.hpp"
#include "rhi_typedef.h"

#include "command_list.hpp"
#include "io/window.hpp"
#include "rhi_context.hpp"

#include "rhi_index_buffer.hpp"
#include "rhi_shader_program.hpp"
#include "rhi_texture_3d.hpp"
#include "rhi_uniform_buffer.hpp"
#include "rhi_vertex_buffer.hpp"

#include "rhi_texure_2d.hpp"
#

BEGIN_PCCORE
    struct RenderHardwareInterfaceCreateInfo
{
    PC_CORE::GraphicAPI GraphicsAPI;
    Window* window;
    const char* appName;
    bool gpuDebug;
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

    PC_CORE_API static std::shared_ptr<RhiShaderProgram> CreateRhiShaderProgram(const ProgramShaderCreateInfo& _programShaderCreateInfo);
    
    PC_CORE_API static std::shared_ptr<CommandList> CreateCommandList(const PC_CORE::CommandListCreateInfo& _commandListCreateInfo);

    PC_CORE_API static std::shared_ptr<RhiRenderPass> CreateRenderPass(PC_CORE::RHIFormat _colorFormat, PC_CORE::RHIFormat _depthFormat);

    PC_CORE_API static std::shared_ptr<RhiRenderPass> CreateRenderPass(PC_CORE::RHIFormat _colorFormat);

    PC_CORE_API static std::shared_ptr<RhiRenderPass> CreateRenderPass(PC_CORE::RHIFormat _colorFormat, uint32_t sampleCount);

    PC_CORE_API static std::shared_ptr<RhiRenderPass> CreateRenderPass(const RenderPassDescriptor& _renderPassDescriptor);

    PC_CORE_API static std::shared_ptr<FrameBuffer> CreateFrameBuffer(const CreateFrameInfo& _createFrameInfo);

    PC_CORE_API static std::shared_ptr<RhiIndexBuffer> CreateIndexBuffer(const void* _data, uint32_t _sizeInByte, IndexFormat _format, MemoryLocalisation _visibility, MemoryUsage _usage); 

    PC_CORE_API static std::shared_ptr<RhiVertexBuffer> CreateVertexBuffer(const void* _data, uint32_t _sizeInByte, MemoryLocalisation _visibility, MemoryUsage _usage);

    PC_CORE_API static std::shared_ptr<RhiVertexBuffer> CreateVertexBuffer(uint32_t _sizeInByte, MemoryLocalisation _visibility, MemoryUsage _usage); 

    PC_CORE_API static std::shared_ptr<RhiUniformBuffer> CreateUniformBuffer(const void* _data, uint32_t _sizeInByte, MemoryLocalisation _visibility, MemoryUsage _usage);
    
    PC_CORE_API static std::shared_ptr<RhiTexture2D> CreateTexture2D(const PC_CORE::CreateImageInfo& _createImageInfo);

    PC_CORE_API static std::shared_ptr<RhiTexture3D> CreateTexture3D(const PC_CORE::CreateImageInfo& _createImageInfo3D);

    PC_CORE_API static std::shared_ptr<RhiSampler> CreateSampler(const PC_CORE::SamplerCreateInfo& _samplerCreateInfo);

    PC_CORE_API static std::shared_ptr<RhiFence> CreateFence(const RhiFenceCreateInfo& _rhiFenceCreateInfo);

    PC_CORE_API static RhiContext* GetRhiContext();
    
    PC_CORE_API static void NextFrame();
 
    PC_CORE_API static uint32_t GetFrameIndex() noexcept
    {
        return (m_Instance) ? m_Instance->m_CurrentFrame : 0;
    }


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
