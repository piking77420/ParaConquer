#include "EditorRenderer.hpp"

#include "Editor.hpp"
#include "Resources/ResourceManager.hpp"
#include "DebugHelper/DebugDrawContext.hpp"
#include "Resources/FileLoader.hpp"
#include "LowRenderer/RhiShaderProgram.hpp"

PC_EDITOR_CORE::EditorRenderer::EditorRenderer(Editor& _editor) : m_Editor(&_editor)
{
 

    /*
    Image imageDirlightIcon(EDITOR_RESOURCE_PATH "/Icons/DirlightIcon.png", RhiChannel::Rgba);
    Image imageSpotLight(EDITOR_RESOURCE_PATH "/Icons/SpotLight.png", RhiChannel::Rgba);
    Image imagePointLight(EDITOR_RESOURCE_PATH "/Icons/PointLight.png", RhiChannel::Rgba);

    auto createTextureFromImage = [&](const std::string& name, std::weak_ptr<PC_CORE::Texture2D>& texture, const Image& image)
        {
            const RhiTexture::RhiTextureDesciptor desc =
            {
            .Width = image.GetWidht(),
            .Height = image.GetHeight(),
            .Depth = 1,
            .Level = static_cast<uint32_t>(std::floor(std::log2(std::max(image.GetWidht(), image.GetHeight())))) + 1,
            .LayerCount = 1,
            .Samples = 1,
            .TextureType = RhiTexture::Type::Texture2D,
            .TextureUsage = static_cast<RhiTexture::TextureUsageFlagBits>(RhiTexture::TextureUsageFlagBits::Sampled | RhiTexture::TextureUsageFlagBits::TransferDst | RhiTexture::TextureUsageFlagBits::TransferSrc),
            .RhiFormat = RhiFormat::R8G8B8A8Unorm,
            .AllowCpuAcces = false
            };

            texture = PC_CORE::ResourceManager::Create<PC_CORE::Texture2D>(m_Editor->gameApp.RenderHarwareInteface, name, desc, RhiResource::MemoryUsage::Static);

            texture.lock()->Get()->Build();
            
            _editor.gameApp.RenderHarwareInteface.PushResourceUpdate([&](CommandList* list)
                {
                    texture.lock()->Get()->UploadData2D(list, image.GetData(), image.GetWidht(), image.GetHeight(), image.GetChannel());
                    texture.lock()->Get()->GenerateMipMap(list);
                });
        };

    createTextureFromImage("DirectionalLightSprite", m_DirectionalLightTexture, imageDirlightIcon);
    createTextureFromImage("SpotLightSprite", m_SpotLightTexture, imageSpotLight);
    createTextureFromImage("PointLightSprite", m_PointLightTexture, imagePointLight);


    InitResources();*/
}

void PC_EDITOR_CORE::EditorRenderer::PushCustomCommand()
{
    /*
    m_Editor->gameApp.Renderer.UserCustomForwardPass.emplace_back(
        [this]<typename T0, typename T1, typename T2, typename T3>(T0&& PH1, T1&& PH2, T2&& PH3, T3&& PH4)
        {
            DrawLightGizmo(std::forward<T0>(PH1), std::forward<T1>(PH2),
                           std::forward<T2>(PH3), std::forward<T3>(PH4));
        }
    );*/
}

void PC_EDITOR_CORE::EditorRenderer::DrawSelectedEntity()
{
    if (!std::holds_alternative<PC_CORE::EntityId>(m_Editor->selectedObject))
    {
        return;
    }

    PC_CORE::EntityId selectedEntity = std::get<PC_CORE::EntityId>(m_Editor->selectedObject);

    if (m_Editor->gameApp.World.level.HasComponent<PC_CORE::Transform>(selectedEntity) && m_Editor->gameApp.World.level.
        HasComponent<PC_CORE::PointLight>(selectedEntity))
    {
        PC_CORE::Transform& t = m_Editor->gameApp.World.level.GetComponent<PC_CORE::Transform>(selectedEntity);
        PC_CORE::PointLight& p = m_Editor->gameApp.World.level.GetComponent<PC_CORE::PointLight>(selectedEntity);

        PC_CORE::DebugDrawContext::DrawWireSphere(t.Position, std::sqrt(p.intensity), Tbx::Vector3f(0, 1, 0));
    }
}

void PC_EDITOR_CORE::EditorRenderer::DrawLightGizmo(PC_CORE::Renderer& _renderer, PC_CORE::CommandList* _commandList,
                                                    const PC_CORE::RenderingContext& _renderingContext,
                                                    const PC_CORE::RenderingWorldData* _renderingWorldData)
{
    /*
    PERF_REGION_SCOPED;

    m_DirectionalLightIndices.clear();
    m_SpotLightIndices.clear();
    m_PointLightIndices.clear();

    _commandList->BeginDebugLabel("DrawLightGizmo", {1.f,1.f,1.f,1.f});

    {
        _commandList->BindProgram(*m_DrawSpriteShader);
        _commandList->SetPrimitiveTopology(PC_CORE::RhiShaderProgram::PrimitiveTopology::PrimitiveTopologyTriangleStrip);
        _commandList->BindDescriptorSet(*m_DrawSpriteShader, m_CameraSet.get(), SCENE_DESCRIPTOR_SET, 1);

        // Dir light
        const auto cameraPos = static_cast<Tbx::Vector3f>(_renderingContext.LowLevelCamera.Position);
        for (size_t i = 0; i < _renderingWorldData->LightData.size(); i++)
        {
            switch (_renderingWorldData->LightData[i].LightType)
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
            _commandList->BindDescriptorSet(*m_DrawSpriteShader, m_DirectionalDescriptorSet.get(), SPRITE_SET, 1);
        for (size_t i = 0; i < m_DirectionalLightIndices.size(); i++)
        {
            const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(
                -static_cast<Tbx::Vector3<float>>(_renderingContext.LowLevelCamera.Position), Tbx::Vector3f::Zero(),
                Tbx::Vector3f::UnitY()).Invert();
            _commandList->PushConstant(*m_DrawSpriteShader, "pushConstants", &invertView, sizeof(invertView));
            _commandList->Draw(4, 1, 0, 0);
        }

        if (!m_SpotLightIndices.empty())
            _commandList->BindDescriptorSet(*m_DrawSpriteShader, m_SpotLightDescriptorSet.get(), SPRITE_SET, 1);
        for (size_t i = 0; i < m_SpotLightIndices.size(); i++)
        {
            auto& spothlight = _renderingWorldData->LightData.at(m_SpotLightIndices[i]).Data.SpotLight;
            auto pointPos = static_cast<Tbx::Vector3f>(spothlight.Position - _renderingContext.LowLevelCamera.Position);

            const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(pointPos, Tbx::Vector3f::Zero(), Tbx::Vector3f::UnitY()).
                Invert();
            _commandList->PushConstant(*m_DrawSpriteShader, "pushConstants", &invertView, sizeof(invertView));
            _commandList->Draw(4, 1, 0, 0);
        }

        if (!m_PointLightIndices.empty())
            _commandList->BindDescriptorSet(*m_DrawSpriteShader, m_PointLightDescriptorSet.get(), SPRITE_SET, 1);
        for (size_t i = 0; i < m_PointLightIndices.size(); i++)
        {
            auto& pointLight = _renderingWorldData->LightData.at(m_PointLightIndices[i]).Data.PointLightData;
            auto pointPos = static_cast<Tbx::Vector3f>(pointLight.Position - _renderingContext.LowLevelCamera.Position);

            const Tbx::Matrix4x4f invertView = Tbx::LookAtRH(pointPos, Tbx::Vector3f::Zero(), Tbx::Vector3f::UnitY()).
                Invert();

            _commandList->PushConstant(*m_DrawSpriteShader, "pushConstants", &invertView, sizeof(invertView));
            _commandList->Draw(4, 1, 0, 0);
        }
    }

    _commandList->EndDebugLabel();*/
}

void PC_EDITOR_CORE::EditorRenderer::InitResources()
{
    /*
    const PC_CORE::SamplerCreateInfo info =
    {
        .magFilter = PC_CORE::Filter::Linear,
        .minFilter = PC_CORE::Filter::Linear,
        .u = PC_CORE::SamplerAddressMode::Repeat,
        .v = PC_CORE::SamplerAddressMode::Repeat,
        .w = PC_CORE::SamplerAddressMode::Repeat
    };

    m_SpriteSampler = PC_CORE::Sampler(m_Editor->gameApp.RenderHarwareInteface, "SpriteSampler", info);
    m_SpriteSampler->Build();

    constexpr PC_CORE::RhiShaderProgram::RasterizerInfo rasterizerInfo =
    {
        .polygonMode = PC_CORE::RhiShaderProgram::PolygonMode::Fill,
        .cullModeFlag = PC_CORE::RhiShaderProgram::CullModeFlagBit::None,
        .frontFace = PC_CORE::RhiShaderProgram::FrontFace::CounterClockwise,
        .multiSampleRasterization = 1
    };

    constexpr PC_CORE::RhiShaderProgram::DephStencilInfo dephStencilInfo =
    {
        .depthCompareOp = PC_CORE::CompareOp::Less,
        .enableDepthTest = true
    };

    constexpr PC_CORE::RhiShaderProgram::BlendState blendInfo = {
        .enabled = true,
        .srcColorBlendFactor = PC_CORE::BlendFactor::SrcAlpha,
        .dstColorBlendFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
        .colorBlendOp = PC_CORE::BlendOp::Add,

        .srcAlphaBlendFactor = PC_CORE::BlendFactor::One,
        .dstAlphaBlendFactor = PC_CORE::BlendFactor::OneMinusSrcAlpha,
        .alphaBlendOp = PC_CORE::BlendOp::Add,

        .colorMask = static_cast<PC_CORE::ColorComponent>(PC_CORE::ColorComponent::ColorComponentR |
            PC_CORE::ColorComponent::ColorComponentG |
            PC_CORE::ColorComponent::ColorComponentB |
            PC_CORE::ColorComponent::ColorComponentA)
    };
    PC_CORE::RhiShaderProgram::ShaderGraphicPointInfo shaderGraphicPointInfo =
    {
        .rasterizerInfo = rasterizerInfo,
        .dephInfo = dephStencilInfo,
        .blendInfo = blendInfo,
        .vertexInputBindingDescritions = {},
        .vertexAttributeDescriptions = {}
    };

    std::vector<PC_CORE::RhiShaderProgram::ShaderModule> sourceList =
    {
        {
            PC_CORE::RhiShaderProgram::ShaderStageType::Vertex,
            PC_CORE::ResourceManager::Get<PC_CORE::ShaderSourceBinary>("DrawSprite.vs.hlsl.binary")->GetCode(),
        },
        {
            PC_CORE::RhiShaderProgram::ShaderStageType::Pixel,
            PC_CORE::ResourceManager::Get<PC_CORE::ShaderSourceBinary>("DrawSprite.ps.hlsl.binary")->GetCode()
        }
    };

    PC_CORE::RhiShaderProgram::ShaderInfo shaderInfo =
    {
        .type = PC_CORE::RhiShaderProgram::PipelineType::Graphic,
        .shaderInfoData = shaderGraphicPointInfo
    };

    PC_CORE::RhiShaderProgram::ProgramShaderCreateInfo graphicShaderProgramCreateInfo =
    {
        .shaderInfo = shaderInfo,
        .renderPass = m_Editor->gameApp.Renderer.RenderPasses.ForwardPass.get(),
        .shaderModule = sourceList,
        .attachementCount = 1,
        .subPassIndex = 0
    };


    m_DrawSpriteShader = PC_CORE::ResourceManager::Create<PC_CORE::GraphicShader>(
        "SpriteShader", graphicShaderProgramCreateInfo);
    m_DrawSpriteShader.lock()->Get()->Build();

    auto lockedShader = m_DrawSpriteShader.lock();
    if (lockedShader == nullptr)
        return;

    m_CameraSet.reset(lockedShader->Get()->CreateDescriptorBinding());
    m_DirectionalDescriptorSet.reset(lockedShader->Get()->CreateDescriptorBinding());
    m_SpotLightDescriptorSet.reset(lockedShader->Get()->CreateDescriptorBinding());
    m_PointLightDescriptorSet.reset(lockedShader->Get()->CreateDescriptorBinding());

    PC_CORE::BufferDescriptor cameraBufferDescritptor
    {
        .buffer = m_Editor->gameApp.Renderer.UniformBuffers.CameraUniformBuffer.Get(),
    };

    PC_CORE::ImageSamplerDescriptor directionalTexture
    {
        .sampler = m_SpriteSampler.Get(),
        .texture = m_DirectionalLightTexture.lock()->Get(),
        .resourceState = RhiResourceState::ShaderRead
    };

    PC_CORE::ImageSamplerDescriptor spothLightTexture
    {
        .sampler = directionalTexture.sampler,
        .texture = m_SpotLightTexture.lock()->Get(),
        .resourceState = RhiResourceState::ShaderRead
    };

    PC_CORE::ImageSamplerDescriptor pointLightTexture
    {
        .sampler = directionalTexture.sampler,
        .texture = m_PointLightTexture.lock()->Get(),
        .resourceState = RhiResourceState::ShaderRead
    };

    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
    {
        {
            PC_CORE::ShaderProgramDescriptorType::UniformBuffer,
            CAMERA_BINDING,
            cameraBufferDescritptor,
        }
    };

    m_CameraSet->SetBindings(descriptorSets, SCENE_DESCRIPTOR_SET).Build();

    descriptorSets =
    {
        {
            PC_CORE::ShaderProgramDescriptorType::CombinedImageSampler,
            SPRITE_TEXTURE,
            directionalTexture,
        }
    };
    m_DirectionalDescriptorSet->SetBindings(descriptorSets, SPRITE_SET).Build();

    descriptorSets[0].descriptor = spothLightTexture;
    m_SpotLightDescriptorSet->SetBindings(descriptorSets, SPRITE_SET).Build();

    descriptorSets[0].descriptor = pointLightTexture;
    m_PointLightDescriptorSet->SetBindings(descriptorSets, SPRITE_SET).Build();*/
}
