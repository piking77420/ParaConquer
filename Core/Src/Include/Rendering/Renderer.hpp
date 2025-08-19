#pragma once
#include "Light.hpp"
#include "RenderingTypedef.h"
#include "DebugHelper/DebugDrawContext.hpp"
#include "LowRenderer/RhiVertexBuffer.hpp"
#include "LowRenderer/CommandList.hpp"
#include "LowRenderer/RhiIndexBuffer.hpp"
#include "LowRenderer/RhiContext.hpp"
#include "LowRenderer/RhiShaderProgram.hpp"
#include "LowRenderer/RhiUniformBuffer.hpp"
#include "Resources/GraphicShader.hpp"
#include "Resources/Texture3d.hpp"
#include "World/StaticMesh.hpp"
#include "World/Transform.hpp"
#include "World/World.hpp"
#include "Resources/ComputeShader.hpp"
#include "View.hpp"

BEGIN_PCCORE
    // TODO
#define PREPASS_COLOR {1,0.2,1,1}

#define FORWARD_DEBUG_COLOR {0,0,1,1}
#define GEOMETRY_PASS_COLOR {0.1,0.7,0.2,1}
#define DEFERD_PASS_COLOR {1,0.2,1,1}
#define POST_PROCESS {1,0,1,1}
#define FINAL_RENDER_PASS_DEBUG_COLOR {1,1,1,1}

struct RenderPasses
{
    std::shared_ptr<RhiRenderPass> defferedPass;
    std::shared_ptr<RhiRenderPass> forwardPass;
    std::shared_ptr<RhiRenderPass> drawToFinalViewPort;
};

struct UniformBuffers
{
    UniformBuffer cameraUniformBuffer;
    UniformBuffer postProcessUniformBuffer;
    UniformBuffer dynamicGpuLightUniformBuffer;
};



class Renderer
{
public:
   
    UniformBuffers uniformBuffers;

    // Critical section
    RenderingWorldData renderWorldData;
    
    std::shared_ptr<PC_CORE::CommandList> primaryCommandList;

    std::shared_ptr<PC_CORE::CommandList> swapChainPassCommandList;

    ResourceRef<PC_CORE::GraphicShader> m_ForwardShader;

    ResourceRef<PC_CORE::GraphicShader> m_DrawTextureScreenQuadShader;

    ResourceRef<PC_CORE::GraphicShader> m_SkyBoxShader;

    ResourceRef<PC_CORE::GraphicShader> m_GeometryBufferShader;

    ResourceRef<PC_CORE::GraphicShader> m_DeferedShader;

    ResourceRef<PC_CORE::ComputeShader> m_AcesShader;

    RenderPasses renderPasses;

    std::weak_ptr<Texture3D> m_Cubemap;

    PC_CORE_API void GetRenderingData(const RenderingWorldData& _newRenderingData);

#ifdef WITH_EDITOR
    std::vector<std::function<void(Renderer& ,CommandList*, const RenderingContext&, const RenderingWorldData*)>> UserCustomForwardPass;
#endif
    
    PC_CORE_API Renderer() = default;

    PC_CORE_API ~Renderer();
    
    PC_CORE_API void Init();
    
    PC_CORE_API void BeginFrame(Window* _window);
    
    PC_CORE_API void Draw(const View& _view);

    PC_CORE_API void SwapBuffers(Window* _window);

    PC_CORE_API std::shared_ptr<View> CreateView(Tbx::Vector2i _defaultSize);

private:    
    RhiContext* m_RhiContext;

    const View* m_CurrentView = nullptr;
  
    ShaderProgramDescriptorSets* skyboxCameraDescriptorSet;

    ShaderProgramDescriptorSets* skyBoxCubeMapDescriptorSet;

    std::weak_ptr<Mesh> m_CubeMesh;

#ifdef WITH_EDITOR
   std::unique_ptr<DebugDrawContext> m_DebugDrawContext;
#endif

    std::vector<std::shared_ptr<View>> m_Views;

    UniformBuffer gpuLightUniformBufferStaging;

    GPUDynamicLightData m_GpuDynamicLightData;

    PC_CORE_API void CreateBuffers();
    
    PC_CORE_API void CreateRenderPasss();

    PC_CORE_API void CreateShaders();

    PC_CORE_API void CreateDescriptorSets();

    PC_CORE_API void CreateThirdPartyResources();
        
    PC_CORE_API void DrawStaticMesh(MaterialType type, std::shared_ptr<PC_CORE::GraphicShader> shader);

    PC_CORE_API void ClearRenderData();

    PC_CORE_API void UpdateLightGPUData(CommandList* commandlist);

    PC_CORE_API void UpdateGpuCameraData();

    PC_CORE_API void ForwardPass(const ViewportInfo& _viewportInfo);

    PC_CORE_API void DefferdPass(const ViewportInfo& _viewportInfo);

    PC_CORE_API void PostProcess(const ViewportInfo& _viewportInfo);

    PC_CORE_API void FinalPass(const ViewportInfo& _viewportInfo);
};

    

END_PCCORE