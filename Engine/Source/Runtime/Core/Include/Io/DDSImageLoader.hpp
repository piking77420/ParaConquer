#ifndef PC_CORE_DDS_IMAGELOADER
#define PC_CORE_DDS_IMAGELOADER

#include <CoreHeader.hpp>
#include <filesystem>
#include <LowRenderer/RhiTypedef.h>

namespace PC_CORE
{
	class PC_CORE_API DDSImageLoader
	{
	public:
		explicit DDSImageLoader() = default;
		~DDSImageLoader() = default;
		DDSImageLoader(const std::filesystem::path& _Path);
		struct DdsMip {
			uint32_t width;
			uint32_t height;
			size_t offset;
			size_t size;
		};

		struct DdsTexture {
			uint32_t width = 0;
			uint32_t height = 0;
			uint32_t mipLevels = 1;
			RhiFormat format;
			std::vector<uint8_t> pixels;   // contiguous image data, no DDS header
			std::vector<DdsMip> mips;
		};

	private:
		std::vector<char> File;
	};


}

#endif // !PC_CORE_DDS_IMAGELOADER
