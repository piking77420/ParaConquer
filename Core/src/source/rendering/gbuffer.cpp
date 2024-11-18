#include "rendering/gbuffer.h"

#include "rendering/frame_buffer.hpp"

PC_CORE::Gbuffer& PC_CORE::Gbuffer::operator=(Gbuffer&& _other)
{
    for (size_t i = 0; i < m_RenderTargets.size(); i++)
    {
        m_RenderTargets[i] = std::move(_other.m_RenderTargets[i]);
    }

    return *this;
}

PC_CORE::Gbuffer::Gbuffer()
{
    m_RenderTargets.resize(MAX_FRAMES_IN_FLIGHT);
    m_FrameBuffers.resize(MAX_FRAMES_IN_FLIGHT);
}

bool PC_CORE::Gbuffer::HandleResize(Tbx::Vector2i _size, const RenderPass& _renderPass)
{
    if (m_Size != _size)
    {
        m_Size = _size;
        for (size_t i = 0; i < m_RenderTargets.size(); i++)
        {
            for (size_t j = 0; j < m_RenderTargets[i].size(); j++)
            {
                const GBufferType type = static_cast<GBufferType>(j);

                PC_CORE::CreateTextureInfo createTextureInfo =
                    {
                    .width = m_Size.x,
                    .height = m_Size.y,
                    .depth = 1,
                    .mipsLevels = 1,
                    .imageType = ImageType::TYPE_2D,
                    .format = RHIFormat::COUNT,
                    .textureAspect = TextureAspect::COLOR,
                    .GenerateMipMap = false,
                    .useAsAttachement = true,
                    };
            
                switch (type)
                {
                case GBufferType::COLOR:
                case GBufferType::POSITION:
                case GBufferType::NORMAL:
                    createTextureInfo.format = RHIFormat::R8G8B8_SRGB;
                    break;
                case GBufferType::TEXTCOORD:
                    createTextureInfo.format = RHIFormat::R8G8_SRGB;
                    break;
                case GBufferType::COUT:
                    createTextureInfo.format = RHIFormat::UNDEFINED;
                    break;
                default:
                    createTextureInfo.format = RHIFormat::UNDEFINED;
                    break;
                }

                m_RenderTargets[i][j] = Texture(createTextureInfo);
            }

            const FrameBufferCreateInfo frameBufferCreateInfo =
                {
                .renderTargets = m_RenderTargets[i].data(),
                .renderTargetCount = static_cast<uint32_t>(m_RenderTargets[i].size()),
                .renderPass = &_renderPass,
                .width = static_cast<uint32_t>(m_Size.x),
                .height = static_cast<uint32_t>(m_Size.y),
                .layers = 1
                };

            m_FrameBuffers[i] = FrameBuffer(frameBufferCreateInfo);
        }

        return true;
    }

    return false; 
}
