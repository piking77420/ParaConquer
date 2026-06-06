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
		~DDSImageLoader();
		DDSImageLoader(const std::filesystem::path& _Path);
		DDSImageLoader(const uint8_t* _Ptr, uint32_t _Size, std::string_view _Name);
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
			std::vector<DdsMip> mips;
		};

		const DdsTexture& GetDDSTexture() const;

		uint8_t* GetPixel() {

			uint8_t* outPixel = pixels;
			pixels = nullptr;
			return outPixel;
		}
		

	private:
		uint8_t* pixels = nullptr;

		DdsTexture m_DdsTexture;

		 bool FromMemory(const uint8_t* _Ptr, uint32_t _Size);
	};


}

#endif // !PC_CORE_DDS_IMAGELOADER
