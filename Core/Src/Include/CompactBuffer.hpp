#pragma once

#include <memory>
#include "Reflection/Reflector.hpp"
#include "lzav.h"

BEGIN_PCCORE

class PC_CORE_API CompactBuffer
{
public:

	DEFAULT_COPY_MOVE_OPERATIONS(CompactBuffer)

	DEFAULT_CONSTRUCTOR_DESTRUCTOR(CompactBuffer)

	template <typename T>
    void CompactDataT(const T* _src, size_t _nbr)
	{
		CompressData(_src, _nbr * sizeof(T));
	}

	void CompressData(const void* _src, size_t _size);

	// The memory liberation is let to the user
	std::vector<uint8_t> ExtractData();

	template<typename T>
	std::vector<T> ExtractData()
	{
		assert(m_CompressedDataSize != 0);

		std::vector<T> buffer;

		// allocate destination buffer (could be larger than actual uncompressed size)
		buffer.resize(m_Data.size() / sizeof(T));

		int l = lzav_decompress(m_Data.data(), buffer.data(), m_CompressedDataSize, buffer.size() * sizeof(T));

		if (l < 0)
		{
			switch (l) {
			case LZAV_E_PARAMS:
				PC_LOGERROR("lzav_decompress incorrect param");
				break;
			case LZAV_E_UNKFMT:
				PC_LOGERROR("lzav_decompress Unknown Stream format");
				break;
			default:
				PC_LOGERROR("lzav_decompress failed with code %d", l);
				break;
			}

			return {};
		}

		buffer.resize(static_cast<size_t>(l) / sizeof(T));
		return buffer;
	}


	size_t GetCompressedDataSize() const
	{
		return m_CompressedDataSize;
	}

private:
	std::vector<uint8_t> m_Data{};

	size_t m_CompressedDataSize = 0;
	
	REFLECT(CompactBuffer);
	REFLECT_MEMBER(CompactBuffer, m_Data);
	REFLECT_MEMBER(CompactBuffer, m_CompressedDataSize);

};



END_PCCORE