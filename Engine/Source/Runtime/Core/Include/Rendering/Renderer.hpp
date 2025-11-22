#pragma once
#include "Light.hpp"
#include "RenderingTypedef.h"
#include "DebugHelper/DebugDrawContext.hpp"
#include "LowRenderer/CommandList.hpp"
#include "LowRenderer/RhiContext.hpp"
#include "LowRenderer/RhiShaderProgram.hpp"
#include "Resources/GraphicShader.hpp"
#include "Resources/Texture3d.hpp"
#include "World/StaticMeshComponent.hpp"
#include "World/Transform.hpp"
#include "World/World.hpp"
#include "Resources/ComputeShader.hpp"
#include "View.hpp"
#include "Sampler.hpp"

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
        std::shared_ptr<RhiRenderPass> DefferedPass;
        std::shared_ptr<RhiRenderPass> ForwardPass;
        std::shared_ptr<RhiRenderPass> DrawToFinalViewPort;
    };

    struct UniformBuffers
    {
        UniformBuffer CameraUniformBuffer;
        UniformBuffer PostProcessUniformBuffer;
        UniformBuffer LightBuffer;
    };


    class Renderer
    {
    public:
        UniformBuffers UniformBuffers;

        // Critical section
        RenderingWorldData RenderWorldData;

        std::unique_ptr<CommandList> PrimaryCommandList;

        std::unique_ptr<CommandList> SwapChainPassCommandList;

        std::unique_ptr<RhiShaderProgram> ForwardShader;

        std::unique_ptr<RhiShaderProgram> DrawTextureScreenQuadShader;

        std::unique_ptr<RhiShaderProgram> SkyBoxShader;

        std::unique_ptr<RhiShaderProgram> GeometryBufferShader;

        std::unique_ptr<RhiShaderProgram> DeferedShader;

        std::unique_ptr<RhiShaderProgram> AcesShader;

        RenderPasses RenderPasses;

        Sampler LinearReapeat;

        WeakObjectPtr<Texture3D> Cubemap;

        PC_CORE_API void GetRenderingData(const RenderingWorldData& _newRenderingData);

#ifdef WITH_EDITOR
        std::vector<std::function<void(Renderer&, CommandList*, const RenderingContext&, const RenderingWorldData*)>>
        UserCustomForwardPass;
#endif

        PC_CORE_API Renderer() = default;

        PC_CORE_API ~Renderer();

        PC_CORE_API void Init(Rhi& _Rhi);

        PC_CORE_API void BeginFrame(Window* _window);

        PC_CORE_API void Draw(const View& _view);

        PC_CORE_API void SwapBuffers(Window* _window);

        PC_CORE_API std::shared_ptr<View> CreateView(Tbx::Vector2i _defaultSize);

        PC_CORE_API Rhi& GetRhi(); 

    private:
        Rhi* m_Rhi{nullptr};

        const View* m_CurrentView = nullptr;

        ShaderProgramDescriptorSets* m_SkyboxCameraDescriptorSet;

        ShaderProgramDescriptorSets* m_SkyBoxCubeMapDescriptorSet;

        VertexBuffer m_CubeVertexBuffer;

        Sampler m_SkyBoxSampler;

#ifdef WITH_EDITOR
        std::unique_ptr<DebugDrawContext> m_DebugDrawContext;
#endif

        std::vector<std::shared_ptr<View>> m_Views;

        GPUDynamicLightData m_GpuDynamicLightData;

        void InitCubeBuffers();

        PC_CORE_API void CreateBuffers();

        PC_CORE_API void CreateRenderPasss();

        PC_CORE_API void CreateShaders();

        PC_CORE_API void CreateDescriptorSets();

        PC_CORE_API void CreateThirdPartyResources();

        PC_CORE_API void DrawStaticMesh(MaterialType _type, RhiShaderProgram& _shader);

        PC_CORE_API void ClearRenderData();

        PC_CORE_API void UpdateLightGpuData(CommandList* _commandlist);

        PC_CORE_API void UpdateGpuCameraData();

        PC_CORE_API void ForwardPass(const ViewportInfo& _viewportInfo);

        PC_CORE_API void DefferdPass(const ViewportInfo& _viewportInfo);

        PC_CORE_API void PostProcess(const ViewportInfo& _viewportInfo);

        PC_CORE_API void FinalPass(const ViewportInfo& _viewportInfo);
    };


END_PCCORE
