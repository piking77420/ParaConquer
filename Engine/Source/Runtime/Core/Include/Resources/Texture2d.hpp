#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "Texture.hpp"
#include "LowRenderer/RhiTexure2d.hpp"
#include "Math/ToolboxTypedef.hpp"
#include "Reflection/Reflector.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "Rendering/Buffer/GpuBuffer.hpp"


BEGIN_PCCORE
    struct Texture2DMetaData
    {
        std::vector<uint8_t> data;

        REFLECT(Texture2DMetaData)
        REFLECT_MEMBER(Texture2DMetaData, data)
    };

    class Texture2D final : public Texture
    {
    public:
        PC_CORE_API IMP_DYNAMIC_REFLECT()

        DEFAULT_COPY_MOVE_OPERATIONS(Texture2D)

        PC_CORE_API void AfterSerialize(Serializer* _serializer) const override;

        PC_CORE_API void AfterDeSerialize(Serializer* _serializer) override;

        PC_CORE_API RhiFormat GetRhiFormat() const;

        std::shared_ptr<RhiResource> GetRhiHandle() const override
        {
            return m_Texture2D;
        }

        std::shared_ptr<RhiTexture2D> GetRhiTexture2D()
        {
            return m_Texture2D;
        }

        const std::shared_ptr<RhiTexture2D> GetRhiTexture2D() const
        {
            return m_Texture2D;
        }

        Tbx::Vector2i GetSize() const
        {
            return m_Size;
        }

        RhiFormat GetFormat()
        {
            return m_Texture2D ? GetFormat() : RhiFormat::Undefined;
        }

        PC_CORE_API Texture2D();

        PC_CORE_API explicit Texture2D(const std::string& _name);

        PC_CORE_API explicit Texture2D(const std::string& _name, const std::string& _path);

        PC_CORE_API explicit Texture2D(const CreateImageInfo& _createTextureInfo);

        PC_CORE_API ~Texture2D() override;

    private:
        std::shared_ptr<RhiTexture2D> m_Texture2D;

        RhiFormat m_Format{RhiFormat::Undefined};

        Tbx::Vector2i m_Size{};

        void LoadTextureFromPath(const std::string& _path);
        Texture2DMetaData m_Texture2DMetaData;

        REFLECT(Texture2D, Resource)
        REFLECT_MEMBER(Texture2D, m_Size)
        REFLECT_MEMBER(Texture2D, m_Format)
        REFLECT_MEMBER(Texture2D, m_Texture2DMetaData)
    };


END_PCCORE
