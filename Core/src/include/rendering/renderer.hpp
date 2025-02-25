#pragma once
#include "light.hpp"
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


struct ViewDirExtremum
{
    Tbx::Vector3f topLeft;
    Tbx::Vector3f topRight;
    Tbx::Vector3f bottomLeft;
    Tbx::Vector3f bottomRight;
};


class Renderer
{
public:

    std::shared_ptr<PC_CORE::CommandList> primaryCommandList;

    std::shared_ptr<PC_CORE::ShaderProgram> m_ForwardShader;

    std::shared_ptr<PC_CORE::ShaderProgram> m_DrawTextureScreenQuadShader;

    std::shared_ptr<RhiRenderPass> forwardPass;

    std::shared_ptr<RhiRenderPass> drawTextureScreenQuadPass;
    
    PC_CORE_API Renderer() = default;

    PC_CORE_API ~Renderer() = default;
    
    PC_CORE_API void Init();

    PC_CORE_API void Destroy();

    PC_CORE_API bool BeginDraw(Window* _window);
    
    PC_CORE_API void DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext, Gbuffers* gbuffers, World* _world);

    PC_CORE_API void SwapBuffers(Window* _window);

    PC_CORE_API void DrawTextureScreenQuad(const ShaderProgramDescriptorSets& _ShaderProgramDescriptorSets);

private:
    World* m_CurrentWorld;
    
    RhiContext* m_RhiContext;
    
    ShaderProgramDescriptorSets* m_ShaderProgramDescriptorSet = nullptr;

    UniformBuffer cameraUniformBuffer;

    SceneBufferGPU sceneBufferGPU;

    ViewDirExtremum m_ViewExtremum;

    UniformBuffer m_ViewExtrmumUniformBuffer;

    std::unique_ptr<SceneLightsBuffer> sceneLightsBuffer;

    const RenderingContext* currentRenderingContext = nullptr;
    
    PC_CORE_API void UpdateCameraUniformBuffer(const PC_CORE::RenderingContext& renderingContext);

    PC_CORE_API void UpdateViewExtremumBuffer(const PC_CORE::RenderingContext& renderingContext);

    PC_CORE_API void QueryWorldData(World* world);

    PC_CORE_API void QueryLightDirData(DirLight& dirLight, Transform& transform);
    
    PC_CORE_API void CreateForwardShader();

    PC_CORE_API void CreateDrawQuadShader();

    PC_CORE_API void DrawStaticMesh(PC_CORE::Transform& _transform, PC_CORE::StaticMesh& _staticMesh);

    PC_CORE_API void AtmoSpherePass();
};

END_PCCORE