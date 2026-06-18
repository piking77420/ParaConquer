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

    PC_CORE::Image::Image(std::string_view _ImageName, const uint8_t* _ptr, size_t _size, const char* _name, PC_CORE::RhiChannel _Channel)
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Core);

        if (_ImageName.find(".dds") != std::string::npos) // dirty do an enum
        {
            DDSImageLoader ddsLoder(_ptr, _size, _ImageName);

            const auto& ddsTexture = ddsLoder.GetDDSTexture();
            m_MipsDescriptor.reserve(ddsTexture.mips.size());
            for (size_t i = 0; i < ddsTexture.mips.size(); i++)
            {
                auto& mip = m_MipsDescriptor.emplace_back();
                mip.width = ddsTexture.mips[i].width;
                mip.height = ddsTexture.mips[i].height;
                mip.offset = ddsTexture.mips[i].offset;
                mip.size = ddsTexture.mips[i].size;
            }
            m_Data.reset(ddsLoder.GetPixel());
            m_BuildInFormat = ddsTexture.format;
        }
        else
        {
            int x = -1;
            int y = -1;
            m_Data.reset(FileLoader::LoadImageFromMemory(_ptr, _size, &x, &y, &m_Channel, _Channel, m_IsHDR));

            if (x != -1 && y != -1)
            {
                auto& mip = m_MipsDescriptor.emplace_back();
                mip.width = static_cast<uint32_t>(x);
                mip.height = static_cast<uint32_t>(y);
                mip.offset = 0;
                mip.size = mip.width * mip.height * static_cast<int>(m_Channel) * (m_IsHDR ? sizeof(float) : sizeof(uint8_t));
            }
            else
            {
                PC_LOGERROR("Something went wrong {}", _name);
            }
        }

       
    }

    void PC_CORE::Image::LoadFromPath(const std::string_view& _view, PC_CORE::RhiChannel _desireChannel) 
    {
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Core);

        if (_view.find(".dds") != std::string::npos) // dirty do an enum
        {
            DDSImageLoader ddsLoder(_view.data());

            const auto& ddsTexture = ddsLoder.GetDDSTexture();
            m_MipsDescriptor.reserve(ddsTexture.mips.size());
            for (size_t i = 0; i < ddsTexture.mips.size(); i++)
            {
                auto& mip = m_MipsDescriptor.emplace_back();
                mip.width = ddsTexture.mips[i].width;
                mip.height = ddsTexture.mips[i].height;
                mip.offset = ddsTexture.mips[i].offset;
                mip.size = ddsTexture.mips[i].size;
            }
            m_Data.reset(ddsLoder.GetPixel());
            m_BuildInFormat = ddsTexture.format;
        }
        else
        {
            int x = -1;
            int y = -1;
            m_Data.reset(FileLoader::LoadImage(_view.data(), &x, &y, &m_Channel, _desireChannel, m_IsHDR));
            if (x != -1 && y != -1)
            {
                auto& mip = m_MipsDescriptor.emplace_back();
                mip.width = static_cast<uint32_t>(x);
                mip.height = static_cast<uint32_t>(y);
                mip.offset = 0;
                mip.size = mip.width * mip.height * static_cast<int>(m_Channel) * (m_IsHDR ? sizeof(float) : sizeof(uint8_t));
            }
            else
            {
                PC_LOGERROR("Something went wrong {}", _view);
            }
        }
        
    }

    void Image::ImageDeleter::operator()(uint8_t* p)
    {
        FileLoader::FreeData(p);
    }

} // namespace PC_CORE 
