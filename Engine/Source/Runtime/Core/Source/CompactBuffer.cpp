#include "CompactBuffer.hpp"

#define LZAV_NS_CUSTOM lzav
#include "lzav.h"


using namespace PC_CORE;

void CompactBuffer::CompressData(const void* _src, size_t _size)
{
    assert(_size != 0 && _src != nullptr);

    if (_size == 0 || !_src)
    {
        PC_LOGERROR("Source size equal 0 or source pointer null");
        return;
    }

    if (_size > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        PC_LOGERROR("Source size too large to compress");
        return;
    }

    const int sizeInt = static_cast<int>(_size);

    const int max_len = lzav::lzav_compress_bound_hi(sizeInt);
    m_Data.resize(static_cast<size_t>(max_len));

    const int outLength = lzav::lzav_compress_hi(_src, m_Data.data(), sizeInt, max_len);

    if (outLength <= 0)
    {
        PC_LOGERROR("Compress size equal 0 or compression failed");
        m_Data.clear();
        m_CompressedDataSize = 0;
        return;
    }

    m_CompressedDataSize = static_cast<size_t>(outLength);
    m_Data.resize(m_CompressedDataSize);
}

size_t CompactBuffer::Decompress(const void* const _ptrSrc, void* const _ptrDst, const int _srcL, const int _dstL)
{
    const int l = lzav::lzav_decompress(_ptrSrc, _ptrDst, _srcL, _dstL);

    if (l < 0)
    {
        switch (l) {
        case lzav::enum_wrapper::LZAV_E_PARAMS:
            PC_LOGERROR("lzav_decompress incorrect param");
            break;
        case lzav::enum_wrapper::LZAV_E_UNKFMT:
            PC_LOGERROR("lzav_decompress Unknown Stream format");
            break;
        default:
            PC_LOGERROR("lzav_decompress failed with code %d", l);
            break;
        }

        return static_cast<size_t>(0);
    }

    return static_cast<size_t>(l);
}

std::vector<uint8_t> CompactBuffer::ExtractData()
{
    assert(m_CompressedDataSize != 0);

    std::vector<uint8_t> buffer;

    // allocate destination buffer (could be larger than actual uncompressed size)
    buffer.resize(m_Data.size());

    size_t l = Decompress(m_Data.data(), buffer.data(), m_CompressedDataSize, buffer.size());

    buffer.resize(l);
    return buffer;
}

