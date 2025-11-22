#pragma once

#include "RhiTypedef.h"
#include "LowRenderer/RhiResource.hpp"

BEGIN_PCCORE

class CommandList;

    class RhiTexture : public RhiResource
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

        enum TextureUsageFlag : uint8_t
        {
            None = 0,
            Sampled = 1 << 0, // Shader-readable (SRV)
            RenderTarget = 1 << 1, // Color attachment (ex: RGBA render target)
            Storage = 1 << 4, // Shader-writable (UAV)
            TransferSrc = 1 << 5,
            TransferDst = 1 << 6,
        
            All = Sampled | RenderTarget | Storage | TransferSrc |TransferDst,
        };
    
        struct RhiTextureDesciptor
        {
            uint32_t Width = 0;
            uint32_t Height = 0;
            uint32_t Depth = 0;
            // MipLevel
            uint32_t Level = 1;
            // Array slice or face
            uint32_t LayerCount = 1;
            uint32_t Samples = 1;
            Type TextureType = Type::None;
            TextureUsageFlag TextureUsage = TextureUsageFlag::None;
            RhiFormat RhiFormat = RhiFormat::Undefined;
            bool AllowCpuAcces = false;
        };

       
        REFLECT(TextureUsageFlag)
    
        DEFAULT_COPY_MOVE_OPERATIONS(RhiTexture)
    
        PC_CORE_API RhiTexture(Rhi& _Rhi, const std::string& _name, const RhiTextureDesciptor& _rhiTextureDesciptor, MemoryUsage _memoryUsage);

        PC_CORE_API RhiTexture(Rhi& _Rhi, std::string&& _name, const RhiTextureDesciptor& _rhiTextureDesciptor, MemoryUsage _memoryUsage);
    
        PC_CORE_API ~RhiTexture() override = default;
    
        PC_CORE_API virtual void UploadData2D(CommandList* commandList, const void* _imageData, uint32_t _imageWidht, uint32_t _imageHeight, RhiChannel _channel) = 0;
    
        PC_CORE_API virtual void UploadDataLayer(CommandList* commandList, const std::vector<void*>& _imageDatas, uint32_t _imageWidht, uint32_t _imageHeight, uint32_t _layer, RhiChannel _channel) = 0;
    
        PC_CORE_API virtual void GenerateMipMap(CommandList* commandList) = 0;
    
        MemoryUsage GetMemoryUsage() const
        {
            return m_MemoryUsage;
        }
    
        uint32_t GetWidth() const
        {
            return m_RhiTextureDesciptor.Width;
        }
    
        uint32_t GetHeight() const
        {
            return m_RhiTextureDesciptor.Height;
        }
    
        uint32_t GetDepth() const
        {
            return m_RhiTextureDesciptor.Depth;
        }
    
        uint32_t GetMipLevelCount() const
        {
            return m_RhiTextureDesciptor.Level;
        }

        uint32_t GetLayerCount() const
        {
            return m_RhiTextureDesciptor.LayerCount;
        }
    
        uint32_t GetSamples() const
        {
            return m_RhiTextureDesciptor.Samples;
        }
    
        Type GetTextureType() const
        {
            return m_RhiTextureDesciptor.TextureType;
        }

        TextureUsageFlag GetTextureUsage() const
        {
            return m_RhiTextureDesciptor.TextureUsage;
        }

        RhiFormat GetRhiFormat() const
        {
            return m_RhiTextureDesciptor.RhiFormat;
        }
    
        bool GetAllowCpuAcces() const
        {
            return m_RhiTextureDesciptor.AllowCpuAcces;
        }
    
        bool IsNeededToGenerateMip() const
        {
            return m_RhiTextureDesciptor.Level > 1;
        }
    
        size_t GetNbrOfInFlightResource() const
        {
            return GetNbrOfHandle(m_MemoryUsage);
        }

        static bool IsDepthFormat(RhiFormat _format);

    protected:    
        RhiTextureDesciptor m_RhiTextureDesciptor;
    };

END_PCCORE
