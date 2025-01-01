#include "data_structures//sparse_sets.hpp"

#include <stdexcept>

#include "log.hpp"


std::vector<uint8_t>* PC_CORE::SparseSet::GetData()
{
    return &m_Dense;
}

const std::vector<uint8_t>* PC_CORE::SparseSet::GetData() const
{
    return &m_Dense;
}


uint8_t* PC_CORE::SparseSet::Alloc(size_t _index)
{
  // TO DO PAGGING
    const size_t pageIndex = _index / PAGE_SIZE;
    const size_t offset = _index % PAGE_SIZE;
    
    if (m_SparseList.size() <= pageIndex)
        m_SparseList.resize(pageIndex + 1);
    
    std::vector<size_t>& page = m_SparseList[pageIndex];
    
    if (page.size() < PAGE_SIZE)
    {
        page.resize(PAGE_SIZE);
        for (size_t& i : page)
            i = NULL_INDEX;
    }

    
    if (page.at(offset) != NULL_INDEX)
    {
        PC_LOGERROR("This entity Has already have index")
        return nullptr;
    }
    const size_t denseSize = m_Dense.size();
    page.at(offset) = static_cast<uint32_t>(m_Dense.size());
    m_Dense.resize(denseSize + m_Density);
    uint8_t* newItemData = m_Dense.data() + denseSize;
    
    return newItemData;
}

void PC_CORE::SparseSet::Free(size_t _index)
{
    if (Empty())
        return;

    // Index of item that we want to delete
    const size_t pageIndex = _index / PAGE_SIZE;
    const size_t offset = _index % PAGE_SIZE;

    if (pageIndex <= m_SparseList.size())
    {
        PC_LOGERROR("pageIndex outside of sparse list")
        return;
    }
    
    std::vector<size_t>& page = m_SparseList[pageIndex];

    if (page.size() >= offset)
    {
        PC_LOGERROR("offset is grater than page size")
        return;
    }

    const size_t elementToDeleteIndexInDenseList = m_SparseList[pageIndex][offset];
    
    // Look for the last element page and offset
    size_t lastIndexPage = NULL_INDEX;
    size_t lastIndexOffset = NULL_INDEX;
    
    size_t lastIndexInDenseList = NULL_INDEX;

    for (size_t i = 0; i < m_SparseList.size(); i++)
    {
        for (size_t j = 0; j < m_SparseList[i].size(); j++)
        {
            if (m_SparseList[i][j] == NULL_INDEX)
                continue;

            if (m_SparseList[i][j] < lastIndexInDenseList)
                continue;
            
            lastIndexInDenseList = m_SparseList[i][j];
            lastIndexPage = i;
            lastIndexOffset = j;
        }
    }
    
    std::swap(m_Dense[lastIndexInDenseList], m_Dense[elementToDeleteIndexInDenseList]);
    m_Dense.pop_back();
    
    m_SparseList[pageIndex][offset] = m_SparseList[lastIndexPage][lastIndexOffset];
    m_SparseList[pageIndex][offset] = NULL_INDEX;
}

const uint8_t* PC_CORE::SparseSet::At(size_t _index) const
{
    const size_t pageIndex = _index / PAGE_SIZE;
    const size_t offset = _index % PAGE_SIZE;
    const size_t denseIndex = m_SparseList.at(pageIndex).at(offset);
    
    if (denseIndex == NULL_INDEX)
        return nullptr;

    return &m_Dense.at(denseIndex);
}

uint8_t* PC_CORE::SparseSet::At(size_t _index)
{
    const size_t pageIndex = _index / PAGE_SIZE;
    const size_t offset = _index % PAGE_SIZE;
    const size_t denseIndex = m_SparseList.at(pageIndex).at(offset);
    
    if (denseIndex == NULL_INDEX)
        return nullptr;

    return &m_Dense.at(denseIndex);
}

bool PC_CORE::SparseSet::Empty() const
{
    return m_Dense.empty();
}

PC_CORE::SparseSet::SparseSet(size_t _densitySize) : m_Density(_densitySize)
{
}


