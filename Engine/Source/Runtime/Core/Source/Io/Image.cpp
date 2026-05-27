#include <Io/Image.hpp>
#include <Io/FileLoader.hpp>
#include <Io/DDSImageLoader.hpp>

namespace PC_CORE 
{
    PC_CORE::Image::Image(const std::string& _path, PC_CORE::RhiChannel _desireChannel)
        : m_IsHDR(FileLoader::IsHdr(_path.c_str()))
    {
        LoadFromPath(_path, _desireChannel);
    }

    PC_CORE::Image::Image(const char* _path, PC_CORE::RhiChannel _desireChannel)
        : m_IsHDR(FileLoader::IsHdr(_path))
    {
        LoadFromPath(_path, _desireChannel);
    }

    PC_CORE::Image::Image(const uint8_t* _ptr, size_t _size, const char* _name, PC_CORE::RhiChannel _Channel)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Core);

        int x = -1;
        int y = -1;
        m_Data.reset(FileLoader::LoadImageFromMemory(_ptr, _size, &x, &y, &m_Channel, _Channel));

        if (x != -1 && y != -1)
        {
            m_Width = static_cast<uint32_t>(x);
            m_Height = static_cast<uint32_t>(y);
            ComputeDataSize();
        }
        else
        {
            PC_LOGERROR("Something went wrong {}", _name);
        }
    }

    void PC_CORE::Image::ComputeDataSize()
    {
        m_SizeInBytes = static_cast<size_t>(m_Width) *
            static_cast<size_t>(m_Height) *
            static_cast<size_t>(m_Channel) * (m_IsHDR ? sizeof(float) : sizeof(uint8_t));

    };

    void PC_CORE::Image::LoadFromPath(const std::string_view& _view, PC_CORE::RhiChannel _desireChannel) 
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Core);

        int x = -1;
        int y = -1;

        if (_view.find(".dds") != std::string::npos) // dirty do an enum
        {
            DDSImageLoader image(_view.data());
        }
        else
        {
            m_Data.reset(FileLoader::LoadImage(_view.data(), &x, &y, &m_Channel, _desireChannel));
        }

        if (x != -1 && y != -1)
        {
            m_Width = static_cast<uint32_t>(x);
            m_Height = static_cast<uint32_t>(y);
            ComputeDataSize();
        }
        else
        {
            PC_LOGERROR("Something went wrong {}", _view);
        }
    }

    void Image::ImageDeleter::operator()(uint8_t* p)
    {
        FileLoader::FreeData(p);
    }

} // namespace PC_CORE 
