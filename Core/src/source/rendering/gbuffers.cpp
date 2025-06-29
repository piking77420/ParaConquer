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

        std::vector<FrameBufferAttachementDesriptor> attachementDescritpor =
        {
            {
                .texture = m_gbuffers[0].get(),
            },
            {
                .texture = m_gbuffers[1].get(),
            }
            
        };
        
        CreateFrameInfo create_frame_info =
            {
            .width = static_cast<uint32_t>(m_size.x),
            .height = static_cast<uint32_t>(m_size.y),
            .attachements = &attachementDescritpor,
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
    
        CreateImageInfo2D texture_info =
        {
            .width = m_size.x,
            .height = m_size.y,
            .depth = 1,
            .mipsLevels = 1,
            .format = RHIFormat::COUNT,
            .channel = Channel::DEFAULT,
            .textureUsage = TextureUsage::RenderTarget | TextureUsage::Sampled,
            .textureMemoryUsage = TextureMemoryUsage::GPU_Only,
            .samples = 1,
            .GenerateMipMap = false,
            .data = nullptr,
        };

        switch (gbufferType)
        {
        case Albedo:
            texture_info.format = RHIFormat::R8G8B8A8_UNORM;
            texture_info.channel = Channel::RGBA;

            break;
        case Depth:
            texture_info.format = RHIFormat::D32_SFLOAT;
            texture_info.channel = Channel::GREY;
            texture_info.textureUsage = TextureUsage::Depth;
            break;
        case Count:
            break;
        default: ;
        }
        
        frameInFlight = std::make_shared<Texture2D>(texture_info);
        gbufferType = static_cast<GbufferType>((static_cast<int>(gbufferType) + 1) % GbufferType::Count); 
    }
}

std::shared_ptr<PC_CORE::Texture2D> PC_CORE::Gbuffers::GetTexture(GbufferType type) const
{
    const size_t index = static_cast<size_t>(type);

    if (m_gbuffers.size() <= index)
    {
        return nullptr;
        
    }

    return m_gbuffers[index];
}
