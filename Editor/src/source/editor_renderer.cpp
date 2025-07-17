#include "editor_renderer.hpp"

#include "editor.hpp"
#include "resources/resource_manager.hpp"

PC_EDITOR_CORE::EditorRenderer::EditorRenderer(Editor& _editor) : m_Editor(&_editor)
{
    m_DirectionalLightTexture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>("directional_light_sprite", EDITOR_RESOURCE_PATH "/icons/dirlight_icon.png");
    m_SpotLightTexture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>("spot_light_sprite", EDITOR_RESOURCE_PATH "/icons/spot_light.png");
    m_PointLightTexture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>("point_light_sprite", EDITOR_RESOURCE_PATH "/icons/point_light.png");

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
 
    m_DirectionalLightIndices.clear();
    m_SpotLightIndices.clear();
    m_PointLightIndices.clear();

    if (auto p = m_DrawSpriteShader.lock())
    {
        _commandList->BindProgram(p.get());
        _commandList->SetPrimitiveTopology(PC_CORE::PrimitiveTopology::PrimitiveTopologyTriangleStrip);
        _commandList->BindDescriptorSet(p.get(), m_CameraSet, SCENE_DESCRIPTOR_SET, 1);

        // Dir light
        const Tbx::Vector3f cameraPos = static_cast<Tbx::Vector3f>(_renderingContext.lowLevelCamera.position);
        for (size_t i = 0; i < _renderingWorldData->lightData.size(); i++)
        {
            switch (_renderingWorldData->lightData[i].lightType)
            {
            case PC_CORE::LightType::Directional:
                m_DirectionalLightIndices.push_back(i);
                break;
            case PC_CORE::LightType::Spotlight:
                m_SpotLightIndices.push_back(i);
                break;
            case PC_CORE::LightType::Point:
                m_PointLightIndices.push_back(i);
                break;
            case PC_CORE::LightType::Area:
            case PC_CORE::LightType::Count:
            default:
                assert(false);
            }
        }

        if (!m_DirectionalLightIndices.empty())
            _commandList->BindDescriptorSet(p.get(), m_DirectionalDescriptorSet, SPRITE_SET, 1);
        for (size_t i = 0; i < m_DirectionalLightIndices.size(); i++)
        {
             const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(-static_cast<Tbx::Vector3<float>>(_renderingContext.lowLevelCamera.position), Tbx::Vector3f::Zero(), Tbx::Vector3f::UnitY()).Invert();
             _commandList->PushConstant(p.get(), "PushConstants", &invertView, sizeof(invertView));
             _commandList->Draw(4, 1, 0, 0);
        }

        if (!m_SpotLightIndices.empty())
            _commandList->BindDescriptorSet(p.get(), m_SpotLightDescriptorSet, SPRITE_SET, 1);
        for (size_t i = 0; i < m_SpotLightIndices.size(); i++)
        {
            auto& spothlight = _renderingWorldData->lightData.at(m_SpotLightIndices[i]).data.spotLight;
            Tbx::Vector3f pointPos = static_cast<Tbx::Vector3f>(spothlight.position - _renderingContext.lowLevelCamera.position);
            
            const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(pointPos, Tbx::Vector3f::Zero(), Tbx::Vector3f::UnitY()).Invert();
            _commandList->PushConstant(p.get(), "PushConstants", &invertView, sizeof(invertView));
            _commandList->Draw(4, 1, 0, 0);
        }

        if (!m_PointLightIndices.empty())
            _commandList->BindDescriptorSet(p.get(), m_PointLightDescriptorSet, SPRITE_SET, 1);
        for (size_t i = 0; i < m_PointLightIndices.size(); i++)
        {
            auto& pointLight = _renderingWorldData->lightData.at(m_PointLightIndices[i]).data.pointLightData;
            Tbx::Vector3f pointPos = static_cast<Tbx::Vector3f>(pointLight.position - _renderingContext.lowLevelCamera.position);
            
            const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(pointPos, Tbx::Vector3f::Zero(), Tbx::Vector3f::UnitY()).Invert();
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
            PC_CORE::ShaderStageTypeFlag::Vertex,
            PC_CORE::ResourceManager::Get<PC_CORE::ShaderSourceBinary>("draw_sprite_spv.vert"),
        },
        {
            PC_CORE::ShaderStageTypeFlag::Fragment,
                PC_CORE::ResourceManager::Get<PC_CORE::ShaderSourceBinary>("draw_sprite_spv.frag")
        }
        };
   
    PC_CORE::GraphicShaderProgramCreateInfo graphicShaderProgramCreateInfo =
        {
        .shaderGraphicPointInfo = shaderGraphicPointInfo,
        .sourceList = sourceList,
        .renderPass = m_Editor->gameApp.renderer.renderPasses.forwardPass.get(),
        .colorAttachementCount = 1,
        .subPassIndex = 0
        };
    
    
    m_DrawSpriteShader = PC_CORE::ResourceManager::Create<PC_CORE::GraphicShader>("sprite_shader", graphicShaderProgramCreateInfo);

    auto lockedShader = m_DrawSpriteShader.lock();
    if (lockedShader == nullptr)
        return;
    
    lockedShader->AllocDescriptorSet(&m_CameraSet, SCENE_DESCRIPTOR_SET);
    lockedShader->AllocDescriptorSet(&m_DirectionalDescriptorSet, SPRITE_SET);
    lockedShader->AllocDescriptorSet(&m_SpotLightDescriptorSet, SPRITE_SET);
    lockedShader->AllocDescriptorSet(&m_PointLightDescriptorSet, SPRITE_SET);

    PC_CORE::UniformBufferDescriptor cameraBufferDescritptor
  {
      .buffer = &m_Editor->gameApp.renderer.cameraUniformBuffer,
  };

    PC_CORE::ImageSamperDescriptor directionalTexture
    {
        .sampler = PC_CORE::ResourceManager::Get<PC_CORE::Sampler>("LinearRepeat").get(),
        .texture = m_DirectionalLightTexture.lock().get()
    };

    PC_CORE::ImageSamperDescriptor spothLightTexture
    {
        .sampler = directionalTexture.sampler,
        .texture = m_SpotLightTexture.lock().get()
    };

    PC_CORE::ImageSamperDescriptor pointLightTexture
    {
        .sampler = directionalTexture.sampler,
        .texture = m_PointLightTexture.lock().get()
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
            PC_CORE::ShaderProgramDescriptorType::CombinedImageSampler,
            SPRITE_TEXTURE,
            nullptr,
            &directionalTexture,
        }
    };
    m_DirectionalDescriptorSet->WriteDescriptorSets(descriptorSets);

    descriptorSets[0].imageSamperDescriptor = &spothLightTexture;
    m_SpotLightDescriptorSet->WriteDescriptorSets(descriptorSets);

    descriptorSets[0].imageSamperDescriptor = &pointLightTexture;
    m_PointLightDescriptorSet->WriteDescriptorSets(descriptorSets);
    
}
