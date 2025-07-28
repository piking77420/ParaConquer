#include "rendering/gbuffers.hpp"

#include "app.hpp"
#include "low_renderer/rhi.hpp"
#include "resources/resource_manager.hpp"


std::shared_ptr<PC_CORE::FrameBuffer> PC_CORE::Gbuffers::GetGbufferFrameBuffer() const
{
    return m_GbufferFrameBuffer;
}

std::shared_ptr<PC_CORE::FrameBuffer> PC_CORE::Gbuffers::GetForwardFrameBuffer() const
{
    return m_ForwardFrameBuffer;
}

void PC_CORE::Gbuffers::HandleResize(Tbx::Vector2i _targetSize , std::shared_ptr<RhiRenderPass> _forwardPass, std::shared_ptr<RhiRenderPass> _gbufferPass)
{
    if (_targetSize != m_size)
    {
        m_size = _targetSize;
        const CreateImageInfo imageInfo =
        {
            .width = m_size.x,
            .height = m_size.y,
            .depth = 1,
            .layerCount = 1,
            .mipsLevels = 1,
            .textureType = TextureType::Texture2D,
            .format = RHIFormat::R8G8B8A8_UNORM,
            .channel = Channel::DEFAULT,
            .textureUsage = TextureUsage::RenderTarget | TextureUsage::Sampled,
            .memoryVisibility = MemoryLocalisation::GPU_Only,
            .samples = 1,
            .GenerateMipMap = false,
            .datas = {},
        };
        m_Image = Texture2D(imageInfo);

        if (m_DescriptorSets != nullptr)
        {
            std::shared_ptr<GraphicShader> deferredShader = App::instance->renderer.m_DeferedShader.lock();
            deferredShader->FreeDescriptorSet(&m_DescriptorSets);
        }

        CreateGBuffers();

        // forward
        std::vector<FrameBufferAttachementDesriptor> attachementDesriptors =
        {
            {
                .texture = &m_Image,
            },
            {
                .texture = m_Gbuffers[static_cast<uint8_t>(GbufferType::Depth)].get(),
            }
        };
        
         CreateFrameInfo create_frame_info =
            {
            .width = static_cast<uint32_t>(m_size.x),
            .height = static_cast<uint32_t>(m_size.y),
            .attachements = &attachementDesriptors,
            .renderPass = _forwardPass.get()
            };
        m_ForwardFrameBuffer = Rhi::CreateFrameBuffer(create_frame_info);

        // Gbuffer
        create_frame_info.renderPass = _gbufferPass.get();
        // + 1 for final image
        attachementDesriptors.resize(static_cast<uint8_t>(GbufferType::Count) + 1);
        for (uint8_t i = 0; i < static_cast<uint8_t>(GbufferType::Count) - 1; i++)
        {
            attachementDesriptors[i].texture = m_Gbuffers[i].get();
        }

        // SWAPING because in render pass vulkan backend the last attachement is the depth attement
        attachementDesriptors[attachementDesriptors.size() - 2].texture = &m_Image;
        attachementDesriptors[attachementDesriptors.size() - 1].texture = m_Gbuffers[m_Gbuffers.size() - 1].get();

        m_GbufferFrameBuffer = Rhi::CreateFrameBuffer(create_frame_info);
    }
    
    
}

PC_CORE::Gbuffers::Gbuffers(Tbx::Vector2i _size , std::shared_ptr<RhiRenderPass> _renderPass) : m_size(_size)
{
    
}

void PC_CORE::Gbuffers::CreateGBuffers()
{
    GbufferType gbufferType = {};
    for (auto& frameInFlight : m_Gbuffers)
    {
    
        CreateImageInfo texture_info =
        {
            .width = m_size.x,
            .height = m_size.y,
            .depth = 1,
            .layerCount = 1,
            .mipsLevels = 1,
            .textureType = TextureType::Texture2D,
            .format = RHIFormat::COUNT,
            .channel = Channel::DEFAULT,
            .textureUsage = TextureUsage::RenderTarget | TextureUsage::Sampled,
            .memoryVisibility = MemoryLocalisation::GPU_Only,
            .samples = 1,
            .GenerateMipMap = false,
            .datas = {},
        };

        static_assert(static_cast<size_t>(GbufferType::Depth) == static_cast<size_t>(GbufferType::Count) - 1, "Last GBuffer Shoulbe be depth for texture usage purpose" );
        switch (gbufferType)
        {
        case GbufferType::Albedo:
            texture_info.format = RHIFormat::R8G8B8A8_UNORM; // TODO GAMA CORRECTION 
            texture_info.channel = Channel::RGBA;
            break;
        case GbufferType::Normal:
            texture_info.format = RHIFormat::R16G16_SNORM;
            texture_info.channel = Channel::RGB;
            break;
        case GbufferType::RoughnessMetallicAo:
            texture_info.format = RHIFormat::R8G8B8A8_UNORM;
            texture_info.channel = Channel::RGBA;
            break;
        case GbufferType::WorldPosition:
            texture_info.format = RHIFormat::R16G16B16A16_SFLOAT;
            texture_info.channel = Channel::RGBA;
            break;
        case GbufferType::Depth:
            texture_info.format = RHIFormat::D32_SFLOAT;
            texture_info.channel = Channel::GREY;
            texture_info.textureUsage =  TextureUsage::Depth;
            break;
        case GbufferType::Count:
        default:
            assert(false);
        }
        
        frameInFlight = std::make_shared<Texture2D>(texture_info);
        gbufferType = static_cast<GbufferType>((static_cast<int>(gbufferType) + 1) % static_cast<uint8_t>(GbufferType::Count)); 
    }

    std::shared_ptr<GraphicShader> deferredShader = App::instance->renderer.m_DeferedShader.lock();
    
    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets;
    descriptorSets.resize(static_cast<uint8_t>(GbufferType::Depth));
    std::array<InputAttachementDescriptor, static_cast<uint8_t>(GbufferType::Depth)> inputAttachements;

    for (size_t i = 0; i < static_cast<uint8_t>(GbufferType::Depth); i++)
    {
        inputAttachements[i] =
            {
            .image = m_Gbuffers[i].get(),
            },
        
        descriptorSets[i] =
            {
            .shaderProgramDescriptorType = ShaderProgramDescriptorType::InputAttachment,
            .bindingIndex = static_cast<uint32_t>(i),
            .uniformBufferDescriptor = nullptr,
            .imageSamperDescriptor = nullptr,
            .inputAttachementDescriptor = &inputAttachements[i]
            };
    }
    deferredShader->AllocDescriptorSet(&m_DescriptorSets, GBUFFER_SET);
    m_DescriptorSets->WriteDescriptorSets(descriptorSets);
}

std::shared_ptr<PC_CORE::Texture2D> PC_CORE::Gbuffers::GetTexture(GbufferType type) const
{
    const size_t index = static_cast<size_t>(type);

    if (m_Gbuffers.size() <= index)
    {
        return nullptr;
        
    }

    return m_Gbuffers[index];
}
