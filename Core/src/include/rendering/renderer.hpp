#pragma once
#include "rendering_typedef.h"
#include "low_renderer/vertex_buffer.hpp"
#include "low_renderer/command_list.hpp"
#include "low_renderer/index_buffer.hpp"
#include "low_renderer/rhi_context.hpp"
#include "low_renderer/uniform_buffer.hpp"
#include "resources/scene_lights_manager.h"
#include "resources/shader_program.h"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"

BEGIN_PCCORE
    class Renderer
{
public:

    std::shared_ptr<PC_CORE::CommandList> primaryCommandList;

    PC_CORE::ShaderProgram* m_ForwardShader = nullptr;

    PC_CORE::ShaderProgram* m_DrawTextureScreenQuadShader = nullptr;

    std::shared_ptr<RhiRenderPass> forwardPass;

    std::shared_ptr<RhiRenderPass> drawTextureScreenQuadPass;

    SceneLightsBuffer* sceneLightsBuffer = nullptr;

    PC_CORE_API Renderer() = default;

    PC_CORE_API ~Renderer() = default;
    
    PC_CORE_API void Init();

    PC_CORE_API void Destroy();

    PC_CORE_API bool BeginDraw(Window* _window);
    
    PC_CORE_API void DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext, Gbuffers* gbuffers, World* _world);

    PC_CORE_API void SwapBuffers(Window* _window);

    PC_CORE_API void DrawTextureScreenQuad(const ShaderProgramDescriptorSets& _ShaderProgramDescriptorSets);

private:
    RhiContext* m_RhiContext;
    
    ShaderProgramDescriptorSets* m_ShaderProgramDescriptorSet = nullptr;

    UniformBuffer m_UniformBuffer;

    SceneBufferGPU sceneBufferGPU;
    

    PC_CORE_API void CreateForwardShader();

    PC_CORE_API void CreateDrawQuadShader();

    PC_CORE_API void DrawStaticMesh(PC_CORE::Transform& _transform, PC_CORE::StaticMesh& _staticMesh);
};

END_PCCORE