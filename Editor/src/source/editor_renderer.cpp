#include "editor_renderer.hpp"

#include "editor.hpp"
#include "resources/resource_manager.hpp"

PC_EDITOR_CORE::EditorRenderer::EditorRenderer(Editor& _editor) : m_Editor(&_editor)
{
    m_DirectionalLightTexture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>("directional_light_sprite", EDITOR_RESOURCE_PATH "/icons/dirlight_icon.png");
    InitResources();
   
}

void PC_EDITOR_CORE::EditorRenderer::PushCustomCommand()
{
    m_Editor->gameApp.renderer.UserCustomForwardPass.emplace_back([this]<typename T0, typename T1, typename T2, typename T3>(T0&& PH1, T1&& PH2, T2&& PH3, T3 && PH4)
       {
           DrawLightGizmo(std::forward<T0>(PH1), std::forward<T1>(PH2),
                          std::forward<T2>(PH3), std::forward<T3>(PH4));
       }
   );
}

void PC_EDITOR_CORE::EditorRenderer::DrawLightGizmo(PC_CORE::Renderer& _renderer ,PC_CORE::CommandList* _commandList,
                                                    const PC_CORE::RenderingContext& _renderingContext, const PC_CORE::RenderingWorldData* _renderingWorldData)
{
    PERF_FRAME_MARK;

    if (auto p = m_DrawSpriteShader.lock())
    {
        _commandList->BindProgram(p.get());
        _commandList->SetPrimitiveTopology(PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleStrip);
        _commandList->BindDescriptorSet(p.get(), m_CameraSet, SCENE_DESCRIPTOR_SET, 1);

        // Dir light
        _commandList->BindDescriptorSet(p.get(), m_DirectionalDescriptorSet, SPRITE_SET, 1);
        const Tbx::Vector3f cameraPos = _renderingContext.lowLevelCamera.position;
        for (size_t i = 0; i < _renderingWorldData->lightData.size(); i++)
        {
            if (_renderingWorldData->lightData[i].lightType != PC_CORE::LightType::Directional)
                continue;

            const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(-cameraPos, Tbx::Vector3f::Zero(), Tbx::Vector3f::UnitY()).Invert();

            
            _commandList->PushConstant(p.get(), "PushConstants", &invertView, sizeof(invertView));
            _commandList->Draw(4, 1, 0, 0);
        }            
    }
    
}

void PC_EDITOR_CORE::EditorRenderer::InitResources()
{

    const PC_CORE::RasterizerInfo rasterizerInfo =
        {
        .polygonMode = PC_CORE::PolygonMode::Fill,
        .cullModeFlag = PC_CORE::CullModeFlagBit::None,
        .frontFace = PC_CORE::FrontFace::CounterClockwise,
        .multiSampleRasterization = 1
        };

    const PC_CORE::DephStencilInfo dephStencilInfo =
        {
        .depthCompareOp = PC_CORE::CompareOp::LESS,
        .enableDepthTest = true
        };

    const PC_CORE::BlendInfo blendInfo = {
        .enabled = true,
        .srcColorBlendFactor = PC_CORE::BlendFactor::SrcAlpha,
        .dstColorBlendFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
        .colorBlendOp = PC_CORE::BlendOp::Add,

        .srcAlphaBlendFactor = PC_CORE::BlendFactor::One,
        .dstAlphaBlendFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
        .alphaBlendOp = PC_CORE::BlendOp::Add,

        .colorMask = static_cast<PC_CORE::ColorComponent>(PC_CORE::ColorComponent::ColorComponent_R |
                     PC_CORE::ColorComponent::ColorComponent_G |
                     PC_CORE::ColorComponent::ColorComponent_B |
                     PC_CORE::ColorComponent::ColorComponent_A)
    };
    PC_CORE::ShaderGraphicPointInfo shaderGraphicPointInfo =
        {
        .rasterizerInfo = rasterizerInfo,
        .dephInfo = dephStencilInfo,
        .blendInfo = blendInfo,
        .vertexInputBindingDescritions = {},
        .vertexAttributeDescriptions = {}
        };

    PC_CORE::SourceList sourceList =
        {
        {
            PC_CORE::ShaderStageType::VERTEX,
            PC_CORE::ResourceManager::Get<PC_CORE::ShaderSourceBinary>("draw_sprite_spv.vert"),
        },
        {
            PC_CORE::ShaderStageType::FRAGMENT,
                PC_CORE::ResourceManager::Get<PC_CORE::ShaderSourceBinary>("draw_sprite_spv.frag")
        }
        };
   
    PC_CORE::GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
        .shaderGraphicPointInfo = shaderGraphicPointInfo,
        .sourceList = sourceList,
        .renderPass = m_Editor->gameApp.renderer.renderPasses.forwardPass.get(),
        .colorAttachementCount = 1
        };
    
    
    m_DrawSpriteShader = PC_CORE::ResourceManager::Create<PC_CORE::GraphicShader>("sprite_shader", graphicShaderProgramCreateInfo);

    auto lockedShader = m_DrawSpriteShader.lock();
    if (lockedShader == nullptr)
        return;
    
    lockedShader->AllocDescriptorSet(&m_CameraSet, SCENE_DESCRIPTOR_SET);
    lockedShader->AllocDescriptorSet(&m_DirectionalDescriptorSet, SPRITE_SET);

    PC_CORE::UniformBufferDescriptor cameraBufferDescritptor
  {
      .buffer = &m_Editor->gameApp.renderer.cameraUniformBuffer,
  };

    PC_CORE::ImageSamperDescriptor directionalTexture
    {
        .sampler = PC_CORE::ResourceManager::Get<PC_CORE::Sampler>("LinearRepeat").get(),
        .texture = m_DirectionalLightTexture.lock().get()
    };

    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
        {
        {
            PC_CORE::ShaderProgramDescriptorType::UniformBuffer,
            CAMERA_BINDING,
            &cameraBufferDescritptor,
            nullptr,
        }
        };
        
    m_CameraSet->WriteDescriptorSets(descriptorSets);

    descriptorSets =
        {
        {
            PC_CORE::ShaderProgramDescriptorType::CombineImageSampler,
            SPRITE_TEXTURE,
            nullptr,
            &directionalTexture,
        }
    };
    m_DirectionalDescriptorSet->WriteDescriptorSets(descriptorSets);
}
