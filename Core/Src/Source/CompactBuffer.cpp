#include "CompactBuffer.hpp"

#define LZAV_NS_CUSTOM lzav

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

std::vector<uint8_t> CompactBuffer::ExtractData()
{
    assert(m_CompressedDataSize != 0);

    std::vector<uint8_t> buffer;

    // allocate destination buffer (could be larger than actual uncompressed size)
    buffer.resize(m_Data.size());

    int l = lzav_decompress(m_Data.data(), buffer.data(), m_CompressedDataSize, buffer.size());

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

    buffer.resize(l);
    return buffer;
}

