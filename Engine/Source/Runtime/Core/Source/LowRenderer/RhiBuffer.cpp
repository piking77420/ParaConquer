#include "LowRenderer/RhiBuffer.h"

PC_CORE::RhiBuffer::RhiBuffer(Rhi& _Rhi, const std::string& _name)
    : RhiResource(_Rhi, _name)
{
}

PC_CORE::RhiBuffer::RhiBuffer(Rhi& _Rhi, std::string&& _name)
    : RhiResource(_Rhi, _name)
{
    
}