#pragma once
#include "core_header.hpp"
#include "rendering_typedef.h"
#include "render_pass.hpp"
#include "buffer/index_buffer.hpp"
#include "buffer/vertex_buffer.hpp"
#include "buffer/uniform_buffer.hpp"

#include "front_end/vulkan_app.hpp"
#include "render_harware_interface/command_pool.hpp"
#include "render_harware_interface/descriptor_pool.hpp"

#include "resources/shader_program.h"
#include "world/world.hpp"

BEGIN_PCCORE
    class Window;


struct RenderResources
{
    std::array<UniformBuffer, MAX_FRAMES_IN_FLIGHT> sceneUniform;
    SceneBufferGPU sceneBufferGPU;
};

class Renderer
{
public:
    static PC_CORE_API void RenderLog(LogType _logType, const char* _message);
public:

    RenderPass forwardRenderPass;

    RenderPass colorPass;

    ShaderProgram* shader = nullptr;

    
    PC_CORE_API void InitRHiAndObject(GraphicAPI _graphicAPI, Window* _window);

    PC_CORE_API void InitRenderResources();
    
    PC_CORE_API void Render(const PC_CORE::RenderingContext& _renderingContext, const PC_CORE::World& _world);

    PC_CORE_API void BeginFrame();

    PC_CORE_API void EndRender();
    
    PC_CORE_API void SwapBuffers();
    

    PC_CORE_API void WaitDevice();

    PC_CORE_API void Destroy();

    PC_CORE_API CommandBuffer& GetCommandSwapChainBuffer();

    PC_CORE_API GraphicAPI GetGraphicsAPI();

private:

    GraphicAPI m_GraphicApi = GraphicAPI::VULKAN;
    
    size_t m_CurrentImage;

    CommandPool m_SwapChainCommandPool;
    
    std::array<CommandBuffer, MAX_FRAMES_IN_FLIGHT> m_SwapChainCommandBuffers;

    RenderResources renderResources;


    Window* Windowtpr = nullptr;
    
    CommandBuffer* m_CommandBuffer = nullptr;
    
    std::vector<PC_CORE::DescriptorSetHandle> descriptorSets;
    
    PC_CORE_API void InitCommandPools();

    PC_CORE_API void InitDescriptors();

    PC_CORE_API void InitShader();

    PC_CORE_API void InitBuffer();

    PC_CORE_API void UpdateUniforms(const RenderingContext& _renderingContext);

    PC_CORE_API void DrawStaticMesh(const RenderingContext& _renderingContext, const PC_CORE::World& _world);

    PC_CORE_API void CreateForwardPass();
};

END_PCCORE