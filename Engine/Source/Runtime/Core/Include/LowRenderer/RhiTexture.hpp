#pragma once

#include "LowRenderer/RhiResource.hpp"
#include "RhiTypedef.h"

BEGIN_PCCORE

class CommandList;

class RhiTexture : public RhiResourceT<RhiTexture>
{
public:
    
    enum class Type
    {
        None,
        Texture1D,
        Texture2D,
        TextureArray2D,
        CubeMap,
        CubeMapArray,
        Count,
    };
    REFLECT(Type)

    enum TextureUsageFlagBits : uint32_t
    {
        None = 0,
        Sampled = 1 << 0, // Shader-readable (SRV)
        RenderTarget = 1 << 1, // Color attachment (ex: RGBA render target)
        LoadAndStore = 1 << 4, // Shader-writable (UAV)
        DepthStencil = 1 << 5,
        TransferSrc = 1 << 6,
        TransferDst = 1 << 7,
        InputAttachement,
        
        All = Sampled | RenderTarget | LoadAndStore | TransferSrc |TransferDst,
    };

    using TextureUsageFlag = uint32_t;
       
    REFLECT(TextureUsageFlagBits)
    
    DEFAULT_COPY_MOVE_OPERATIONS(RhiTexture)
    
    PC_CORE_API RhiTexture(Rhi& _Rhi);
    
    PC_CORE_API ~RhiTexture() override = default;
    
    PC_CORE_API virtual bool UploadData2D(CommandList* _CommandList, const void* _ImageData, RhiFormat _Format, uint32_t _ImageWidht, uint32_t _ImageHeight) = 0;
    
    PC_CORE_API virtual void UploadDataLayer(CommandList* commandList, const std::vector<void*>& _imageDatas, uint32_t _imageWidht, uint32_t _imageHeight, uint32_t _layer) = 0;
    
    PC_CORE_API virtual bool GenerateMipMap(CommandList* commandList, PC_CORE::Filter _Filter, RhiResourceState _StateAfterOperation) = 0;

    PC_CORE_API virtual RhiResourceState GetResourceState() const = 0;
    
    // Setter 

    RhiTexture& SetWidth(uint32_t _Width)
    {
        m_Width = _Width;
        return *this;
    }

    RhiTexture& SetHeight(uint32_t _Height)
    {
        m_Height = _Height;
        return *this;
    }

    RhiTexture& SetDepth(uint32_t _Depth)
    {
        m_Depth = _Depth;
        return *this;
    }

    RhiTexture& SetLevel(uint32_t _Level)
    {
        m_Level = _Level;
        return *this;
    }

    RhiTexture& SetLayer(uint32_t _Layer)
    {
        m_Layer = _Layer;
        return *this;
    }

    RhiTexture& SetSamples(uint32_t _Samples)
    {
        m_Samples = _Samples;
        return *this;
    }

    RhiTexture& SetTextureType(Type _Type)
    {
        m_TextureType = _Type;
        return *this;
    }

    RhiTexture& SetTextureUsage(TextureUsageFlag _TextureUsageFlag)
    {
        m_TextureUsage = _TextureUsageFlag;
        return *this;
    }

    RhiTexture& SetRhiFormat(RhiFormat _RhiFormat)
    {
        m_RhiFormat = _RhiFormat;
        return *this;
    }

    // Getter

    uint32_t GetWidth() const
    {
        return m_Width;
    }
    
    uint32_t GetHeight() const
    {
        return m_Height;
    }
    
    uint32_t GetDepth() const
    {
        return m_Depth;
    }
    
    uint32_t GetLevel() const
    {
        return m_Level;
    }

    uint32_t GetLayer() const
    {
        return m_Layer;
    }
    
    uint32_t GetSamples() const
    {
        return m_Samples;
    }
    
    Type GetTextureType() const
    {
        return m_TextureType;
    }

    TextureUsageFlag GetTextureUsage() const
    {
        return m_TextureUsage;
    }

    RhiFormat GetRhiFormat() const
    {
        return m_RhiFormat;
    }
    
    bool IsNeededToGenerateMip() const
    {
        return m_Level > 1;
    }
    
    size_t GetNbrOfInFlightResource() const
    {
        return GetNbrOfHandle(m_MemoryUsage);
    }

    static bool IsDepthFormat(RhiFormat _format);

protected:    
    uint32_t m_Width = 0;

    uint32_t m_Height = 0;

    uint32_t m_Depth = 1;

    // MipLevel
    uint32_t m_Level = 1;
    // Array slice or face
    uint32_t m_Layer = 1;

    uint32_t m_Samples = 1;

    Type m_TextureType = Type::Texture2D;

    TextureUsageFlag m_TextureUsage = TextureUsageFlagBits::None;

    RhiFormat m_RhiFormat = RhiFormat::Undefined;
};


END_PCCORE
