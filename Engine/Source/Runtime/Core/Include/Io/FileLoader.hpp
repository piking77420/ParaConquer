#ifndef PC_CORE_FILELOADER
#define PC_CORE_FILELOADER

#include <CoreHeader.hpp>
#include "LowRenderer/RhiTypedef.h"

namespace PC_CORE
{
	class PC_CORE_API FileLoader
	{
	public:
		FileLoader() = delete;

		~FileLoader() = delete;
		
		static bool ReadFile(const std::filesystem::path& _path, std::vector<char>* _data);

		static bool WriteFile(const std::filesystem::path& _path, const void* _data, size_t _size,
			bool _createDirectories);

		static uint8_t* LoadImage(const char* _filename, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _channel);

		static uint8_t* LoadImageFromMemory(const uint8_t* _ptr, size_t _size, int* _x, int* _y, PC_CORE::RhiChannel* _comp, PC_CORE::RhiChannel _req_comp);

		static void FreeData(uint8_t* _file);

		static bool IsHdr(const char* _filename);

	private:

	};
}

#endif // PC_CORE_FILELOADER