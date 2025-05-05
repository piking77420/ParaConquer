#include "rendering/gbuffers.hpp"

#include "low_renderer/rhi.hpp"

std::shared_ptr<PC_CORE::FrameBuffer> PC_CORE::Gbuffers::GetFrameBuffer() const
{
    return m_FrameBuffer;
}

void PC_CORE::Gbuffers::HandleResize(Tbx::Vector2i _targetSize , std::shared_ptr<RhiRenderPass> _renderPass)
{
    if (_targetSize != m_size)
    {
        m_size = _targetSize;
        CreateGBuffers();

        std::vector<Texture*> handles;
        handles.reserve(m_gbuffers.size());

        for (auto& gbuffer : m_gbuffers)
            handles.emplace_back(gbuffer.get());
        
        CreateFrameInfo create_frame_info =
            {
            .width = static_cast<uint32_t>(m_size.x),
            .height = static_cast<uint32_t>(m_size.y),
            .attachements = &handles,
            .renderPass = _renderPass.get()
            };
        m_FrameBuffer = Rhi::CreateFrameBuffer(create_frame_info);
    }
    
    
}

PC_CORE::Gbuffers::Gbuffers(Tbx::Vector2i _size , std::shared_ptr<RhiRenderPass> _renderPass) : m_size(_size)
{
    
}

void PC_CORE::Gbuffers::CreateGBuffers()
{
    GbufferType gbufferType = {};
    for (auto& frameInFlight : m_gbuffers)
    {
        CreateTextureInfo texture_info =
            {
            .width = m_size.x,
            .height = m_size.y,
            .depth = 1,
            .mipsLevels = 1,
            .imageType = ImageType::TYPE_2D,
            .format = RHIFormat::COUNT,
            .channel = Channel::DEFAULT,
            .textureAttachement = TextureAttachement::Color,
            .textureNature = TextureNature::RenderTarget,
            .canbeSampled = false,
            .GenerateMipMap = false,
            .data = nullptr,
            };

        switch (gbufferType)
        {
        case Albedo:
            texture_info.format = RHIFormat::R8G8B8A8_UNORM;
            texture_info.channel = Channel::RGBA;
            texture_info.textureAttachement = TextureAttachement::Color;

            break;
        case Depth:
            texture_info.format = RHIFormat::D32_SFLOAT;
            texture_info.channel = Channel::GREY;
            texture_info.textureAttachement = TextureAttachement::DepthStencil;
            break;
        case Count:
            break;
        default: ;
        }
        
        frameInFlight = std::make_shared<Texture>(texture_info);
        gbufferType = static_cast<GbufferType>((static_cast<int>(gbufferType) + 1) % GbufferType::Count); 
    }
}

std::shared_ptr<PC_CORE::Texture> PC_CORE::Gbuffers::GetTexture(GbufferType type) const
{
    const size_t index = static_cast<size_t>(type);

    if (m_gbuffers.size() <= index)
    {
        return nullptr;
        
    }

    return m_gbuffers[index];
}
