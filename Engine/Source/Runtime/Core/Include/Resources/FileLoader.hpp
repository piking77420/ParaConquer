#pragma once

#include <String>
#include <memory>

#include "CoreHeader.hpp"
#include "LowRenderer/RhiTypedef.h"

BEGIN_PCCORE


    class PC_CORE_API FileLoader
    {
    public:
        FileLoader() = delete;

        ~FileLoader() = delete;

        static uint8_t* LoadImage(const char* _filename, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _channel);

        static uint8_t* LoadImageFromMemory(const uint8_t* _ptr, size_t _size, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _req_comp);

        static void FreeData(uint8_t* _file);
    };

    class Image
    {
    public:
        struct ImageDeleter {

            ImageDeleter() = default;
            ImageDeleter(ImageDeleter&&) noexcept = default;
            ImageDeleter& operator=(ImageDeleter&&) noexcept = default;

            void operator()(uint8_t* p) const noexcept {
                FileLoader::FreeData(p);
            }
        };


        DEFAULT_COPY_MOVE_OPERATIONS(Image);

        PC_CORE_API Image(const std::string& _path, PC_CORE::RhiChannel _desireChannel);

        PC_CORE_API Image(const char* _path, PC_CORE::RhiChannel _desireChannel);

        PC_CORE_API Image(const uint8_t* _ptr, size_t _size, const char* _name);

        Image() = default;

        ~Image() = default;


        uint32_t GetWidht() const
        {
            return m_Widht;
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

        [[nodiscard]] std::unique_ptr<uint8_t[], ImageDeleter> Release() noexcept
        {
            auto tmp = std::move(m_Data);
            m_Data.reset();
            return tmp;
        }


    private:
       
        REFLECT(Image);

        uint32_t m_Widht = -1;

        uint32_t m_Height = -1;

        PC_CORE::RhiChannel m_Channel{};

        std::unique_ptr<uint8_t[], ImageDeleter> m_Data;

        bool m_IsHDR = false;
    };

END_PCCORE
