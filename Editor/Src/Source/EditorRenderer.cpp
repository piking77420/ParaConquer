#include "EditorRenderer.hpp"

#include "Editor.hpp"
#include "Resources/ResourceManager.hpp"
#include "DebugHelper/DebugDrawContext.hpp"

PC_EDITOR_CORE::EditorRenderer::EditorRenderer(Editor& _editor) : m_Editor(&_editor)
{
    m_DirectionalLightTexture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>("DirectionalLightSprite", EDITOR_RESOURCE_PATH "/Icons/DirlightIcon.png");
    m_SpotLightTexture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>("SpotLightSprite", EDITOR_RESOURCE_PATH "/Icons/SpotLight.png");
    m_PointLightTexture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>("PointLightSprite", EDITOR_RESOURCE_PATH "/Icons/PointLight.png");

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

void PC_EDITOR_CORE::EditorRenderer::DrawSelectedEntity()
{
    if (!std::holds_alternative<PC_CORE::EntityId>(m_Editor->selectedObject))
    {
        return;
    }

    PC_CORE::EntityId selectedEntity = std::get<PC_CORE::EntityId>(m_Editor->selectedObject);

    if (m_Editor->gameApp.world.level.HasComponent<PC_CORE::Transform>(selectedEntity) && m_Editor->gameApp.world.level.HasComponent<PC_CORE::PointLight>(selectedEntity))
    {
        PC_CORE::Transform& t = m_Editor->gameApp.world.level.GetComponent<PC_CORE::Transform>(selectedEntity);
        PC_CORE::PointLight& p = m_Editor->gameApp.world.level.GetComponent<PC_CORE::PointLight>(selectedEntity);

        PC_CORE::DebugDrawContext::DrawWireSphere(t.position, std::sqrt(p.intensity), Tbx::Vector3f(0, 1, 0));
    }

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
             _commandList->PushConstant(p.get(), "pushConstants", &invertView, sizeof(invertView));
             _commandList->Draw(4, 1, 0, 0);
        }

        if (!m_SpotLightIndices.empty())
            _commandList->BindDescriptorSet(p.get(), m_SpotLightDescriptorSet, SPRITE_SET, 1);
        for (size_t i = 0; i < m_SpotLightIndices.size(); i++)
        {
            auto& spothlight = _renderingWorldData->lightData.at(m_SpotLightIndices[i]).data.spotLight;
            Tbx::Vector3f pointPos = static_cast<Tbx::Vector3f>(spothlight.position - _renderingContext.lowLevelCamera.position);
            
            const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(pointPos, Tbx::Vector3f::Zero(), Tbx::Vector3f::UnitY()).Invert();
            _commandList->PushConstant(p.get(), "pushConstants", &invertView, sizeof(invertView));
            _commandList->Draw(4, 1, 0, 0);
        }

        if (!m_PointLightIndices.empty())
            _commandList->BindDescriptorSet(p.get(), m_PointLightDescriptorSet, SPRITE_SET, 1);
        for (size_t i = 0; i < m_PointLightIndices.size(); i++)
        {
            auto& pointLight = _renderingWorldData->lightData.at(m_PointLightIndices[i]).data.pointLightData;
            Tbx::Vector3f pointPos = static_cast<Tbx::Vector3f>(pointLight.position - _renderingContext.lowLevelCamera.position);
            
            const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(pointPos, Tbx::Vector3f::Zero(), Tbx::Vector3f::UnitY()).Invert();

            _commandList->PushConstant(p.get(), "pushConstants", &invertView, sizeof(invertView));
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
            PC_CORE::ResourceManager::Get<PC_CORE::ShaderSourceBinary>("DrawSprite.vs.hlsl.binary"),
        },
        {
            PC_CORE::ShaderStageTypeFlag::Pixel,
                PC_CORE::ResourceManager::Get<PC_CORE::ShaderSourceBinary>("DrawSprite.ps.hlsl.binary")
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
    
    
    m_DrawSpriteShader = PC_CORE::ResourceManager::Create<PC_CORE::GraphicShader>("SpriteShader", graphicShaderProgramCreateInfo);

    auto lockedShader = m_DrawSpriteShader.lock();
    if (lockedShader == nullptr)
        return;
    
    lockedShader->AllocDescriptorSet(&m_CameraSet, SCENE_DESCRIPTOR_SET);
    lockedShader->AllocDescriptorSet(&m_DirectionalDescriptorSet, SPRITE_SET);
    lockedShader->AllocDescriptorSet(&m_SpotLightDescriptorSet, SPRITE_SET);
    lockedShader->AllocDescriptorSet(&m_PointLightDescriptorSet, SPRITE_SET);

    PC_CORE::UniformBufferDescriptor cameraBufferDescritptor
  {
      .buffer = &m_Editor->gameApp.renderer.uniformBuffers.cameraUniformBuffer,
  };

    PC_CORE::ImageSamplerDescriptor directionalTexture
    {
        .sampler = PC_CORE::ResourceManager::Get<PC_CORE::Sampler>("LinearRepeat").get(),
        .texture = m_DirectionalLightTexture.lock().get(),
        .imageState = PC_CORE::ImageState::ShaderReadOptimal
    };

    PC_CORE::ImageSamplerDescriptor spothLightTexture
    {
        .sampler = directionalTexture.sampler,
        .texture = m_SpotLightTexture.lock().get(),
        .imageState = PC_CORE::ImageState::ShaderReadOptimal
    };

    PC_CORE::ImageSamplerDescriptor pointLightTexture
    {
        .sampler = directionalTexture.sampler,
        .texture = m_PointLightTexture.lock().get(),
        .imageState = PC_CORE::ImageState::ShaderReadOptimal
    };

    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
        {
            {
                PC_CORE::ShaderProgramDescriptorType::UniformBuffer,
                CAMERA_BINDING,
                cameraBufferDescritptor,
            }
        };
        
    m_CameraSet->WriteDescriptorSets(descriptorSets);

    descriptorSets =
        {
        {
            PC_CORE::ShaderProgramDescriptorType::CombinedImageSampler,
            SPRITE_TEXTURE,
            directionalTexture,
        }
    };
    m_DirectionalDescriptorSet->WriteDescriptorSets(descriptorSets);

    descriptorSets[0].descriptor = spothLightTexture;
    m_SpotLightDescriptorSet->WriteDescriptorSets(descriptorSets);

    descriptorSets[0].descriptor = pointLightTexture;
    m_PointLightDescriptorSet->WriteDescriptorSets(descriptorSets);
    
}
