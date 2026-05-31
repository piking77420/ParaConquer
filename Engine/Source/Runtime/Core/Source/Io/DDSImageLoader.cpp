#include <Io/DDSImageLoader.hpp>

#include <Log.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>

#include <Io/FileLoader.hpp>

namespace PC_CORE
{
#define DDS_FOURCC(a,b,c,d) \
    (uint32_t(a) | (uint32_t(b) << 8) | (uint32_t(c) << 16) | (uint32_t(d) << 24))

	static constexpr uint32_t DDS_MAGIC = DDS_FOURCC('D', 'D', 'S', ' ');
	static constexpr uint32_t DDPF_ALPHAPIXELS = 0x1;
	static constexpr uint32_t DDPF_FOURCC = 0x4;
	static constexpr uint32_t DDPF_RGB = 0x40;

#pragma pack(push, 1)
	struct DDS_PIXELFORMAT {
		uint32_t size;
		uint32_t flags;
		uint32_t fourCC;
		uint32_t rgbBitCount;
		uint32_t rBitMask;
		uint32_t gBitMask;
		uint32_t bBitMask;
		uint32_t aBitMask;
	};

    struct DDS_HEADER {
        uint32_t size;
        uint32_t flags;
        uint32_t height;
        uint32_t width;
        uint32_t pitchOrLinearSize;
        uint32_t depth;
        uint32_t mipMapCount;
        uint32_t reserved1[11];
        DDS_PIXELFORMAT ddspf;
        uint32_t caps;
        uint32_t caps2;
        uint32_t caps3;
        uint32_t caps4;
        uint32_t reserved2;
    };

    struct DDS_HEADER_DXT10 {
        uint32_t dxgiFormat;
        uint32_t resourceDimension;
        uint32_t miscFlag;
        uint32_t arraySize;
        uint32_t miscFlags2;
    };
#pragma pack(pop)

    struct DdsMip {
        uint32_t width;
        uint32_t height;
        size_t offset;
        size_t size;
    };

    
    static size_t mipSizeBytes(RhiFormat format, uint32_t w, uint32_t h) {
        if (IsBcFormat(format)) {
            uint32_t bw = std::max(1u, (w + 3) / 4);
            uint32_t bh = std::max(1u, (h + 3) / 4);
            return size_t(bw) * size_t(bh) * GetBytePerPixel(format);
        }

        return size_t(w) * size_t(h) * GetBytePerPixel(format);
    }

    
    static RhiFormat dxgiToVkFormat(uint32_t dxgi) {
        switch (dxgi) {
        case 28: return RhiFormat::R8G8B8A8Unorm;
        case 29: return RhiFormat::R8G8B8A8Srgb;
/*
        case 71: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case 72: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;

        case 74: return VK_FORMAT_BC2_UNORM_BLOCK;
        case 75: return VK_FORMAT_BC2_SRGB_BLOCK;

        case 77: return VK_FORMAT_BC3_UNORM_BLOCK;
        case 78: return VK_FORMAT_BC3_SRGB_BLOCK;

        case 80: return VK_FORMAT_BC4_UNORM_BLOCK;
        case 81: return VK_FORMAT_BC4_SNORM_BLOCK;

        case 83: return VK_FORMAT_BC5_UNORM_BLOCK;
        case 84: return VK_FORMAT_BC5_SNORM_BLOCK;

        case 95: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case 96: return VK_FORMAT_BC6H_SFLOAT_BLOCK;

        case 98: return VK_FORMAT_BC7_UNORM_BLOCK;
        case 99: return VK_FORMAT_BC7_SRGB_BLOCK;
        */
        default:
            //return VK_FORMAT_UNDEFINED;
        }

        PC_LOG("Not supported dds format to rhi format");
        return RhiFormat::Undefined;
    }

    static RhiFormat ddsFormatToVkFormat(const DDS_PIXELFORMAT& pf, const char* dx10HeaderOrNull) {

        if ((pf.flags & DDPF_FOURCC) && pf.fourCC == DDS_FOURCC('D', 'X', '1', '0')) {
            auto* dx10 = reinterpret_cast<const DDS_HEADER_DXT10*>(dx10HeaderOrNull);
            return dxgiToVkFormat(dx10->dxgiFormat);
        }

        if (pf.flags & DDPF_FOURCC) {
            switch (pf.fourCC) {
            case DDS_FOURCC('D', 'X', 'T', '1'):
                return RhiFormat::BC1_RGBA_UNORM_BLOCK;
            case DDS_FOURCC('D', 'X', 'T', '3'):
                return RhiFormat::BC2_UNORM_BLOCK;
            case DDS_FOURCC('D', 'X', 'T', '5'):
                return RhiFormat::BC3_UNORM_BLOCK;
            case DDS_FOURCC('A', 'T', 'I', '1'):
            case DDS_FOURCC('B', 'C', '4', 'U'):
                return RhiFormat::BC4_UNORM_BLOCK;
            case DDS_FOURCC('A', 'T', 'I', '2'):
            case DDS_FOURCC('B', 'C', '5', 'U'):
                return RhiFormat::BC5_UNORM_BLOCK;
            }
        }

        return RhiFormat::Undefined;
    }

	DDSImageLoader::DDSImageLoader(const std::filesystem::path& _Path)
	{
        std::vector<char> File;

		if (!std::filesystem::exists(_Path))
		{
			PC_LOGERROR("This path dont exist {}", _Path.generic_string());
			return;
		}

        if (!FileLoader::ReadFile(_Path, &File))
        {
            PC_LOGERROR("Failed to load image {}", _Path.generic_string());
            return;
        }

        if (File.size() < 4 + sizeof(DDS_HEADER)) {
            PC_LOGERROR("DDS file too small");
            return;
        }
        uint32_t magic = *reinterpret_cast<const uint32_t*>(File.data());
        const DDS_HEADER* header = reinterpret_cast<const DDS_HEADER*>(File.data() + 4);

        if (header->size != 124 || header->ddspf.size != 32) {
            PC_LOGERROR("Invalid DDS header");
        }

        size_t dataOffset = 4 + sizeof(DDS_HEADER);
        const char* dx10 = nullptr;

        if ((header->ddspf.flags & DDPF_FOURCC) &&
            header->ddspf.fourCC == DDS_FOURCC('D', 'X', '1', '0')) {
            if (File.size() < dataOffset + sizeof(DDS_HEADER_DXT10)) {
                PC_LOGERROR("DDS DX10 header missing");
            }
            dx10 = File.data() + dataOffset;
            dataOffset += sizeof(DDS_HEADER_DXT10);
        }


        DdsTexture out;
        out.width = header->width;
        out.height = header->height;
        out.mipLevels = std::max(1u, header->mipMapCount);
        out.format = ddsFormatToVkFormat(header->ddspf, dx10);

        if (out.format == RhiFormat::Undefined)
        {
            PC_LOGERROR("Failed parse dds texture unsuported format {}", _Path.generic_string());
        }

        if (dataOffset >= File.size()) {
            PC_LOGERROR("DDS has no pixel data {}", _Path.generic_string());
        }
        
        const size_t pixelSize = File.end() - (File.begin() + dataOffset);
        
        pixels = std::make_unique<uint8_t[]>(pixelSize);
        std::memcpy(pixels.get(), File.data() + dataOffset, pixelSize);

        size_t offset = 0;
        uint32_t w = out.width;
        uint32_t h = out.height;

        out.mips.reserve(out.mipLevels);
        for (uint32_t mip = 0; mip < out.mipLevels; ++mip) {
            const size_t size = mipSizeBytes(out.format, w, h);

            if (offset + size > pixelSize) {
                PC_LOGERROR("DDS mip data is truncated {}", _Path.generic_string())
                return;
            }

            out.mips.emplace_back(DdsMip{
                w,
                h,
                offset,
                size
                });

            offset += size;
            w = std::max(1u, w / 2);
            h = std::max(1u, h / 2);
        }

	}
    const DDSImageLoader::DdsTexture& DDSImageLoader::GetDDSTexture() const
    {
        return m_DdsTexture;
    }
}

