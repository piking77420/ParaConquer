#include "Resources/Texture2d.hpp"

#include "Log.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Resources/FileLoader.hpp"


using namespace PC_CORE;

Texture2D::Texture2D()
{
    DYNAMIC_REFLECT_INIT
}


Texture2D::Texture2D(PC_CORE::Rhi& rhi, const std::string& _name)
    : Texture(_name)
{
    m_RhiTexture.reset(rhi.CreateTexture());
    m_RhiTexture->SetName(Name);
}

Texture2D::Texture2D(PC_CORE::Rhi& rhi, std::string&& _name)
    : Texture(std::move(_name))
{
    m_RhiTexture.reset(rhi.CreateTexture());
    m_RhiTexture->SetName(Name);
}

Texture2D::Texture2D(PC_CORE::Rhi& rhi, const std::filesystem::path& _Path)
    : Texture(_Path.filename().generic_string())
{
    m_RhiTexture.reset(rhi.CreateTexture());
    m_RhiTexture->SetName(Name);
}

Texture2D::Texture2D(std::unique_ptr<PC_CORE::RhiTexture> _Texture)
    : Texture(std::move(_Texture))
{

}


void Texture2D::AfterSerialize(Serializer* _serializer) const
{

}

void Texture2D::AfterDeSerialize(Serializer* _serializer)
{
    /*
    _serializer->DeSerializeStream(m_Texture2DMetaData);


    if (!m_Texture2DMetaData.data.empty() && m_Format != RhiFormat::Undefined
        && m_Size != Tbx::Vector2i::Zero() && m_TextureChannel != Channel::Default)
    {
        PC_LOG("Create Texture");

        const CreateImageInfo createTextureInfo =
        {
            .Width = m_Size.x,
            .Height = m_Size.y,
            .Depth = 1,
            .LayerCount = 1,
            .MipsLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_Size.x, m_Size.y)))) + 1,
            .TextureType = TextureType::Texture2D,
            .Format = m_Format,
            .Channel = m_TextureChannel,
            .TextureUsage = TextureUsage::Sampled,
            .MemoryVisibility = MemoryLocalisation::GpuOnly,
            .Samples = 1,
            .GenerateMipMap = true,
            .Datas = {reinterpret_cast<void*>(m_Texture2DMetaData.data.data())}
        };

        m_Texture2D = Rhi::CreateTexture2D(createTextureInfo);
        m_Texture2DMetaData.data.clear();
    }*/
}
