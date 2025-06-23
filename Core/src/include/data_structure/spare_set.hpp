#pragma once

#include <vector>

#include "core_header.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE
template <class T>
class SpareSet
{
public:
    void Add(size_t _sparseId, const T& _value);

    void Remove(size_t _sparseId);

    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    // TO DO BET ITERATOR
    // ITERATE OVER AVAILABLE INDDICIES
    iterator begin() { return m_Dense.begin(); }
    iterator end() { return m_Dense.end(); }

    const_iterator begin() const { return m_Dense.begin(); }
    const_iterator end() const { return m_Dense.end(); }

    const_iterator cbegin() const { return m_Dense.cbegin(); }
    const_iterator cend() const { return m_Dense.cend(); }

private:
    std::vector<uint8_t> m_Dense;
    
    std::vector<size_t> m_Sparse;

};

template <class T>
void SpareSet<T>::Add(size_t _sparseId, const T& _value)
{
    if (_sparseId >= m_Sparse.size())
        m_Sparse.resize(_sparseId + 1, std::numeric_limits<size_t>::max()); // default to invalid

    // If already present, update the value
    if (m_Sparse[_sparseId] != std::numeric_limits<size_t>::max())
    {
        m_Dense[m_Sparse[_sparseId]] = _value;
        return;
    }

    size_t denseIndex = m_Dense.size();
    m_Sparse[_sparseId] = denseIndex;
    m_Dense.push_back(_value);
}

template <class T>
void SpareSet<T>::Remove(size_t _sparseId)
{
    if (_sparseId >= m_Sparse.size() || m_Sparse[_sparseId] == std::numeric_limits<size_t>::max())
        return; // Not present

    size_t denseIndex = m_Sparse[_sparseId];
    size_t lastDenseIndex = m_Dense.size() - 1;

    if (denseIndex != lastDenseIndex)
    {
        std::swap(m_Dense[denseIndex], m_Dense[lastDenseIndex]);
        // update current indicies
        m_Sparse[lastDenseIndex] = denseIndex;
    }

    m_Dense.pop_back();
    m_Sparse[_sparseId] = std::numeric_limits<size_t>::max();
}


END_PCCORE
