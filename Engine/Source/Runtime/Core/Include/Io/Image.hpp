#pragma once

#include <string>
#include <memory>

#include "CoreHeader.hpp"
#include "LowRenderer/RhiTypedef.h"

BEGIN_PCCORE
    class Image
    {
    public:
        struct ImageDeleter {

            ImageDeleter() = default;
            ImageDeleter(ImageDeleter&&) noexcept = default;
            ImageDeleter& operator=(ImageDeleter&&) noexcept = default;

            PC_CORE_API void operator()(uint8_t* p);
        };

        DEFAULT_COPY_MOVE_OPERATIONS(Image);

        PC_CORE_API Image(const std::string& _path, PC_CORE::RhiChannel _desireChannel);

        PC_CORE_API Image(const char* _path, PC_CORE::RhiChannel _desireChannel);

        PC_CORE_API Image(const uint8_t* _ptr, size_t _size, const char* _name, PC_CORE::RhiChannel _Channel);

        Image() = default;

        ~Image() = default;


        uint32_t GetWidht() const
        {
            return m_Width;
        }

        uint32_t GetHeight() const
        {
            return m_Height;
        }

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

        size_t GetSizeInBytes() const
        {
            return m_SizeInBytes;
        }

        [[nodiscard]] std::unique_ptr<uint8_t[], ImageDeleter> Release() noexcept
        {
            auto tmp = std::move(m_Data);
            m_Data.reset();
            return tmp;
        }


    private:
       
        REFLECT(Image);

        uint32_t m_Width = 0;

        uint32_t m_Height = 0;

        PC_CORE::RhiChannel m_Channel{};

        uint32_t m_SizeInBytes = 0;

        std::unique_ptr<uint8_t[], ImageDeleter> m_Data;

        bool m_IsHDR = false;

        PC_CORE_API void ComputeDataSize();

        PC_CORE_API void LoadFromPath(const std::string_view& _view, PC_CORE::RhiChannel _desireChannel);
    };

END_PCCORE
