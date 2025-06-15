#pragma once

#include <vector>

#include "core_header.hpp"

BEGIN_PCCORE


    /**
 * 
 * @tparam _Nbr 
 */
template <uint32_t _Nbr>
class SpareSet
{
public:

    static constexpr uint32_t NULL_INDEX = std::numeric_limits<uint32_t>::max();

    static_assert(_Nbr < NULL_INDEX, "_Nbr should be lower than uint32_t max");

    uint8_t* Allocate(uint32_t sparseIndex)
    {
        
    }

    SpareSet()
    {
        m_Indices.resize(_Nbr);
    }

private:
    std::vector<uint8_t> m_Data;
    
    /**
     * Vector indicies to data
     * this vector should be preallocated
     */
   std::vector<uint32_t> m_Indices;
};

END_PCCORE