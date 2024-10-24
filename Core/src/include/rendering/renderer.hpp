#pragma once
#include "core_header.hpp"
#include "rendering_typedef.h"
#include "buffer/index_buffer.hpp"
#include "buffer/vertex_buffer.hpp"
#include "buffer/uniform_buffer.hpp"

#include "front_end/vulkan_app.hpp"
#include "render_harware_interface/command_pool.hpp"
#include "render_harware_interface/descriptor_pool.hpp"

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
    PC_CORE_API void InitRHiAndObject(GraphicAPI _graphicAPI, Window* _window);

    PC_CORE_API void InitRenderResources();
    
    PC_CORE_API void Destroy();

    PC_CORE_API void Render(const PC_CORE::RenderingContext& _renderingContext);

    PC_CORE_API void BeginFrame();
    
    PC_CORE_API void SwapBuffers();

    PC_CORE_API void WaitDevice();

private:
    
    CommandPool m_SwapChainCommandPool;

    CommandPool m_TransfertPool;
    
    std::array<CommandBuffer, MAX_FRAMES_IN_FLIGHT> m_SwapChainCommandBuffers;

    ShaderProgram* m_MainShader = nullptr;

    Window* Windowtpr = nullptr;
    
    CommandBuffer* m_CommandBuffer = nullptr;

    PC_CORE::VertexBuffer vertexBuffer;

    IndexBuffer indexBuffer;

    std::vector<UniformBuffer> m_SceneBufferUniforms;

    SceneBufferGPU sceneBufferGPU;

    DescriptorSetLayout descriptorSetLayout;

    DescriptorPool descriptorPool;


    PC_CORE_API void InitCommandPools();

    PC_CORE_API void InitDescriptors();

    PC_CORE_API void InitShader();

    PC_CORE_API void InitBuffer();

    PC_CORE_API void UpdateUniforms(const RenderingContext& _renderingContext);
};

END_PCCORE