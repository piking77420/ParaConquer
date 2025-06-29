#pragma once
#include "light.hpp"
#include "rendering_typedef.h"
#include "low_renderer/rhi_vertex_buffer.hpp"
#include "low_renderer/command_list.hpp"
#include "low_renderer/rhi_index_buffer.hpp"
#include "low_renderer/rhi_context.hpp"
#include "low_renderer/rhi_uniform_buffer.hpp"
#include "resources/scene_lights_manager.h"
#include "resources/shader_program.h"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"

BEGIN_PCCORE

#define FORWARD_DEBUG_COLOR {0,0,1,1}
#define FINAL_RENDER_PASS_DEBUG_COLOR {1,1,1,1}

struct AtmosphereBuffer
{
    /*
    Tbx::Vector3f topLeftDir;
    float padding;
    Tbx::Vector3f topRightDir;
    float padding1;
    Tbx::Vector3f bottomLeftDir;
    float padding2;
    Tbx::Vector3f bottomRightDir;
    float padding3;*/

    Tbx::Matrix4x4f camToWorldMatrix;
    Tbx::Vector3f viewParam;
    float padding;
    Tbx::Vector3f cameraPos;
    float pad2;
    Tbx::Vector3f sunDir;
    float pad3;
    Tbx::Vector3f sunColor;
    float pad4;
    Tbx::Vector3f scatteringCoeff;
};

class RendererSystem : public EcsSystem
{

public:

    DEFAULT_COPY_MOVE_OPERATIONS(RendererSystem)
    
    Signature staticMeshSignature;

    Signature dirLightSignature;

    RendererSystem()
    {
        DYNAMIC_REFLECT_INIT;
    }

    IMP_DYNAMIC_REFLECT();

    PC_CORE_API void Begin() override {};
    PC_CORE_API void Tick(double deltaTime) override {};
    PC_CORE_API void RenderingTick(double deltatime) override {};
private:
    REFLECT(RendererSystem)
    REFLECT_MEMBER(RendererSystem, staticMeshSignature);
    REFLECT_MEMBER(RendererSystem, dirLightSignature);
};


class Renderer
{
public:

    std::shared_ptr<PC_CORE::CommandList> primaryCommandList;

    std::shared_ptr<PC_CORE::ShaderProgram> m_ForwardShader;

    std::shared_ptr<PC_CORE::ShaderProgram> m_DrawTextureScreenQuadShader;

    std::shared_ptr<PC_CORE::ShaderProgram> m_SkyRenderingShader;


    std::shared_ptr<RhiRenderPass> forwardPass;

    std::shared_ptr<RhiRenderPass> drawTextureScreenQuadPass;
    
    PC_CORE_API Renderer() = default;

    PC_CORE_API ~Renderer() = default;
    
    PC_CORE_API void Init();

    PC_CORE_API void Destroy();

    PC_CORE_API void BeginDraw(Window* _window);
    
    PC_CORE_API void DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext, World* _world);

    PC_CORE_API void SwapBuffers(Window* _window);

    PC_CORE_API void DrawTextureScreenQuad(const ShaderProgramDescriptorSets& _ShaderProgramDescriptorSets);

private:
    World* m_CurrentWorld;
    
    RhiContext* m_RhiContext;
    
    ShaderProgramDescriptorSets* m_ShaderProgramDescriptorSet = nullptr;

    ShaderProgramDescriptorSets* m_ShaderProgramDescriptorSetsSky = nullptr;

    UniformBuffer cameraUniformBuffer;

    SceneBufferGPU sceneBufferGPU;

    /*
    AtmosphereBuffer m_AtomsphereBuffer;

    UniformBuffer m_AtmosphereUniformBuffer;
*/
    std::unique_ptr<SceneLightsBuffer> sceneLightsBuffer;

    std::shared_ptr<RendererSystem> rendererSystem;

    const RenderingContext* currentRenderingContext = nullptr;
    
    PC_CORE_API void UpdateCameraUniformBuffer(const PC_CORE::RenderingContext& renderingContext);

    PC_CORE_API void UpdateViewExtremumBuffer(const PC_CORE::RenderingContext& renderingContext);

    PC_CORE_API void QueryWorldData(World* world);

    PC_CORE_API void QueryLightDirData(DirLight& dirLight, Transform& transform);
    
    PC_CORE_API void CreateForwardShader();

    PC_CORE_API void CreateDrawQuadShader();

    PC_CORE_API void CreateSkyRenderingShader();

    PC_CORE_API void DrawStaticMesh(PC_CORE::Transform& _transform, PC_CORE::StaticMesh& _staticMesh);

    PC_CORE_API void DrawSky();

    PC_CORE_API void InitRenderSystem();

    PC_CORE_API void CreateForwardRenderPass();
};

END_PCCORE