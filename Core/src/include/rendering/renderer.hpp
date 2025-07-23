#pragma once
#include "light.hpp"
#include "rendering_typedef.h"
#include "debug_helper/debug_draw_context.hpp"
#include "low_renderer/rhi_vertex_buffer.hpp"
#include "low_renderer/command_list.hpp"
#include "low_renderer/rhi_index_buffer.hpp"
#include "low_renderer/rhi_context.hpp"
#include "low_renderer/rhi_shader_program.hpp"
#include "low_renderer/rhi_uniform_buffer.hpp"
#include "resources/graphic_shader.hpp"
#include "resources/texture_3d.hpp"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"

BEGIN_PCCORE

// TODO

#define PREPASS_COLOR {1,0.2,1,1}

#define FORWARD_DEBUG_COLOR {0,0,1,1}
#define GEOMETRY_PASS_COLOR {0.1,0.7,0.2,1}
#define DEFERD_PASS_COLOR {1,0.2,1,1}
#define FINAL_RENDER_PASS_DEBUG_COLOR {1,1,1,1}

struct RenderPasses
{
    std::shared_ptr<RhiRenderPass> defferedPass;
    std::shared_ptr<RhiRenderPass> forwardPass;
    std::shared_ptr<RhiRenderPass> drawToFinalViewPort;
};

class Renderer
{
public:
    
    std::shared_ptr<PC_CORE::CommandList> primaryCommandList;

    std::weak_ptr<PC_CORE::GraphicShader> m_ForwardShader;

    // TO DO TO RESOURE REF
    std::weak_ptr<PC_CORE::GraphicShader> m_DrawTextureScreenQuadShader;

    std::weak_ptr<PC_CORE::GraphicShader> m_CubeMapShader;

    std::weak_ptr<PC_CORE::GraphicShader> m_GeometryBufferShader;

    std::weak_ptr<PC_CORE::GraphicShader> m_DeferedShader;

    
    RenderPasses renderPasses;

    UniformBuffer cameraUniformBuffer;

    SceneBufferGPU sceneBufferGPU;

    PC_CORE_API void GetRenderingData(const RenderingWorldData& _newRenderingData);

#ifdef WITH_EDITOR
    std::vector<std::function<void(Renderer& ,CommandList*, const RenderingContext&, const RenderingWorldData*)>> UserCustomForwardPass;
#endif
    
    PC_CORE_API Renderer() = default;

    PC_CORE_API ~Renderer() = default;
    
    PC_CORE_API void Init();
    
    PC_CORE_API void BeginDraw(Window* _window);
    
    PC_CORE_API void DrawToRenderingContext(const PC_CORE::RenderingContext& renderingContext);

    PC_CORE_API void SwapBuffers(Window* _window);

    PC_CORE_API void DrawTextureScreenQuad(const ShaderProgramDescriptorSets& _ShaderProgramDescriptorSets);

private:    
    RhiContext* m_RhiContext;

    std::shared_ptr<RhiFence> m_PendingResourceFence;

   
    // Critical section
    RenderingWorldData m_RenderWorldData;
    
    ShaderProgramDescriptorSets* m_ShaderProgramSceneDescriptorSet = nullptr;

    struct DescriptorSetsSkybox
    {
        ShaderProgramDescriptorSets* cameraDescriptorSet = nullptr;
        ShaderProgramDescriptorSets* cubeMapDescriptorSet = nullptr;
    }descriptorSetsSkybox;

    ShaderProgramDescriptorSets* m_GeometryBufferDescriptorSet = nullptr;

    ShaderProgramDescriptorSets* m_DeferdDescriptorSet = nullptr;


    std::weak_ptr<Texture3D> m_Cubemap;

    std::weak_ptr<Mesh> m_CubeMesh;

    std::unique_ptr<GPUDynamicLightData> gpuDynamicLightData;

    UniformBuffer gpuLightUniformBufferStaging;
    UniformBuffer gpuLightUniformBuffer;

    const RenderingContext* currentRenderingContext = nullptr;
    
#ifdef WITH_EDITOR
   // std::unique_ptr<DebugDrawContext> m_DebugDrawContext;
#endif
    
    PC_CORE_API void CreateRenderPasss();

    PC_CORE_API void CreateShaders();

    PC_CORE_API void CreateDescriptorSets();

    PC_CORE_API void CreateThirdPartyResources();
    
    PC_CORE_API void UpdateCameraUniformBuffer(const PC_CORE::RenderingContext& renderingContext);

    PC_CORE_API void UpdateLightData(const RenderingContext& _context, CommandList* commandList);
    
    PC_CORE_API void DrawStaticMesh(MaterialType type, std::shared_ptr<PC_CORE::GraphicShader> shader);

    PC_CORE_API void ClearRenderData();

    PC_CORE_API void DrawSkyBox();

    PC_CORE_API void ForwardPass(const PC_CORE::RenderingContext& _renderingContext, const ViewportInfo& _viewportInfo);

    PC_CORE_API void DefferdPass(const PC_CORE::RenderingContext& _renderingContext, const ViewportInfo& _viewportInfo);

    PC_CORE_API void FinalPass(const PC_CORE::RenderingContext& _renderingContext, const ViewportInfo& _viewportInfo);
};

END_PCCORE