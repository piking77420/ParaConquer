#include "LowRenderer/RhiFrameBuffer.hpp"


PC_CORE::RhiFrameBuffer::RhiFrameBuffer(Rhi& _Rhi, const std::string& _name, uint32_t _width, uint32_t m_height)
    : RhiObject(_Rhi, _name)
    , m_Width(_width)
    , m_Height(m_height)
{
    
}


PC_CORE::RhiFrameBuffer::RhiFrameBuffer(Rhi& _Rhi, std::string&& _name, uint32_t _width, uint32_t m_height)
    : RhiObject(_Rhi, std::move(_name))
    , m_Width(_width)
    , m_Height(m_height)
{

}
