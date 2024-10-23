﻿#pragma once
#include "core_header.hpp"
#include "rendering_typedef.h"
#include "buffer/index_buffer.hpp"
#include "buffer/vertex_buffer.hpp"
#include "buffer/uniform_buffer.hpp"

#include "front_end/vulkan_app.hpp"
#include "render_harware_interface/command_pool.hpp"

#include "resources/shader_program.h"

BEGIN_PCCORE
    class Window;

enum class GraphicAPI
{
    NONE,
    VULKAN,
    DX3D12,
    COUNT
};

class Renderer
{
public:
    static PC_CORE_API void RenderLog(LogType _logType, const char* _message);
public:
    PC_CORE_API void Init(GraphicAPI _graphicAPI, Window* _window);

    PC_CORE_API void Destroy();

    PC_CORE_API void Render(const PC_CORE::RenderingContext& _renderingContext);

    PC_CORE_API void BeginFrame();
    
    PC_CORE_API void SwapBuffers();

    PC_CORE_API void WaitDevice();

private:
    PC_CORE_API void InitRhi(GraphicAPI _graphicAPI, Window* _window);

    PC_CORE_API void InitShader();

    PC_CORE_API void InitBuffer();
    
    CommandPool m_SwapChainCommandPool;

    CommandPool m_TransfertPool;
    
    std::array<CommandBuffer, MAX_FRAMES_IN_FLIGHT> m_SwapChainCommandBuffers;

    ShaderProgram* m_MainShader = nullptr;

    Window* Windowtpr = nullptr;

    RHI* m_RhiRef = nullptr;

    CommandBuffer* m_CommandBuffer = nullptr;

    PC_CORE::VertexBuffer vertexBuffer;

    IndexBuffer indexBuffer;

    UniformBuffer sceneBufferUniform;

    SceneBufferGPU sceneBufferGPU;

    void InitCommandPools();
};

END_PCCORE