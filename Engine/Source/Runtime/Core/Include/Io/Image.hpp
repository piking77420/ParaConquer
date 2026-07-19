#pragma once

#include <string>
#include <memory>

#include "CoreHeader.hpp"
#include "LowRenderer/RhiTypedef.h"

BEGIN_PCCORE
    struct ImageDeleter 
    {

        ImageDeleter() = default;
        ImageDeleter(ImageDeleter&&) noexcept = default;
        ImageDeleter& operator=(ImageDeleter&&) noexcept = default;

        PC_CORE_API void operator()(uint8_t* p);
    };

    class Image
    {
    public:
        

        struct MipsDescriptor
        {
            uint32_t width;
            uint32_t height;
            uint32_t offset;
            uint32_t size;
        };

        DEFAULT_COPY_MOVE_OPERATIONS(Image);

        PC_CORE_API Image(const std::string& _path, PC_CORE::RhiChannel _desireChannel = RhiChannel::Default);

        PC_CORE_API Image(const char* _path, PC_CORE::RhiChannel _desireChannel = RhiChannel::Default);

        PC_CORE_API Image(std::string_view _ImageName, const uint8_t* _ptr, size_t _size, const char* _name, PC_CORE::RhiChannel _desireChannel = RhiChannel::Default);

        Image() = default;

        ~Image() = default;

        PC_CORE::RhiChannel GetChannel() const
        {
            return m_Channel;
        }

        const uint8_t* GetData() const
        {
            return m_Data.get();
        }

        uint8_t* GetData()
        {
            return m_Data.get();
        }

        bool IsHdr() const
        {
            return m_IsHDR;
        }

        operator bool() const
        {
            return m_Data.get() != nullptr;
        }

        [[nodiscard]] std::unique_ptr<uint8_t[], ImageDeleter> Release() noexcept
        {
            auto tmp = std::move(m_Data);
            m_Data.reset();
            return tmp;
        }

        const std::vector<MipsDescriptor>& GetMipDescriptor() const
        {
            return m_MipsDescriptor;
        }

        std::optional<RhiFormat> GetBuildInFormat() const
        {
            return m_BuildInFormat;
        }

    private:
        REFLECT(Image);

        PC_CORE::RhiChannel m_Channel{};

        std::unique_ptr<uint8_t[], ImageDeleter> m_Data;

        std::vector<MipsDescriptor> m_MipsDescriptor;

        std::optional<RhiFormat> m_BuildInFormat;

        bool m_IsHDR = false;

        PC_CORE_API void LoadFromPath(const std::string_view& _view, PC_CORE::RhiChannel _desireChannel);

    };

END_PCCORE
