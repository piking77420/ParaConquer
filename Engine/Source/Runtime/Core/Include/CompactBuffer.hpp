#pragma once

#include <memory>
#include "Reflection/Reflector.hpp"

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

		const size_t l = Decompress(m_Data.data(), buffer.data(), m_CompressedDataSize, buffer.size() * sizeof(T));

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

	size_t Decompress(const void* const _ptrSrc, void* const _ptrDst,
		const int _srcL, const int _dstL);
	
	REFLECT(CompactBuffer);
	REFLECT_MEMBER(CompactBuffer, m_Data);
	REFLECT_MEMBER(CompactBuffer, m_CompressedDataSize);

};

END_PCCORE