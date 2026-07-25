#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "Texture.hpp"
#include <Math/ToolBoxTypeDef.hpp>
#include "Reflection/Reflector.hpp"

BEGIN_PCCORE
    struct Texture2DMetaData
    {
        // may had a soruce files 
        std::vector<uint8_t> data;

        REFLECT(Texture2DMetaData)
        REFLECT_MEMBER(Texture2DMetaData, data)
    };

    class Texture2D final : public Texture
    {
    public:        
        PC_CORE_API explicit Texture2D(PC_CORE::Rhi& rhi, const std::string& _name);

        PC_CORE_API explicit Texture2D(PC_CORE::Rhi& rhi, std::string&& _name);

        PC_CORE_API explicit Texture2D(PC_CORE::Rhi& rhi, const std::filesystem::path& _Path);

        PC_CORE_API explicit Texture2D(std::unique_ptr<PC_CORE::RhiTexture> _Texture);

        PC_CORE_API explicit Texture2D(std::unique_ptr<PC_CORE::RhiTexture> _Texture, const std::filesystem::path& _Path);

        PC_CORE_API Texture2D();

        PC_CORE_API ~Texture2D() override = default;

        PC_CORE_API Texture2D(Texture2D&&) noexcept = default;

        PC_CORE_API Texture2D& operator=(Texture2D&&) noexcept = default;

        IMP_DYNAMIC_REFLECT()
        
        PC_CORE_API void AfterSerialize(Serializer* _serializer) const override;

        PC_CORE_API void AfterDeSerialize(Serializer* _serializer) override;

        std::string_view GetPath() const
        {
            return std::string_view(m_Path);
        }
    
    private:
        std::string m_Path;

        REFLECT(Texture2D, Resource)
        REFLECT_MEMBER(Texture2D, m_Path)
    };


END_PCCORE
