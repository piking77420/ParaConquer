#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "Texture.hpp"
#include "Math/ToolboxTypedef.hpp"
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
        PC_CORE_API IMP_DYNAMIC_REFLECT()

        DEFAULT_COPY_MOVE_OPERATIONS(Texture2D)
        
        PC_CORE_API explicit Texture2D(PC_CORE::Rhi& rhi, const std::string& _name);

        PC_CORE_API explicit Texture2D(PC_CORE::Rhi& rhi, std::string&& _name);

        PC_CORE_API Texture2D();

        PC_CORE_API ~Texture2D() override = default;
        
        PC_CORE_API void AfterSerialize(Serializer* _serializer) const override;

        PC_CORE_API void AfterDeSerialize(Serializer* _serializer) override;
    
    private:
        REFLECT(Texture2D, Resource)
    };


END_PCCORE
